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
	RenderContext_Dx12(const CreateDesc& desc);

	virtual Vec2f	worldToLocalPos(const Vec2f& pt) override {
		return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt);
	}
	virtual Vec2f	localToWorldPos(const Vec2f& pt) override {
		return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt);
	}
	
protected:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	virtual BackBuffer* onGetBackBuffer(Int i) override { return _backBuffers.tryGetElement(i); }
	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) override { AX_ASSERT_TODO; return nullptr; }
	virtual void onPresentSurface(RenderRequest* req) override { AX_ASSERT_TODO; }
	
	virtual void onPostCreate(const CreateDesc& desc) override;
	virtual void onSetFrameSize(const Vec2i& s) override;
	virtual void onSetRenderNeeded() override;
	
private:
	void _createWindow(const CreateDesc& desc);
	void _createRenderTargetView();
	void _releaseRenderTargetView();
	void _setCurrentBackBufferIndex(UINT i);

	class UIEventHandler : public NativeUIEventHandler_Win32 {
		RenderContext_Dx12* _owner = nullptr;
	public:
		UIEventHandler(RenderContext_Dx12* owner) : _owner(owner) {}

		virtual void onUIMouseEvent(NativeUIMouseEvent& ev) override { _owner->onUIMouseEvent(ev); }
		virtual void onUIKeyEvent(NativeUIKeyEvent& ev) override { _owner->onUIKeyEvent(ev); }
	};
	static constexpr u32 kRenderTimerId = 100;
	UIEventHandler _uiEventHandler;	

	static constexpr UINT kBackBufferCount = 2; // front and back buffer

	ThreadId							_currentThreadId;
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandQueue>			_computeCmdQueue;

	ComPtr<IDXGISwapChain3>				_swapChain;

	Dx12DescripterHeap_RenderTarget			_renderTargetDescHeap;
	Dx12DescripterHeap_DepthStencilBuffer	_depthStencilDescHeap;

	struct BackBuffer_Dx12 : public BackBuffer {
		UINT index = 0xffff;
		Dx12DescriptorHandle				renderTarget;
		Dx12DescriptorHandle				depthStencilBuffer;

		Dx12Resource_RenderTarget			_renderTargetResource;
		Dx12Resource_DepthStencilBuffer		_depthStencilBufferResource;
	};

	FixedArray<BackBuffer_Dx12, kBackBufferCount>	_backBuffers;
	BackBuffer_Dx12*	_currentBackBuffer = nullptr;
	UINT				_currentBackBufferIndex = 0;

	HWND _hwnd = nullptr;
};

} // namespace


#endif //AX_RENDERER_DX12
