module AxRender;
import :RenderRequest_Dx12;
import :RenderSystem_Dx12;
import :RenderPass_Dx12;
import :Material_Dx12;
import :GpuBuffer_Dx12;
import :RenderObjectManager_Dx12;
import :Texture_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderRequest_Dx12::RenderRequest_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	AX_LOG("create {}", _name);
	
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	_d3dDevice = dev;

	_uploadCmdList_dx12.create( dev, RenderCommandListType::Direct,  "uploadCmdList" ); // RenderCommandListType::Copy
	_graphCmdList_dx12.create(  dev, RenderCommandListType::Direct,  "graphCmdList"  );
	_computeCmdList_dx12.create(dev, RenderCommandListType::Compute, "computeCmdList");
	_fence.create(dev, static_cast<u64>(_renderSeqId));
	_cpuEvent.create();

	auto& info = _renderSystem->info();
	auto* resMgr = RenderObjectManager_Dx12::s_instance();

	_descriptorHeapPools = &resMgr->descriptorHeapPools;
	_resourceDescriptors = &resMgr->resourceDescriptors;
	
#if AX_RENDER_BINDLESS
	_bindlessDescriptors = &resMgr->bindlessDescriptors;
#endif

	auto& pool = resMgr->descriptorHeapPools;
	auto& info_pass = info.renderPass;
	auto& info_req  = info.renderRequest;

	_dynamicDescriptors.ColorBuffer.create(Fmt("dynamic#{}.ColorBuffer", desc.index), pool.ColorBuffer, info_pass.maxColorBufferCount * info_pass.maxCount, false);
	_dynamicDescriptors.DepthBuffer.create(Fmt("dynamic#{}.DepthBuffer", desc.index), pool.DepthBuffer, info_pass.maxDepthBufferCount * info_pass.maxCount, false);

	Int renderReq_CBV_SRV_UAV_Count = info_req.maxConstBufferCount + info_req.maxTextureCount;
	_dynamicDescriptors.CBV_SRV_UAV.create(Fmt("dynamic#{}.CBV_SRV_UAV", desc.index), pool.CBV_SRV_UAV, renderReq_CBV_SRV_UAV_Count       , false);
	    _dynamicDescriptors.Sampler.create(Fmt("dynamic#{}.Sampler"    , desc.index), pool.Sampler,     info.renderRequest.maxSamplerCount, false);
	
	
	StructuredGpuBuffer_CreateDesc createDesc;
	createDesc.name   = AX_NAMEID("indirectDraw.drawArguments");
	createDesc.stride = AX_SIZEOF(Dx12_IndirectDrawArgument);
//	createDesc.count  = info.indirectDraw.maxDrawCount;
	indirectDraw.drawArguments = StructuredGpuBuffer::s_new(AX_NEW, createDesc);
}

void RenderRequest_Dx12::onFrameBegin() {
	_uploadCmdList_dx12.commandBegin();
	_graphCmdList_dx12.commandBegin();

	_dynamicDescriptors.ColorBuffer.reset();
	_dynamicDescriptors.DepthBuffer.reset();
	_dynamicDescriptors.CBV_SRV_UAV.reset();
	_dynamicDescriptors.Sampler.reset();
	
	auto& heapPool = RenderObjectManager_Dx12::s_instance()->descriptorHeapPools;
	ID3D12DescriptorHeap* descHeaps[] = {heapPool.CBV_SRV_UAV.d3dHeap(), heapPool.Sampler.d3dHeap()};
	setDescriptorHeaps(descHeaps);
}

void RenderRequest_Dx12::onFrameEnd() {
	_uploadCmdList_dx12.commandEnd();
	_graphCmdList_dx12.commandEnd();
}

void RenderRequest_Dx12::onWaitCompleted() {
	u64 fenceValue = fenceValue_dx12();
	if (_fence.getCompletedValue() < fenceValue) {
		_cpuEvent.signalOnFenceCompletion(_fence, fenceValue);
		
		if (!_cpuEvent.wait(AxRenderConfig::kMaxRenderWaitTime())) {
			throw Error_Undefined("Render - timeout");
		}
	}
}

void RenderRequest_Dx12::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	D3D12_VIEWPORT tmp;
	tmp.TopLeftX = rect.x;
	tmp.TopLeftY = rect.y;
	tmp.Width    = rect.w;
	tmp.Height   = rect.h;
	tmp.MinDepth = minDepth;
	tmp.MaxDepth = maxDepth;
	_graphCmdList_dx12->RSSetViewports(1, &tmp);
}

void RenderRequest_Dx12::onSetScissorRect(const Rect2f& rect) {
	D3D12_RECT tmp;
	tmp.left   = static_cast<LONG>(rect.xMin());
	tmp.top    = static_cast<LONG>(rect.yMin());
	tmp.right  = static_cast<LONG>(rect.xMax());
	tmp.bottom = static_cast<LONG>(rect.yMax());
	_graphCmdList_dx12->RSSetScissorRects(1, &tmp);
}

void RenderRequest_Dx12::onDispatchMesh(AxDrawCallDesc& cmd, u32x3 groupCount) {
	_graphCmdList_dx12->DispatchMesh(groupCount.x, groupCount.y, groupCount.z);
}

