module;
export module AxRender:RenderGraph;
export import :RenderDataType;
export import :RenderRequest;

export namespace ax /*::AxRender*/ {

class RenderGraph;
class RenderGraph_Pass;
class RenderGraph_Input;
class RenderGraph_Inputs;

class RenderGraph_ColorBuffer : public NonCopyable {
	using This = RenderGraph_ColorBuffer;
public:
	using Graph  = RenderGraph;
	using Pass   = RenderGraph_Pass;
	using Desc   = RenderPassColorAttachmentDesc;

	RenderGraph_ColorBuffer(Pass* pass, InNameId name, ColorType colorType);

	Pass*       pass() { return _pass; }
	NameId      name() const { return _name; }
	const Desc& desc() const { return _desc; }

	void setDirty();
	void setDesc(const Desc& attachment)		{ _assignIfDiff(_desc, attachment); }
	void setClearColor(const Color4f& color)	{ _assignIfDiff(_desc.clearColor, color); }
	void setLoadOp(RenderBufferLoadOp loadOp)	{ _assignIfDiff(_desc.loadOp, loadOp); }
	void setColorType(ColorType colorType)		{ _assignIfDiff(_desc.colorType, colorType); }

private:
	template<class V>
	void _assignIfDiff(V& dst, const V& src) {
		if (dst == src) return;
		dst = src;
		setDirty();
	}
	
	Pass*	_pass = nullptr;
	NameId	_name;
	Desc	_desc;
};

class RenderGraph_DepthBuffer : public NonCopyable {
	using This = RenderGraph_DepthBuffer;
public:
	using Graph  = RenderGraph;
	using Pass   = RenderGraph_Pass;
	using Desc	 = RenderPassDepthAttachmentDesc;

	RenderGraph_DepthBuffer(Pass* pass, InNameId name, RenderDepthType depthType);
	
	Pass*		pass() { return _pass; }
	NameId		name() const { return _name; }
	const Desc&	desc() const { return _desc; }
	
	void setDirty();
	void setDesc(const Desc& desc)	{ _assignIfDiff(_desc, desc); }
	void setDepthType(RenderDepthType depthType)			{ _assignIfDiff(_desc.depthType, depthType); }
	void setLoadOp(RenderBufferLoadOp loadOp)				{ _assignIfDiff(_desc.loadOp, loadOp); }
	void setClearDepth(f32 depth)							{ _assignIfDiff(_desc.clearDepth, depth); }
	void setClearStencil(u32 stencil)						{ _assignIfDiff(_desc.clearStencil, stencil); }

private:
	template<class V>
	void _assignIfDiff(V& dst, const V& src) {
		if (dst == src) return;
		dst = src;
		setDirty();
	}
	
	Pass*	_pass = nullptr;
	NameId	_name;
	Desc	_desc;
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
	using Pass        = RenderGraph_Pass;
	using Input       = RenderGraph_Input;
	using Inputs      = RenderGraph_Inputs;
	using ColorBuffer = RenderGraph_ColorBuffer;
	using DepthBuffer = RenderGraph_DepthBuffer;

	using RenderDelegate = Delegate_<void (RenderRequest* req, Span<Input> inputs)>;

	template<class GRAPH>
	RenderGraph_Pass(GRAPH* graph, InNameId name, void (GRAPH::*func)(RenderRequest* req, Span<Input> inputs))
		: _depthBuffer(this, "depth", RenderDepthType::Depth_UNorm24_Stencil_UInt8)
	{
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

	ColorBuffer*		addColorBuffer(NameId name);

	NameId				name() const			{ return _name; }
	const Vec2i&		frameSize() const		{ return _frameSize; }
	bool				isBackBuffer() const	{ return _isBackBuffer; }

	Span<ColorBuffer*> colorBuffers() { return _colorBuffers; }
	DepthBuffer&       depthBuffer() { return _depthBuffer; }

	void setFrameSize(Vec2i frameSize);

	void setAutoFrameSize(AutoFrameSize autoFrameSize);
	AutoFrameSize	autoFrameSize() const { return _autoFrameSize; }

	void setDirty();

friend class RenderGraph;
friend class RenderGraph_ColorBuffer;
protected:

	void _init(RenderGraph* graph, NameId name);
	void _addColorBuffer(ColorBuffer* colorBuf);
	bool _compareInputs(Span<ColorBuffer*> inputs);

	void _createRenderPass();
	void _render(class RenderRequest_Backend* req);

	RenderGraph*	_graph = nullptr;
	NameId			_name;
	Vec2i			_frameSize {0,0};
	AutoFrameSize	_autoFrameSize = AutoFrameSize::Full;

	bool			_isBackBuffer = false;

	Array<Input, 8>        _inputs;
	Array<ColorBuffer*, 8> _colorBuffers;
	DepthBuffer            _depthBuffer;

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
	using ColorAttachmentDesc = RenderPassColorAttachmentDesc;
	using DepthAttachmentDesc = RenderPassDepthAttachmentDesc;

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
