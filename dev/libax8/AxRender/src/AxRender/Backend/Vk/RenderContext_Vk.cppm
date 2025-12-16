module;

export module AxRender:RenderContext_Vk;

#if AX_RENDERER_VK
export import :RenderContext_Backend;
export import :CommandBuffer_Vk;
export import :RenderPass_Vk;

export namespace ax /*::AxRender*/ {

class RenderContext_Vk_Base;

class RenderContext_Vk_Base : public RenderContext_Backend {
	AX_RTTI_INFO(RenderContext_Vk_Base, RenderContext_Backend)
public:
	RenderContext_Vk_Base(const CreateDesc& desc);
	virtual ~RenderContext_Vk_Base() override;

	AX_VkSurfaceKHR&	surface() { return _surface_vk; }
	
	SPtr<RenderPassDepthBuffer_Vk>	_depthBuf_vk;

	struct BackBuffer_Vk : public NonCopyable {
		void createOrUpdate(RenderContext_Vk_Base* renderContext, AX_VkDevice& dev, Int index, VkImage& vkImage, Vec2i frameSize);
		
		Int					_index = -1;

		SPtr<RenderPassColorBuffer_Vk>	_colorBuf_vk;
		SPtr<RenderPass_Vk>					_renderPass_vk;
		
		CommandBuffer_Vk	_presentCmdBuf_vk;
		AX_VkSemaphore		_presentSemaphore_vk;
		VkImage				_vkImage;
	};
	
	BackBuffer_Vk* _getBackBuffer(Int i) { return ax_ptr_ptr(_backBuffers_vk.tryGetElement(i)); }
protected:	
	void _createSwapChain();
	void _createBackBuffers(AX_VkDevice& dev, Vec2i frameSize);
	virtual RenderPass_Backend* onAcquireBackBufferRenderPass(RenderRequest* req) override;
	virtual void onPresentSurface(RenderRequest* req) override;
	virtual void onPostCreate(const CreateDesc& desc) override;
	
	AX_VkSurfaceKHR		_surface_vk;
	AX_VkDeviceQueue	_graphQueue_vk;
	AX_VkDeviceQueue	_presentQueue_vk;
	AX_VkSwapchainKHR	_swapChain_vk;
	Array< UPtr<BackBuffer_Vk>, 8 >	_backBuffers_vk;
};

#if AX_NATIVE_UI_WIN32

class RenderContext_Vk_Win32 : public RenderContext_Vk_Base {
	AX_RTTI_INFO(RenderContext_Vk_Win32, RenderContext_Vk_Base)
public:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	RenderContext_Vk_Win32(const CreateDesc& desc);
	virtual ~RenderContext_Vk_Win32() override;

	virtual Vec2f	worldToLocalPos(const Vec2f& pt) override { return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt); }
	virtual Vec2f	localToWorldPos(const Vec2f& pt) override { return NativeUI_Win32::s_worldToLocalPos(_hwnd, pt); }

private:
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	virtual void onPostCreate(const CreateDesc& desc) override;
	virtual void onSetFrameSize(const Vec2i& rc) override;
	virtual void onSetRenderNeeded() override;

	HWND _hwnd = nullptr;

	class UIEventHandler : public NativeUIEventHandler_Win32 {
		RenderContext_Vk_Win32* _owner = nullptr;
	public:
		UIEventHandler(RenderContext_Vk_Win32* owner) : _owner(owner) {}

		virtual void onUIMouseEvent(NativeUIMouseEvent& ev) override { _owner->onUIMouseEvent(ev); }
		virtual void onUIKeyEvent(NativeUIKeyEvent& ev) override { _owner->onUIKeyEvent(ev); }
	};

	UIEventHandler _uiEventHandler;

	static constexpr u32 kRenderTimerId = 100;
};

using RenderContext_Vk_Impl = RenderContext_Vk_Win32;

// #if AX_NATIVE_UI_WIN32
#elif AX_NATIVE_UI_X11  
	
	using RenderContext_VK_Impl = RenderContext_VK_X11;
#else
	#error
#endif



class RenderContext_Vk : public RenderContext_Vk_Impl {
	AX_RTTI_INFO(RenderContext_Vk, RenderContext_Vk_Impl)
public:
	RenderContext_Vk(const CreateDesc& desc) : Base(desc) {}
};


} // namespace 

#endif // AX_RENDERER_VK