void RenderRequest_Dx12::onDrawCall(AxDrawCallDesc& drawcall) {
	auto* mat = rttiCastCheck<Material_Dx12>(drawcall.material);
	if (!mat) return;
	auto* matPass = mat->getPass(drawcall.materialPassIndex);
	if (!matPass) return;

	auto& cmdList = _graphCmdList_dx12;

	D3D12_PRIMITIVE_TOPOLOGY topology  = Dx12Util::getDxPrimitiveTopology(drawcall.primitiveType);
	cmdList->IASetPrimitiveTopology(topology); // already in pso

	{ // bind vertex buffer
		auto vertexLayout = drawcall.vertexLayout;

		auto* vb = ax_const_cast(rttiCastCheck<GpuBuffer_Dx12>(drawcall.vertexBuffer));
		if (!vb) throw Error_Undefined();

		vb->updateResourceBarrier(cmdList);

		D3D12_VERTEX_BUFFER_VIEW vbView = {};
		vbView.BufferLocation = ax_safe_cast_from(vb->gpuAddress() + vb->bufferOffset());
		vbView.SizeInBytes    = ax_safe_cast_from(vb->size());
		vbView.StrideInBytes  = ax_safe_cast_from(vertexLayout->strideInBytes);
		cmdList->IASetVertexBuffers(0, 1, &vbView);
	}
	
	if (drawcall.indexType == VertexIndexType::None) {
		cmdList->DrawInstanced(Dx12Util::castUINT(drawcall.vertexCount),
		                       ax_safe_cast_from(drawcall.instanceCount),
		                       ax_safe_cast_from(drawcall.vertexStart),
		                       ax_safe_cast_from(drawcall.instanceStart));
		
	} else {
		auto* ib = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(drawcall.indexBuffer));
		if (!ib) throw Error_Undefined();

		ib->updateResourceBarrier(cmdList);

		D3D12_INDEX_BUFFER_VIEW ibView = {};
		ibView.BufferLocation          = ib->gpuAddress() + ib->bufferOffset();
		ibView.SizeInBytes             = Dx12Util::castUINT(ib->size());
		ibView.Format                  = Dx12Util::getDxIndexType(drawcall.indexType);;

		cmdList->IASetIndexBuffer(&ibView);
		cmdList->DrawIndexedInstanced(Dx12Util::castUINT(drawcall.indexCount),
		                              ax_safe_cast_from(drawcall.instanceCount),
		                              ax_safe_cast_from(drawcall.indexStart),
		                              ax_safe_cast_from(drawcall.vertexStart),
		                              ax_safe_cast_from(drawcall.instanceStart));
	}
}

void RenderRequest_Dx12::onRenderPassBegin(RenderPass* pass_) {
	auto* pass = rttiCastCheck<RenderPass_Dx12>(pass_);
	pass->colorBuf0_resourceBarrier(this, D3D12_RESOURCE_STATE_RENDER_TARGET);

	Array<D3D12_CPU_DESCRIPTOR_HANDLE, 16> rtViews;
	{
		for (auto& colorAttachment : pass->colorAttachments()) {
			auto* colorBuf = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorAttachment.buffer.ptr());
			auto h = _dynamicDescriptors.ColorBuffer.addRenderTargetView(colorBuf->_resource_dx12);
			rtViews.emplaceBack(h.handle.cpu);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE depthView;
	{
		auto* depthBuf = rttiCastCheck<RenderPassDepthBuffer_Dx12>(pass->depthAttachment().buffer.ptr());
		depthView = _dynamicDescriptors.DepthBuffer.addDepthStencilView(depthBuf->_resource_dx12).handle.cpu;
	}

	//------
	auto& cmdList = _graphCmdList_dx12;
	BOOL  rtSingleHandle = FALSE;
	cmdList->OMSetRenderTargets(ax_safe_cast_from(rtViews.size()), rtViews.data(), rtSingleHandle, &depthView);

	Int rtIndex = 0;
	for (auto& colorAttachment : pass->colorAttachments()) {
		auto& desc = colorAttachment.desc;
		auto* colorBuffer = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorAttachment.buffer.ptr());
		if (colorBuffer) {
			if (desc.loadOp == RenderBufferLoadOp::Clear) {
				cmdList->ClearRenderTargetView(	rtViews[rtIndex],
												desc.clearColor.data(),
												0,
												nullptr);
			}
		}
		++rtIndex;
	}

	if (auto& desc = pass->depthAttachment().desc) {
		if (desc.loadOp == RenderBufferLoadOp::Clear) {
			cmdList->ClearDepthStencilView(depthView,
			                              D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			                              desc.clearDepth,
			                              ax_safe_cast_to<u8>(desc.clearStencil),
			                              0,
			                              nullptr);
		}
	}
}

void RenderRequest_Dx12::onRenderPassEnd(RenderPass* pass_) {
	auto* pass = rttiCastCheck<RenderPass_Dx12>(pass_);
	pass->colorBuf0_resourceBarrier(this, D3D12_RESOURCE_STATE_PRESENT);
}



} // namespace

#endif // #if AX_RENDERER_DX12