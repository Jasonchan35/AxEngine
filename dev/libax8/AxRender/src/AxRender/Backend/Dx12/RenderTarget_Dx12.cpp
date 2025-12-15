module AxRender;
import :RenderTarget_Dx12;
import :RenderContext_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderTargetColorBuffer_Dx12::RenderTargetColorBuffer_Dx12(const CreateDesc& desc): Base(desc) {
	if (desc.backBufferRef) {
		auto* renderContext_dx12 = rttiCastCheck<RenderContext_Dx12>(desc.backBufferRef.renderContext);
		if (!renderContext_dx12) throw Error_Undefined();
		UINT backBufIndex = SafeCast(desc.backBufferRef.index);
		createFromSwapChain(renderContext_dx12->_swapChain_dx12.ptr(), backBufIndex);
	}
}

void RenderTargetColorBuffer_Dx12::createFromSwapChain(AX_DX12_IDXGISwapChain* swapChain, UINT backBufIndex) {
	_d3dResource.createFromSwapChain(swapChain, backBufIndex);
	_descHeap.create(1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	_renderTargetView = _descHeap.getHandle(0);
	Renderer_Dx12::s_d3dDevice()->CreateRenderTargetView(_d3dResource.d3dResource(), nullptr, _renderTargetView);
}

} // namespace

#endif // #if AX_RENDERER_DX12