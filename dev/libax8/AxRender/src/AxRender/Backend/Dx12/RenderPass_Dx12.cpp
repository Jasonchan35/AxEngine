module AxRender;

#if AX_RENDERER_DX12
import :RenderPass_Dx12;
import :RenderContext_Dx12;

namespace ax {

RenderPassColorBuffer_Dx12::RenderPassColorBuffer_Dx12(const CreateDesc& desc): Base(desc) {
	if (desc.backBufferRef) {
		auto* renderContext_dx12 = rttiCastCheck<RenderContext_Dx12>(desc.backBufferRef.renderContext);
		if (!renderContext_dx12) throw Error_Undefined();
		UINT backBufIndex = SafeCast(desc.backBufferRef.index);
		createFromSwapChain(renderContext_dx12->_swapChain_dx12.ptr(), backBufIndex);
	}
}

void RenderPassColorBuffer_Dx12::createFromSwapChain(AX_DX12_IDXGISwapChain* swapChain, UINT backBufIndex) {
	_d3dResource.createFromSwapChain(swapChain, backBufIndex);
	_descHeap.create(1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	_renderTargetView = _descHeap.getHandle(0);
	Renderer_Dx12::s_d3dDevice()->CreateRenderTargetView(_d3dResource.d3dResource(), nullptr, _renderTargetView);
}


RenderPass_Dx12::RenderPass_Dx12(const CreateDesc& desc)
: Base(desc)
{
	RenderContext_Dx12*					 renderContext_dx12 = nullptr;
	RenderContext_Dx12::BackBuffer_Dx12* backBuffer_dx12    = nullptr;
	if (desc.isBackBuffer) {
		renderContext_dx12	= rttiCastCheck<RenderContext_Dx12>(desc.renderContext);
		backBuffer_dx12		= renderContext_dx12->_getBackBuffer(desc.backBufferIndex);
	}
	
	//---- color buffers ----
	const Int colorBufferCount = desc.colorBufferAttachments.size();
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& colorBuf = _colorBuffers.emplaceBack();
		auto& colorBufferAttachment = desc.colorBufferAttachments[i];
		_colorBuffers[i].attachment = colorBufferAttachment;

		if (desc.isBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			colorBuf.buffer = backBuffer_dx12->_colorBuf_dx12;
		}
	}

}


} // namespace

#endif // #if AX_RENDERER_DX12
