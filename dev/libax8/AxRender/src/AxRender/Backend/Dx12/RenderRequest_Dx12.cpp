module AxRender;
import :RenderRequest_Dx12;
import :RenderSystem_Dx12;
import :RenderPass_Dx12;
import :Material_Dx12;
import :GpuBuffer_Dx12;

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

	_heap_ColorBuffer.create(256);
	_heap_DepthBuffer.create(64);
	_heap_CBV_SRV_UAV.create(4000);
	_heap_sampler.create(4000);
}

void RenderRequest_Dx12::onFrameBegin() {
	_uploadCmdBuf_dx12.commandBegin();
	_graphCmdBuf_dx12.commandBegin();
	
	_heap_ColorBuffer.reset();
	_heap_DepthBuffer.reset();
	_heap_CBV_SRV_UAV.reset();
	_heap_sampler.reset();

	_currentDescHeaps.clear();
}

void RenderRequest_Dx12::onFrameEnd() {
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
		Dx12DescripterHeap_ColorBuffer::Block heapBlock;
		_heap_ColorBuffer.allocaBlock(heapBlock, _d3dDevice, pass->colorAttachments().size());
		for (auto& colorAttachment : pass->colorAttachments()) {
			auto* colorBuf = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorAttachment.buffer.ptr());
			auto h = heapBlock.addRenderTargetView(_d3dDevice, colorBuf->_resource_dx12);
			rtViews.emplaceBack(h.handle.cpu);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE depthView;
	{
		Dx12DescripterHeap_DepthBuffer::Block heapBlock;
		_heap_DepthBuffer.allocaBlock(heapBlock, _d3dDevice, pass->colorAttachments().size());
		auto* depthBuf = rttiCastCheck<RenderPassDepthBuffer_Dx12>(pass->depthAttachment().buffer.ptr());
		depthView = heapBlock.addDepthStencilView(_d3dDevice, depthBuf->_resource_dx12).handle.cpu;
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