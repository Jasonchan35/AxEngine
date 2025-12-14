module;
export module AxRender:RenderGraph;
export import :RenderDataType;
export import :RenderRequest;

export namespace ax::AxRender {

class RenderGraph;
class RenderGraph_Pass;
class RenderGraph_Input;
class RenderGraph_Inputs;

class RenderGraph_ColorBuffer : public NonCopyable {
	using This = RenderGraph_ColorBuffer;
public:
	using Graph  = RenderGraph;
	using Pass   = RenderGraph_Pass;

	RenderGraph_ColorBuffer(Pass* pass, StrView name, const RenderColorBufferDesc& desc);

	Pass*			pass() { return _pass; }
	StrView			name() const { return _name; }

	const RenderColorBufferDesc&	desc() const { return _desc; }

	void setDesc(const RenderColorBufferDesc& desc);
	void setDirty();

	void setClearColor(const Color4f& color);
	void setLoadOp(RenderBufferLoadOp loadOp);
	void setColorType(ColorType colorType);

private:
	Pass*				_pass = nullptr;
	String				_name;
	RenderColorBufferDesc		_desc;
};

class RenderGraph_Input : public NonCopyable {
public:
	using Graph  = RenderGraph;
	using Pass   = RenderGraph_Pass;
	using Input  = RenderGraph_Input;
	using ColorBuffer = RenderGraph_ColorBuffer;

	RenderGraph_Input(Pass* pass, ColorBuffer* source)
	: _pass(pass), _source(source) {}

	ColorBuffer*	source() { return _source; }
	Pass*			sourcePass() { return _source ? _source->pass() : nullptr; }
	Pass*			pass() { return _pass; }

private:
	Pass*			_pass = nullptr;
	ColorBuffer*	_source = nullptr;
};

class RenderGraph_Pass : public NonCopyable {
public:
	using Pass   = RenderGraph_Pass;
	using Input  = RenderGraph_Input;
	using Inputs = RenderGraph_Inputs;
	using ColorBuffer  = RenderGraph_ColorBuffer;

	using RenderDelegate = Delegate_<void (RenderRequest* req, Span<Input> inputs)>;

	template<class GRAPH>
	RenderGraph_Pass(GRAPH* graph, StrView name, void (GRAPH::*func)(RenderRequest* req, Span<Input> inputs)) {
		static_assert(std::is_base_of_v<RenderGraph, GRAPH>);
		_renderDelegate.bindUnowned(graph, func);
		_init(graph, name);
	}

	template<class... ARGS>
	void setInputs(ARGS&... args) {
		ColorBuffer* arr[] = {&args...};
		setInputs(Span(arr));
	}

	void setInputs(Span<ColorBuffer*> inputs);

	ColorBuffer*		addColorBuffer(StrView name);

	StrView				name() const			{ return _name; }
	const Vec2i&		frameSize() const		{ return _frameSize; }
	bool				isBackBuffer() const	{ return _isBackBuffer; }

	Span<ColorBuffer*>	colorBuffers() { return _colorBuffers; }

	void setDepthBufferDesc(const RenderDepthBufferDesc& v);

	void setFrameSize(Vec2i frameSize);

	void setAutoFrameSize(AutoFrameSize autoFrameSize);
	AutoFrameSize	autoFrameSize() const { return _autoFrameSize; }

	void setDirty();

friend class RenderGraph;
friend class RenderGraph_ColorBuffer;
protected:

	void _init(RenderGraph* graph, StrView name);
	void _addColorBuffer(ColorBuffer* colorBuf);
	bool _compareInputs(Span<ColorBuffer*> inputs);

	void _createRenderPass();
	void _render(class RenderRequest_Backend* req);

	RenderGraph*	_graph = nullptr;
	String			_name;
	Vec2i			_frameSize {0,0};
	AutoFrameSize	_autoFrameSize = AutoFrameSize::Full;

	bool			_isBackBuffer = false;

	Array<Input,  8>		_inputs;
	Array<ColorBuffer*, 8>	_colorBuffers;
	RenderDepthBufferDesc			_depthBufferDesc;

	RenderDelegate			_renderDelegate;
	SPtr<RenderPass>		_renderPass;


	struct BuildData {
		Int			_leafLevel = 0;
		bool		_added = false;
		Int			_dependedCount = 0;
	} _buildData;
};

class RenderGraph_BackBufferPass : public RenderGraph_Pass {
	using Base = RenderGraph_Pass;
public:
	ColorBuffer		color0 {this, "color0", ColorType::RGBAb};

	template<class GRAPH>
	RenderGraph_BackBufferPass(GRAPH* graph, void (GRAPH::*func)(RenderRequest* req, Span<Input> inputs)) 
	: Base(graph, "BackBufferPass", func)
	{
		_isBackBuffer = true;
	}

	void setRenderPass(RenderPass* pass);
};

class RenderGraph : public RenderObject {
	AX_RTTI_INFO(RenderGraph, RenderObject)
public:
	using Pass	= RenderGraph_Pass;
	using Input	= RenderGraph_Input;
	using BackBufferPass = RenderGraph_BackBufferPass;

	// forward those names for RenderGraph subclass
	using RenderColorBufferDesc	= RenderColorBufferDesc;
	using RenderDepthBufferDesc	= RenderDepthBufferDesc;
	using RenderBufferLoadOp		= RenderBufferLoadOp;

	const Vec2i& frameSize() const { return _frameSize; }

	virtual void onUpdate(RenderRequest* req) = 0;

	void	setDirty()		{ _dirty = true; }
	bool	isDirty() const	{ return _dirty; }

	void	setOutputPass(Pass& pass);
	void	setFrameSize(Vec2i frameSize);

	BackBufferPass& backBufferPass() { return _backBufferPass; }

	virtual void onBackBufferPass(RenderRequest* req, Span<Input> inputs) = 0;

	RenderGraph();

friend class RenderContext_Backend;
friend class RenderGraph_Pass;
protected:

	void	_addPass(Pass* pass) { _passes.emplaceBack(pass); }
	void	_update(RenderRequest* req);
	void	_render(class RenderRequest_Backend* req);
	bool	_rebuild();

private:
	Vec2i	_frameSize {0,0};
	bool	_dirty = true;

	Array<Pass*>	_passes;
	Array<Pass*>	_resultPasses;

	struct BuildData {
		Array<Pass*>	_pendingPasses;
		Array<Pass*>	_dependedPasses;
	} _buildData;

	BackBufferPass	_backBufferPass;
};

AX_INLINE void RenderGraph_Pass::setDirty() {
	if (_graph) _graph->setDirty();
}

AX_INLINE void RenderGraph_ColorBuffer::setDirty() {
	if (_pass) _pass->setDirty();
}

} // namespace
