module AxRender;

#if AX_RENDERER_DX12

import :CommandBuffer_Dx12;
import :RenderPass_Dx12;

namespace ax /*::AxRender*/ {

void CommandBuffer_Dx12::create(ID3D12Device* dev, CommandBufferType type) {

	D3D12_COMMAND_LIST_TYPE type_dx12 = Dx12Util::getDxCommandBufferType(type);
	
	auto hr = dev->CreateCommandAllocator(type_dx12, IID_PPV_ARGS(_cmdAllocator_dx12.ptrForInit()));
	Dx12Util::throwIfError(hr);

	hr = dev->CreateCommandList(0, type_dx12, _cmdAllocator_dx12, nullptr, IID_PPV_ARGS(_cmdList_dx12.ptrForInit()));
	Dx12Util::throwIfError(hr);

	hr = _cmdList_dx12->Close();
	Dx12Util::throwIfError(hr);	
}

void CommandBuffer_Dx12::onCommandBegin() {
	auto hr = _cmdAllocator_dx12->Reset();
	Dx12Util::throwIfError(hr);
	
	ID3D12PipelineState* pso = nullptr;
	hr = _cmdList_dx12->Reset(_cmdAllocator_dx12, pso);
	Dx12Util::throwIfError(hr);
}

void CommandBuffer_Dx12::onCommandEnd() {
	auto hr = _cmdList_dx12->Close();
	Dx12Util::throwIfError(hr);	
}

void CommandBuffer_Dx12::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	D3D12_VIEWPORT tmp;
	tmp.TopLeftX = rect.x;
	tmp.TopLeftY = rect.y;
	tmp.Width    = rect.w;
	tmp.Height   = rect.h;
	tmp.MinDepth = minDepth;
	tmp.MaxDepth = maxDepth;
	_cmdList_dx12->RSSetViewports(1, &tmp);
}

void CommandBuffer_Dx12::onSetScissorRect(const Rect2f& rect) {
	D3D12_RECT tmp;
	tmp.left   = static_cast<LONG>(rect.xMin());
	tmp.top    = static_cast<LONG>(rect.yMin());
	tmp.right  = static_cast<LONG>(rect.xMax());
	tmp.bottom = static_cast<LONG>(rect.yMax());
	_cmdList_dx12->RSSetScissorRects(1, &tmp);
}

void CommandBuffer_Dx12::onDrawCall(Cmd_DrawCall& cmd) {
	// TODO
}

void CommandBuffer_Dx12::onRenderPassBegin(RenderPass* pass_) {
	auto* pass = rttiCastCheck<RenderPass_Dx12>(pass_);

	for (auto& colorBuf : pass->colorBuffers()) {
		auto* colorBuf_dx12 = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorBuf.buffer.ptr());
		colorBuf_dx12->_resource_dx12.resourceBarrier(_cmdList_dx12, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	auto& renderTargetDescriptors = pass->_colorViewList_dx12;
	BOOL RTsSingleHandleToDescriptorRange = FALSE;
	_cmdList_dx12->OMSetRenderTargets(
		SafeCast(renderTargetDescriptors.size()),
		renderTargetDescriptors.data(),
		RTsSingleHandleToDescriptorRange,
		&pass->_depthView_dx12);
}

void CommandBuffer_Dx12::onRenderPassEnd() {
	// TODO
	// for (auto& colorBuf : pass->colorBuffers()) {
	// 	auto* colorBuf_dx12 = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorBuf.buffer.ptr());
	// 	colorBuf_dx12->_resource_dx12.resourceBarrier(_cmdList_dx12, D3D12_RESOURCE_STATE_PRESENT);
	// }	
}

} // namespace

#endif // #if AX_RENDERER_DX12
 