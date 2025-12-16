module;
export module AxRender:RenderContext;
export import :RenderRequest;
export import :RenderGraph;

export namespace ax /*::AxRender*/ {

class RenderContext;

struct SwapChainDesc {
	Int			backBufferCount = 2;
	bool		vsync = true;
	RenderPassColorBufferAttachment	colorBufferAttachment;
	RenderPassDepthBufferAttachment	depthBufferAttachment;
};

class RenderContext_CreateDesc : public NonCopyable {
public:
	NativeUIWindow*				window = nullptr;
	NativeUIWindow_CreateDesc	windowDesc;
	SwapChainDesc				swapChainDesc;
};

class RenderContext : public RttiObject {
	AX_RTTI_INFO(RenderContext, RttiObject)
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

friend class Renderer;
friend class RenderRequest;
protected:
	virtual void onPostCreate(const CreateDesc& desc) {}
	virtual void onSetRenderNeeded() {}
	virtual void onSetFrameSize(const Vec2i& s) {}
	virtual void onUIMouseEvent(NativeUIMouseEvent& ev) {}
	virtual void onUIKeyEvent(NativeUIKeyEvent& ev) {}
	virtual void onRender() {}

	virtual void onSetImGuiIniFilename(StrView name) {}
	virtual void onLoadImGuiIniFile() {}
	virtual void onSaveImGuiIniFile() {}

	Renderer*       _renderer           = nullptr;
	NativeUIWindow* _window             = nullptr;
	bool            _viewportIsBottomUp = false;
	const	Vec2i	_minFrameSize		= {8,8}; 

	SwapChainDesc		_swapChainDesc;
	SPtr<RenderGraph>	_renderGraph;
};

} // namespace