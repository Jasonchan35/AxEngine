module;

export module AxRender:RenderContext_VK;

#if AX_RENDERER_VK
export import :RenderContext_Backend;
export import :CommandBuffer_VK;

export namespace ax::AxRender {

class RenderContext_VK_Base : public RenderContext_Backend {
	AX_RTTI_INFO(RenderContext_VK_Base, RenderContext_Backend)
public:
	RenderContext_VK_Base(const CreateDesc& desc);
	virtual ~RenderContext_VK_Base() override;

	AX_VkSurfaceKHR&	surface() { return _surface; }

	VkImage _getBackBufferImage(Int i) { return _backBufImages[i]; }

private:	
	void	_createSwapChain();

protected:
	virtual void onPostCreate(const CreateDesc& desc) override;

	struct BackBuffer_VK : public BackBuffer {
		void createOrUpdate(RenderContext_VK_Base* renderContext, AX_VkDevice& dev, Int index, Vec2i frameSize);

		CommandBuffer_VK	_presentCmdBuf_vk;
		AX_VkSemaphore		_presentSemaphore_vk;
	};

	AX_INLINE BackBuffer_VK* _getBackBuffer(Int i) {
		return _backBuffers.inBound(i) ? _backBuffers[i].ptr() : nullptr;
	}

	virtual BackBuffer* onGetBackBuffer(Int i) override { return _getBackBuffer(i); }

	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) override;
	virtual void onPresentSurface(RenderRequest* req) override;


	AX_VkSurfaceKHR		_surface;
	AX_VkDeviceQueue	_graphQueue;
	AX_VkDeviceQueue	_presentQueue;

	AX_VkSwapchainKHR	_swapChain;

	Array<VkImage, 8>	_backBufImages;
	Array< UPtr<BackBuffer_VK> >	_backBuffers;
};

#if AX_NATIVE_UI_WIN32

class RenderContext_VK_Win32 : public RenderContext_VK_Base {
	AX_RTTI_INFO(RenderContext_VK_Win32, RenderContext_VK_Base)
public:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	RenderContext_VK_Win32(const CreateDesc& desc);
	virtual ~RenderContext_VK_Win32() override;

	virtual Vec2f	worldToLocalPos(const Vec2f& pt) override;
	virtual Vec2f	localToWorldPos(const Vec2f& pt) override;

private:
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	virtual void onPostCreate(const CreateDesc& desc) override;
	virtual void onSetFrameSize(const Vec2i& rc) override;
	virtual void onSetRenderNeeded() override;

	HWND _hwnd = nullptr;

	class UIEventHandler : public NativeUIEventHandler_Win32 {
		RenderContext_VK_Win32* _owner = nullptr;
	public:
		UIEventHandler(RenderContext_VK_Win32* owner) : _owner(owner) {}

		virtual void onUIMouseEvent(NativeUIMouseEvent& ev) override { _owner->onUIMouseEvent(ev); }
		virtual void onUIKeyEvent(NativeUIKeyEvent& ev) override { _owner->onUIKeyEvent(ev); }
	};

	UIEventHandler _uiEventHandler;

	static constexpr u32 kRenderTimerId = 100;
};

using RenderContext_VK_Impl = RenderContext_VK_Win32;

// #if AX_NATIVE_UI_WIN32
#elif AX_NATIVE_UI_X11  
	
	using RenderContext_VK_Impl = RenderContext_VK_X11;
#else
	#error
#endif



class RenderContext_VK : public RenderContext_VK_Impl {
	AX_RTTI_INFO(RenderContext_VK, RenderContext_VK_Impl)
public:
	RenderContext_VK(const CreateDesc& desc) : Base(desc) {}
};


} // namespace 

#endif // AX_RENDERER_VK