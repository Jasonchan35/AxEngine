module;
export module AxRender:RenderPass;
export import :RenderObject;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

struct BackBufferRef {
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
	String	name;
	Vec2i	frameSize {0,0};
	RenderPassColorBufferAttachment attachment;
	BackBufferRef	backBufferRef;
};

class RenderPassColorBuffer : public RenderObject {
	AX_RTTI_INFO(RenderPassColorBuffer, RenderObject)
public:
	using CreateDesc = RenderPassColorBuffer_CreateDesc;

	const BackBufferRef&	backBufferRef() const { return _backBufferRef; }

	ColorType		colorType() const	{ return _attachment.colorType; }
	const Vec2i&	frameSize() const	{ return _frameSize; }
	const RenderPassColorBufferAttachment& attachment() const { return _attachment; }

protected:
	RenderPassColorBuffer(const CreateDesc& desc);
	Vec2i			_frameSize;
	RenderPassColorBufferAttachment	_attachment;
	BackBufferRef	_backBufferRef;
};

class RenderPassDepthBuffer_CreateDesc : NonCopyable {
public:
	String	name;
	Vec2i	frameSize {0,0};
	RenderPassDepthBufferAttachment attachment;
};

class RenderPassDepthBuffer : public Texture { // TODO Texture -> RenderObject
	AX_RTTI_INFO(RenderPassDepthBuffer, Texture)
public:
	using CreateDesc = RenderPassDepthBuffer_CreateDesc;

	RenderDepthType	depthType() const { return _attachment.depthType; }
	Vec2i			frameSize() const { return _frameSize; }
	const RenderPassDepthBufferAttachment& attachment() const { return _attachment; }

protected:
	RenderPassDepthBuffer(const CreateDesc& desc);
	Vec2i			_frameSize;
	RenderPassDepthBufferAttachment _attachment;
};


class RenderPass_CreateDesc : public NonCopyable {
public:
	void setBackBuffer(class RenderContext* ctx, Int index) {
		isBackBuffer = true;
		renderContext = ctx;
		backBufferIndex = index;
	}

	String			name;
	Vec2i			frameSize {0,0};
	bool			isBackBuffer = false;
	RenderContext*	renderContext = nullptr;
	Int				backBufferIndex = 0;

	Array<RenderPassColorBufferAttachment, 16>	colorBufferAttachments;
	RenderPassDepthBufferAttachment				depthBufferAttachment;
};

class RenderPass : public RenderObject {
	AX_RTTI_INFO(RenderPass, RenderObject)
public:
	using CreateDesc = RenderPass_CreateDesc;

	const Vec2i&	frameSize() const	{ return _frameSize; }

	bool isCompatible(const CreateDesc& desc) const;

	struct ColorBuffer {
		SPtr<RenderPassColorBuffer>		buffer;
		RenderPassColorBufferAttachment	attachment;
	};

	struct DepthBuffer {
		SPtr<RenderPassDepthBuffer>		buffer;
		RenderPassDepthBufferAttachment attachment;
	};
	
	MutSpan<ColorBuffer>	colorBuffers()	{ return _colorBuffers; }
	RenderPassDepthBuffer*	depthBuffer()	{ return _depthBuffer.buffer; }
	
protected:
	RenderPass(const CreateDesc& desc);

	Vec2i	_frameSize {0,0};
	
	Array< ColorBuffer >	_colorBuffers;
	DepthBuffer				_depthBuffer;
};

} // namespace
