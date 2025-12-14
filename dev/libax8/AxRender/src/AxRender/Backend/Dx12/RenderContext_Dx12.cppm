module;

export module AxRender:RenderContext_Dx12;
import :Dx12Util;
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

	void cmdSwapBuffers(CommandDispatcher_Dx12& dispatcher);

protected:
	virtual void onDispatchCommands(RenderRequest& req) override;
	virtual void onPreDestroy();
	virtual void onSetNeedToRender() override;
	virtual void onSetNativeViewRect(const Rect2f& rect) override;

	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}


private:
	void _createWindow(CreateDesc& desc);

	void _createRenderTargetView();
	void _releaseRenderTargetView();

	void _setFrameIndex(UINT i);

	static const UINT kFrameBufferCount = 2; // front and back buffer	
	static const wchar_t* kClassName;
	ThreadId							_currentThreadId;
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandQueue>			_computeCmdQueue;

	ComPtr<IDXGISwapChain3>				_swapChain;

	axDX12DescripterHeap_RenderTarget		_renderTargetDescHeap;
	axDX12DescripterHeap_DepthStencilBuffer	_depthStencilDescHeap;

	struct FrameData {
		UINT index = 0xffff;
		axDX12DescriptorHandle				renderTarget;
		axDX12DescriptorHandle				depthStencilBuffer;

		axDX12Resource_RenderTarget			_renderTargetResource;
		axDX12Resource_DepthStencilBuffer	_depthStencilBufferResource;
	};

	FixedArray<FrameData, kFrameBufferCount>	_frameArray;
	FrameData*	_frame = nullptr;
	UINT		_frameIndex = 0;

	HWND _hwnd = nullptr;
};

} // namespace


#endif //AX_RENDERER_DX12
