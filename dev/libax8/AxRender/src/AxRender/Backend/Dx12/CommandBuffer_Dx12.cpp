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

	Int colorBufferCount = pass->colorBufferCount();
	for (Int i = 0; i < colorBufferCount; ++i) {
		auto* colorBuf = pass->colorBuffer(i);
		if (!colorBuf) continue;
		//		colorBuf->
	}
	
	// for (auto& colorBuf : pass->_colorBuffers) {
	// 	
	// }
	
	
	auto& renderTargetDescriptors = pass->_renderTargetDescriptors;
	BOOL RTsSingleHandleToDescriptorRange = FALSE;
	_cmdList_dx12->OMSetRenderTargets(
		SafeCast(renderTargetDescriptors.size()),
		renderTargetDescriptors.data(),
		RTsSingleHandleToDescriptorRange,
		&pass->_depthStencilDescriptor);
}

void CommandBuffer_Dx12::onRenderPassEnd() {
	// TODO
}

} // namespace

#endif // #if AX_RENDERER_DX12
 