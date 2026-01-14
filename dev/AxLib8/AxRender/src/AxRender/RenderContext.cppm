module;
export module AxRender:RenderContext;
export import :RenderRequest;
export import :RenderGraph;

export namespace ax /*::AxRender*/ {

class RenderContext;

struct SwapChainDesc {
	Int			backBufferCount = 2;
	bool		vsync = true;
	RenderPassColorAttachmentDesc	colorAttachmentDesc;
	RenderPassDepthAttachmentDesc	depthAttachmentDesc;
};

class RenderContext_CreateDesc : public NonCopyable {
public:
	NativeUIWindow*               window = nullptr;
	NativeUIWindow_CreateDesc     windowDesc;
	SwapChainDesc                 swapChainDesc;
};

class RenderContext : public RenderObject {
	AX_RTTI_INFO(RenderContext, RenderObject)
public:
	using CreateDesc	= RenderContext_CreateDesc;
	static UPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderContext(const CreateDesc& desc);
	
	void setRenderNeeded() { onSetRenderNeeded(); }

	void setFrameSize(const Vec2i& s) { onSetFrameSize(s); }

	const SwapChainDesc& swapChainDesc() const { return _swapChainDesc; }

	NativeUIWindow*	window	()		{ return _window; }

	void setRenderGraph(RenderGraph* graph) { _renderGraph = graph; }

	bool viewportIsBottomUp() const { return _viewportIsBottomUp; }

	virtual Vec2f	worldToLocalPos(const Vec2f& pt) = 0;
	virtual Vec2f	localToWorldPos(const Vec2f& pt) = 0;

	void setImGuiIniFilename(StrView name) { onSetImGuiIniFilename(name); }
	void loadImGuiIniFile() { onLoadImGuiIniFile(); }
	void saveImGuiIniFile() { onSaveImGuiIniFile(); }

	void render() { onRender(); }

	void setUIEventHandler(UIEventHandler* uiEventHandler) {
		_uiEventHandler = uiEventHandler;
	}
	
friend class RenderSystem;
friend class RenderRequest;
protected:
	virtual void onPostCreate(const CreateDesc& desc) {}
	virtual void onSetRenderNeeded() {}
	virtual void onSetFrameSize(const Vec2i& s) {}
	virtual void onNativeUIMouseEvent(NativeUIMouseEvent& ev);
	virtual void onNativeUIKeyEvent(NativeUIKeyEvent& ev);
	virtual void onRender() {}

	virtual void onSetImGuiIniFilename(StrView name) {}
	virtual void onLoadImGuiIniFile() {}
	virtual void onSaveImGuiIniFile() {}

	RenderSystem*     _renderSystem       = nullptr;
	NativeUIWindow*   _window             = nullptr;
	UIEventHandler*   _uiEventHandler     = nullptr;
	
	bool              _viewportIsBottomUp = false;
	const Vec2i       _minFrameSize{8, 8};
	
	SwapChainDesc     _swapChainDesc;
	SPtr<RenderGraph> _renderGraph;
};

inline
void RenderContext::onNativeUIMouseEvent(NativeUIMouseEvent& ev) {
	if (_uiEventHandler) {
		UIMouseEvent outEvent;
		UIEventManager::s_instance()->_translateNativeUIMouseEvent(outEvent, ev);
		_uiEventHandler->onUIMouseEvent(outEvent);
	}
}

inline
void RenderContext::onNativeUIKeyEvent(NativeUIKeyEvent& ev) {
	if (_uiEventHandler) {
		UIKeyEvent outEvent;
		UIEventManager::s_instance()->_translateNativeUIKeyEvent(outEvent, ev);
		_uiEventHandler->onUIKeyEvent(outEvent);
	}
}


} // namespace