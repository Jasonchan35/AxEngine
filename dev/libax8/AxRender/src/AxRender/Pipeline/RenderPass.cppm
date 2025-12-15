module;
export module AxRender:RenderPass;
export import :RenderObject;
export import :GpuBuffer;
export import :RenderTarget;

export namespace ax /*::AxRender*/ {

class RenderPass_CreateDesc : public NonCopyable {
public:
	void setBackBuffer(class RenderContext* ctx, Int index) {
		isBackBuffer = true;
		renderContext = ctx;
		backBufferIndex = index;
	}

	String	name;

	Vec2i	frameSize {0,0};
	bool			isBackBuffer = false;
	RenderContext*	renderContext = nullptr;
	Int				backBufferIndex = 0;

	Array<RenderTargetColorBufferDesc, 16>	colorBuffers;
	RenderTargetDepthBufferDesc				depthBuffer;
};

class RenderPass : public RenderObject {
	AX_RTTI_INFO(RenderPass, RenderObject)
public:
	using CreateDesc = RenderPass_CreateDesc;

	const Vec2i&	frameSize() const	{ return _frameSize; }

	Int				colorBufferCount() const	{ return _colorBuffers.size(); }

	const Color4f&	clearColor(Int i) const		{ return _colorBuffers.inBound(i) ? _colorBuffers[i].desc.clearColor : Color4f::kBlack(); }

	f32				clearDepth() const			{ return _depthBuffer.desc.clearDepth; }
	u32				clearStencil() const		{ return _depthBuffer.desc.clearStencil; }

	void			setClearColor(Int i, const Color4f& color) { if (_colorBuffers.inBound(i)) _colorBuffers[i].desc.clearColor = color; }
	void			setClearDepth(f32 v)		{ _depthBuffer.desc.clearDepth   = v; }
	void			setClearStencil(u32 v)		{ _depthBuffer.desc.clearStencil = v; }

			RenderTargetColorBuffer*	colorBuffer(Int i) { return _colorBuffers.inBound(i) ? _colorBuffers[i].colorBuf.ptr() : nullptr; }
	const	RenderTargetColorBuffer*	colorBuffer(Int i) const { return ax_const_cast(this)->colorBuffer(i); }

	const RenderTargetColorBufferDesc*	colorBufferDesc(Int i) { return _colorBuffers.inBound(i) ? &_colorBuffers[i].desc : nullptr; }

	RenderTargetDepthBuffer*	depthBuffer()		{ return _depthBuffer.depthBuf.ptr(); }

	bool isCompatible(const CreateDesc& desc) const;

protected:
	RenderPass(const CreateDesc& desc);

	Vec2i	_frameSize {0,0};

	struct ColorBuffer {
		SPtr<RenderTargetColorBuffer>	colorBuf;
		RenderTargetColorBufferDesc			desc;
	};

	struct DepthBuffer {
		SPtr<RenderTargetDepthBuffer>	depthBuf;
		RenderTargetDepthBufferDesc			desc;
	};

	Array< ColorBuffer >	_colorBuffers;
	DepthBuffer				_depthBuffer;
};

} // namespace
