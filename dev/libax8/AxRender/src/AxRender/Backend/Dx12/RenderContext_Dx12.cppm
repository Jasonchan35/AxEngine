module;

export module AxRender:RenderContext_Dx12;
import :Dx12DescripterHeap;
import :RenderContext_Backend;

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {
class RenderContext_Dx12 : public RenderContext_Backend {
	AX_RTTI_INFO(RenderContext_Dx12, RenderContext_Backend)
public:
	

	~RenderContext_Dx12();

	RenderContext_Dx12(Renderer_Dx12* renderer, CreateDesc& desc);

	Renderer_Dx12* renderer();

protected:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

private:
	void _createWindow(CreateDesc& desc);

	void _createRenderTargetView();
	void _releaseRenderTargetView();

	void _setFrameIndex(UINT i);

	static constexpr UINT kFrameBufferCount = 2; // front and back buffer	

	ThreadId							_currentThreadId;
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandQueue>			_computeCmdQueue;

	ComPtr<IDXGISwapChain3>				_swapChain;

	Dx12DescripterHeap_RenderTarget			_renderTargetDescHeap;
	Dx12DescripterHeap_DepthStencilBuffer	_depthStencilDescHeap;

	struct FrameData {
		UINT index = 0xffff;
		Dx12DescriptorHandle				renderTarget;
		Dx12DescriptorHandle				depthStencilBuffer;

		Dx12Resource_RenderTarget			_renderTargetResource;
		Dx12Resource_DepthStencilBuffer		_depthStencilBufferResource;
	};

	FixedArray<FrameData, kFrameBufferCount>	_frameArray;
	FrameData*	_frame = nullptr;
	UINT		_frameIndex = 0;

	HWND _hwnd = nullptr;
};

} // namespace


#endif //AX_RENDERER_DX12
