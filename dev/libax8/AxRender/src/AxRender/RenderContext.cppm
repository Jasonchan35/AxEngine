module;
export module AxRender:RenderContext;
export import :RenderRequest;
export import :RenderGraph;

export namespace ax /*::AxRender*/ {

class RenderContext;

class RenderContext_CreateDesc : public NonCopyable {
public:
	NativeUIWindow*				window = nullptr;
	NativeUIWindow_CreateDesc	winDesc;

	Int			backBufferCount = 3;
	bool		vsync = true;

	RenderColorBufferDesc	colorBuffer;
	RenderDepthBufferDesc	depthBuffer;
};

class RenderContext : public RttiObject {
	AX_RTTI_INFO(RenderContext, RttiObject)
public:
	using CreateDesc	= RenderContext_CreateDesc;

	static UPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderContext(const CreateDesc& desc);
	
	void setRenderNeeded() { onSetRenderNeeded(); }

	void setFrameSize(const Vec2i& s) { onSetFrameSize(s); }
	const Vec2i&  frameSize() const		{ return _frameSize; }

	ColorType	colorType() const	{ return _colorBufferDesc.colorType; }
	RenderDepthType	depthType() const	{ return _depthBufferDesc.depthType; }

	bool	vsync() const { return _vsync; }

	NativeUIWindow*	window	()		{ return _window; }

	RenderColorBuffer*	backColorBuffer(Int i);
	RenderDepthBuffer*	backDepthBuffer() { return _depthBuf.ptr(); }

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
	struct BackBuffer : public NonCopyable {
		SPtr<RenderColorBuffer>	_colorBuf;
		SPtr<RenderPass>		_renderPass;
	};
	virtual BackBuffer*	onGetBackBuffer(Int i) = 0;

	virtual void onPostCreate(const CreateDesc& desc);
	virtual void onSetRenderNeeded() {}
	virtual void onSetFrameSize(const Vec2i& s) { _frameSize = s; }
	virtual void onUIMouseEvent(NativeUIMouseEvent& ev) {}
	virtual void onUIKeyEvent(NativeUIKeyEvent& ev) {}
	virtual void onRender() {}

	virtual void onSetImGuiIniFilename(StrView name) {}
	virtual void onLoadImGuiIniFile() {}
	virtual void onSaveImGuiIniFile() {}

	Renderer*			_renderer = nullptr;
	NativeUIWindow*		_window = nullptr;

	Int			_requestBackBufferCount = 3;
	bool		_vsync = true;
	bool		_viewportIsBottomUp = false;

	RenderColorBufferDesc	_colorBufferDesc;
	RenderDepthBufferDesc	_depthBufferDesc;

	Vec2i			_frameSize {0,0};

	SPtr<RenderDepthBuffer>		_depthBuf;
	SPtr<RenderGraph>			_renderGraph;
};

} // namespace