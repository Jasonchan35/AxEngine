module AxRender;
import :RenderRequest_Dx12;
import :RenderSystem_Dx12;
import :RenderPass_Dx12;
import :Material_Dx12;
import :GpuBuffer_Dx12;
import :RenderResourceManager_Dx12;
import :Texture_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderRequest_Dx12::RenderRequest_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	_d3dDevice = dev;
	_uploadCmdBuf_dx12.create(  dev, CommandBufferType::Direct,  "uploadCmdList"); // CommandBufferType::Copy
	_graphCmdBuf_dx12.create(   dev, CommandBufferType::Direct,  "graphCmdList");
	_computeCmdList_dx12.create(dev, CommandBufferType::Compute, "computeCmdList");
	_fence.create(dev, static_cast<u64>(_renderSeqId));
	_cpuEvent.create();

	auto& info = _renderSystem->info();
	auto* resMgr = RenderResourceManager_Dx12::s_instance();
	
	Int renderPassCount = info.renderPass.maxCount;
	_descAlloc_ColorBuffer.create(resMgr->descHeap_ColorBuffer, info.renderPass.maxColorBufferCount * renderPassCount);
	_descAlloc_DepthBuffer.create(resMgr->descHeap_DepthBuffer, info.renderPass.maxDepthBufferCount * renderPassCount);

	Int renderRequest_CBV_SRV_UAV_Count = info.renderRequest.maxConstBufferCount
										+ info.renderRequest.maxTextureCount;
	
	_descAlloc_CBV_SRV_UAV.create(resMgr->descHeap_CBV_SRV_UAV, renderRequest_CBV_SRV_UAV_Count);
	    _descAlloc_Sampler.create(resMgr->descHeap_Sampler,     info.renderRequest.maxSamplerCount);
}

void RenderRequest_Dx12::onFrameBegin() {
	_uploadCmdBuf_dx12.commandBegin();
	_graphCmdBuf_dx12.commandBegin();
	
	_descAlloc_ColorBuffer.reset();
	_descAlloc_DepthBuffer.reset();
	_descAlloc_CBV_SRV_UAV.reset();
	_descAlloc_Sampler.reset();

	auto* resMgr = RenderResourceManager_Dx12::s_instance();
	auto descHeaps = Span({resMgr->descHeap_CBV_SRV_UAV.d3dHeap(), resMgr->descHeap_Sampler.d3dHeap()});
	setDescriptorHeaps(descHeaps);
}

void RenderRequest_Dx12::onFrameEnd() {
	_updatedBindlessResources();
	_uploadCmdBuf_dx12.commandEnd();
	_graphCmdBuf_dx12.commandEnd();
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

void RenderRequest_Dx12::_updatedBindlessResources() {
#if 0
	auto* mgr = RenderResourceManager_Dx12::s_instance();

	auto* commonMaterialPass = rttiCastCheck<MaterialPass_Dx12>(_commonMaterialPass);
//	auto* bindlessParamSpace = commonMaterialPass->getParamSpace_dx12(BindSpace::Bindless);

//	bindlessParamSpace->getUpdatedPerFrameData()
	
	for (auto& tex_ : updatedBindlessResources.texture2Ds) {
		auto* tex = rttiCastCheck<Texture2D_Dx12>(tex_.ptr());
		if (!tex) throw Error_Undefined();

		Int index = ax_enum_int(tex->resourceHandle.slotId());
		auto handle =   mgr->bindlessHeap_CBV_SRV_UAV.getHandle(index);
		auto& texResource = tex->_bindImage(this);
		_d3dDevice->CreateShaderResourceView(texResource, nullptr, handle.cpu);
	}

	for (auto& sampler_ : updatedBindlessResources.samplers) {
		auto* sampler = rttiCastCheck<Sampler_Dx12>(sampler_.ptr());
		if (!sampler) throw Error_Undefined();

		Int index = ax_enum_int(sampler->resourceHandle.slotId());
		auto handle = mgr->bindlessHeap_Sampler.getHandle(index);

		auto& ss = sampler->samplerState();
		D3D12_SAMPLER_DESC desc;
		desc.Filter           = Dx12Util::getDxSamplerFilter(ss.filter);
		desc.AddressU         = Dx12Util::getDxSamplerWrap(ss.wrap.u);
		desc.AddressV         = Dx12Util::getDxSamplerWrap(ss.wrap.v);
		desc.AddressW         = Dx12Util::getDxSamplerWrap(ss.wrap.w);
		desc.MipLODBias       = 0;
		desc.MaxAnisotropy    = 0;
		desc.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
		desc.BorderColor[0]   = 0;
		desc.BorderColor[1]   = 0;
		desc.BorderColor[2]   = 0;
		desc.BorderColor[3]   = 0;
		desc.MinLOD           = 0.0f;
		desc.MaxLOD           = D3D12_FLOAT32_MAX;
		
		_d3dDevice->CreateSampler(&desc, handle.cpu);
	}
#endif
}

void RenderRequest_Dx12::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	D3D12_VIEWPORT tmp;
	tmp.TopLeftX = rect.x;
	tmp.TopLeftY = rect.y;
	tmp.Width    = rect.w;
	tmp.Height   = rect.h;
	tmp.MinDepth = minDepth;
	tmp.MaxDepth = maxDepth;
	_graphCmdBuf_dx12->RSSetViewports(1, &tmp);
}

