module;
export module AxRender.RenderBuffer;
export import AxRender.Texture;

export namespace ax::AxRender {

struct BackBufferRef {
	class RenderContext* renderContext = nullptr;
	Int				index = 0;

	explicit operator bool() const { return renderContext; }
};

class RenderColorBuffer_CreateDesc : public NonCopyable {
public:
	String			name;
	ColorType		colorType = ColorType::RGBAb;
	Vec2i			size{0, 0};
	BackBufferRef	backBufferRef;

	void setBackBuffer(RenderContext* renderContext_, Int index_, ColorType colorType_, const Vec2i& size_);
};

class RenderColorBuffer : public RenderObject {
	AX_RTTI_INFO(RenderColorBuffer, RenderObject)
public:
	using CreateDesc = RenderColorBuffer_CreateDesc;

	const BackBufferRef&	backBufferRef() const { return _backBufferRef; }

	ColorType		colorType() const	{ return _colorType; }
	const Vec2i&	size() const		{ return _size; }

protected:
	RenderColorBuffer(const CreateDesc& desc);

	ColorType		_colorType = ColorType::RGBAb;
	Vec2i			_size{0, 0};
	BackBufferRef	_backBufferRef;
};

class RenderDepthBuffer_CreateDesc : NonCopyable {
public:
	String		name;
	DepthType	depthType = DepthType::None;
	Vec2i		frameSize {0,0};
};

class RenderDepthBuffer : public Texture {
	AX_RTTI_INFO(RenderDepthBuffer, Texture)
public:
	using CreateDesc = RenderDepthBuffer_CreateDesc;

	DepthType	depthType() const { return _depthType; }
	Vec2i		frameSize() const { return _frameSize; }

protected:
	RenderDepthBuffer(const CreateDesc& desc);
	DepthType	_depthType = DepthType::None;
	Vec2i		_frameSize {0,0};
};

} // namespace