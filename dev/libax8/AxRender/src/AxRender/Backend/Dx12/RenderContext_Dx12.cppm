module;

export module AxRender:RenderContext_Dx12;
export import :Dx12DescriptorHeap;
export import :RenderContext_Backend;
export import :RenderPass_Dx12;

#if AX_RENDERER_DX12

import :RenderSystem_Backend;

namespace ax {
class RenderContext_Dx12 : public RenderContext_Backend {
	AX_RTTI_INFO(RenderContext_Dx12, RenderContext_Backend)
public:
	RenderContext_Dx12(const CreateDesc& desc);

	virtual Vec2f	worldToLocalPos(const Vec2f& pt) override { return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt); }
	virtual Vec2f	localToWorldPos(const Vec2f& pt) override { return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt); }

	struct BackBuffer_Dx12 {
		Int	_index = -1;

		SPtr<RenderPassColorBuffer_Dx12>	_colorBuf_dx12;
		SPtr<RenderPass_Dx12>				_renderPass_dx12;

		void createOrUpdate(RenderContext_Dx12* renderContext, Int index, Vec2i frameSize);
		void releaseResources();
	};

	Dx12SwapChain						_swapChain_dx12;
	SPtr<RenderPassDepthBuffer_Dx12>	_depthBuffer_dx12;
	BackBuffer_Dx12* _getBackBuffer(Int i) { return ax_ptr_ptr(_backBuffers_dx12.tryGetElement(i)); }

protected:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) override;
	virtual void                onPresentSurface(RenderRequest* req_) override;

	virtual void onPostCreate(const CreateDesc& desc) override;
	virtual void onSetFrameSize(const Vec2i& s) override;
	virtual void onSetRenderNeeded() override;

private:
	void _createWindow(const CreateDesc& desc);
	void _createSwapChain();

	class UIEventHandler : public NativeUIEventHandler_Win32 {
		RenderContext_Dx12* _owner = nullptr;
	public:
		UIEventHandler(RenderContext_Dx12* owner) : _owner(owner) {}

		virtual void onUIMouseEvent(NativeUIMouseEvent& ev) override { _owner->onUIMouseEvent(ev); }
		virtual void onUIKeyEvent(NativeUIKeyEvent& ev) override { _owner->onUIKeyEvent(ev); }
	};
	static constexpr u32 kRenderTimerId = 100;
	UIEventHandler _uiEventHandler;	

	ThreadId			_currentThreadId;
	Dx12CommandQueue	_graphCmdQueue;
	Dx12CommandQueue	_computeCmdQueue;

	void _createBackBuffers();
	Array<UPtr<BackBuffer_Dx12>, 8>	_backBuffers_dx12;
	
	HWND _hwnd = nullptr;
};

} // namespace


#endif //AX_RENDERER_DX12