void RenderRequest_Dx12::onSetScissorRect(const Rect2f& rect) {
	D3D12_RECT tmp;
	tmp.left   = static_cast<LONG>(rect.xMin());
	tmp.top    = static_cast<LONG>(rect.yMin());
	tmp.right  = static_cast<LONG>(rect.xMax());
	tmp.bottom = static_cast<LONG>(rect.yMax());
	_graphCmdBuf_dx12->RSSetScissorRects(1, &tmp);
}

void RenderRequest_Dx12::onDrawCall(Cmd_DrawCall& drawcall) {
	auto& cmdList = _graphCmdBuf_dx12;
	
	D3D12_PRIMITIVE_TOPOLOGY topology  = Dx12Util::getDxPrimitiveTopology(drawcall.primitiveType);
	cmdList->IASetPrimitiveTopology(topology); // already in pso

	{ // bind vertex buffer
		auto vertexLayout = drawcall.vertexLayout;

		auto* vb = ax_const_cast(rttiCastCheck<GpuBuffer_Dx12>(drawcall.vertexBuffer));
		if (!vb) throw Error_Undefined();

		vb->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		D3D12_VERTEX_BUFFER_VIEW vbView = {};
		vbView.BufferLocation = ax_safe_cast_from(vb->resource().gpuAddress());
		vbView.SizeInBytes    = ax_safe_cast_from(vb->bufferSize());
		vbView.StrideInBytes  = ax_safe_cast_from(vertexLayout->strideInBytes);
		cmdList->IASetVertexBuffers(0, 1, &vbView);
	}
	
	if (drawcall.indexType == IndexType::None) {
		cmdList->DrawInstanced(Dx12Util::castUINT(drawcall.vertexCount),
		                       ax_safe_cast_from(drawcall.instanceCount),
		                       ax_safe_cast_from(drawcall.vertexStart),
		                       ax_safe_cast_from(drawcall.instanceStart));
		
	} else {
		auto* ib = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(drawcall.indexBuffer));
		if (!ib) throw Error_Undefined();

		ib->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		D3D12_INDEX_BUFFER_VIEW ibView = {};
		ibView.BufferLocation          = Dx12Util::castUINT64(ib->resource().gpuAddress());
		ibView.SizeInBytes             = Dx12Util::castUINT(ib->bufferSize());
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
			auto h = _descAlloc_ColorBuffer.addRenderTargetView(colorBuf->_resource_dx12);
			rtViews.emplaceBack(h.handle.cpu);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE depthView;
	{
		auto* depthBuf = rttiCastCheck<RenderPassDepthBuffer_Dx12>(pass->depthAttachment().buffer.ptr());
		depthView = _descAlloc_DepthBuffer.addDepthStencilView(depthBuf->_resource_dx12).handle.cpu;
	}

	//------
	auto& cmdBuf         = _graphCmdBuf_dx12;
	BOOL  rtSingleHandle = FALSE;
	cmdBuf->OMSetRenderTargets(ax_safe_cast_from(rtViews.size()), rtViews.data(), rtSingleHandle, &depthView);

	Int rtIndex = 0;
	for (auto& colorAttachment : pass->colorAttachments()) {
		auto& desc = colorAttachment.desc;
		auto* colorBuffer = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorAttachment.buffer.ptr());
		if (colorBuffer) {
			if (desc.loadOp == RenderBufferLoadOp::Clear) {
				cmdBuf->ClearRenderTargetView(	rtViews[rtIndex],
												desc.clearColor.data(),
												0,
												nullptr);
			}
		}
		++rtIndex;
	}

	if (auto& desc = pass->depthAttachment().desc) {
		if (desc.loadOp == RenderBufferLoadOp::Clear) {
			cmdBuf->ClearDepthStencilView(depthView,
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