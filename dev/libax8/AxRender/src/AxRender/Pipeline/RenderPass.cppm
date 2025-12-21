module;
export module AxRender:RenderPass;
export import :RenderObject;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

struct RenderBackBufferRef {
	class RenderContext* renderContext = nullptr;
	Int	index = 0;

	explicit operator bool() const { return renderContext; }

	void set(RenderContext* renderContext_, Int index_) {
		renderContext = renderContext_;
		index = index_;
	}
};

class RenderPassColorBuffer_CreateDesc : public NonCopyable {
public:
	NameId              name;
	Vec2i               frameSize{0, 0};
	ColorType           colorType = ColorType::RGBAb;
	RenderBackBufferRef fromBackBuffer;
};

class RenderPassColorBuffer : public RenderObject {
	AX_RTTI_INFO(RenderPassColorBuffer, RenderObject)
public:
	using CreateDesc = RenderPassColorBuffer_CreateDesc;

	const RenderBackBufferRef&	backBufferRef() const { return _backBufferRef; }

	ColorType		colorType() const	{ return _colorType; }
	const Vec2i&	frameSize() const	{ return _frameSize; }

protected:
	RenderPassColorBuffer(const CreateDesc& desc);
	Vec2i               _frameSize;
	ColorType           _colorType;
	RenderBackBufferRef _backBufferRef;
};

class RenderPassDepthBuffer_CreateDesc : NonCopyable {
public:
	String          name;
	Vec2i           frameSize{0, 0};
	RenderDepthType depthType = RenderDepthType::None;
};

class RenderPassDepthBuffer : public Texture { // TODO Texture -> RenderObject
	AX_RTTI_INFO(RenderPassDepthBuffer, Texture)
public:
	using CreateDesc = RenderPassDepthBuffer_CreateDesc;

	RenderDepthType	depthType() const { return _depthType; }
	Vec2i			frameSize() const { return _frameSize; }

protected:
	RenderPassDepthBuffer(const CreateDesc& desc);
	Vec2i			_frameSize;
	RenderDepthType _depthType;
};


class RenderPass_CreateDesc : public NonCopyable {
public:
	NameId              name;
	Vec2i               frameSize{0, 0};
	RenderBackBufferRef fromBackBuffer;

	Array<RenderPassColorAttachmentDesc, 16>	colorAttachmentDescs;
	RenderPassDepthAttachmentDesc				depthAttachmentDesc;
};

class RenderPass : public RenderObject {
	AX_RTTI_INFO(RenderPass, RenderObject)
public:
	using CreateDesc = RenderPass_CreateDesc;

	const Vec2i&	frameSize() const	{ return _frameSize; }

	bool isCompatible(const CreateDesc& desc) const;

	struct ColorAttachment {
		SPtr<RenderPassColorBuffer>		buffer;
		RenderPassColorAttachmentDesc	desc;
	};

	struct DepthAttachment {
		SPtr<RenderPassDepthBuffer>		buffer;
		RenderPassDepthAttachmentDesc	desc;
		
		AX_INLINE bool isEnabled() const { return desc.isEnabled(); }
		AX_INLINE explicit operator bool() const { return isEnabled(); }
	};
	
	MutSpan<ColorAttachment>	colorAttachments()	{ return _colorAttachments; }
	ColorAttachment*			colorAttachment(Int i) { return _colorAttachments.tryGetElement(i); }

	RenderPassColorBuffer*		colorBuffer(Int i) {
		auto* att = _colorAttachments.tryGetElement(i);
		return att ? att->buffer.ptr() : nullptr;
	}

	DepthAttachment&			depthAttachment()	{ return _depthAttachment; }
	RenderPassDepthBuffer*		depthBuffer()		{ return _depthAttachment.buffer; }
	
protected:
	RenderPass(const CreateDesc& desc);

	Vec2i	_frameSize {0,0};
	
	Array< ColorAttachment >	_colorAttachments;
	DepthAttachment				_depthAttachment;
};

} // namespace
