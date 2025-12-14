module;
export module AxRender:RenderTarget;
export import :Renderer;
export import :Texture;

export namespace ax /*::AxRender*/ {


struct BackBufferRef {
	class RenderContext* renderContext = nullptr;
	Int				index = 0;

	explicit operator bool() const { return renderContext; }
};

class RenderTargetColorBuffer_CreateDesc : public NonCopyable {
public:
	String			name;
	ColorType		colorType = ColorType::RGBAb;
	Vec2i			size{0, 0};
	BackBufferRef	backBufferRef;

	void setBackBuffer(RenderContext* renderContext_, Int index_, ColorType colorType_, const Vec2i& size_);
};

class RenderTargetColorBuffer : public RenderObject {
	AX_RTTI_INFO(RenderTargetColorBuffer, RenderObject)
public:
	using CreateDesc = RenderTargetColorBuffer_CreateDesc;

	const BackBufferRef&	backBufferRef() const { return _backBufferRef; }

	ColorType		colorType() const	{ return _colorType; }
	const Vec2i&	size() const		{ return _size; }

protected:
	RenderTargetColorBuffer(const CreateDesc& desc);

	ColorType		_colorType = ColorType::RGBAb;
	Vec2i			_size{0, 0};
	BackBufferRef	_backBufferRef;
};

class RenderTargetDepthBuffer_CreateDesc : NonCopyable {
public:
	String		name;
	RenderDepthType	depthType = RenderDepthType::None;
	Vec2i		frameSize {0,0};
};

class RenderTargetDepthBuffer : public Texture { // TODO Texture -> RenderObject
	AX_RTTI_INFO(RenderTargetDepthBuffer, Texture)
public:
	using CreateDesc = RenderTargetDepthBuffer_CreateDesc;

	RenderDepthType	depthType() const { return _depthType; }
	Vec2i		frameSize() const { return _frameSize; }

protected:
	RenderTargetDepthBuffer(const CreateDesc& desc);
	RenderDepthType	_depthType = RenderDepthType::None;
	Vec2i		_frameSize {0,0};
};


} // namespace 