module;
export module AxRender:GpuBuffer;
export import :Renderer;
export import :Texture;

export namespace ax /*::AxRender*/ {

#define AX_RENDER_GpuBufferType_ENUM_LIST(E) \
	E(None,			) \
	E(Vertex,		) \
	E(Index,		) \
	E(Uniform,		) \
	E(Storage,		) \
	E(StagingToGpu,	) \
	E(StagingToCpu,	) \
//----
AX_ENUM_CLASS(AX_RENDER_GpuBufferType_ENUM_LIST, GpuBufferType, u8)

class GpuBuffer_CreateDesc : public NonCopyable {
public:
	GpuBuffer_CreateDesc() = default;
	GpuBuffer_CreateDesc(StrView name_, GpuBufferType bufferType_, Int bufferSize_)
		: name(name_)
		, bufferType(bufferType_)
		, bufferSize(bufferSize_)
	{}

	StrView			name;
	GpuBufferType	bufferType = GpuBufferType::None;
	Int				bufferSize = 0;
};

class GpuBuffer : public RenderObject {
	AX_RTTI_INFO(GpuBuffer, RenderObject)
public:
	using CreateDesc = GpuBuffer_CreateDesc;
	
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, StrView name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}
	
	GpuBufferType	bufferType() const	{ return _bufferType; }
	Int				bufferSize() const	{ return _bufferSize; }
	IntRange		bufferRange() const { return IntRange(_bufferSize); }


protected:
	GpuBuffer(const CreateDesc& desc);

private:
	GpuBufferType	_bufferType = GpuBufferType::None;
	Int				_bufferSize = 0;
};

class StorageBuffer_CreateDesc : public NonCopyable {
public:
	String name;
	Int	bufferSize = 0;
};

class StorageBuffer : public RenderObject {
	AX_RTTI_INFO(StorageBuffer, RenderObject)
public:
	using CreateDesc = StorageBuffer_CreateDesc;

	Int bufferSize() const { return _gpuBuffer ? _gpuBuffer->bufferSize() : 0; }
	GpuBuffer* gpuBuffer() { return _gpuBuffer; }

protected:
	StorageBuffer(const CreateDesc& desc);
	SPtr<GpuBuffer>	_gpuBuffer;
};

class DynamicGpuBuffer_CreateDesc : public NonCopyable {
public:
	DynamicGpuBuffer_CreateDesc() = default;
	DynamicGpuBuffer_CreateDesc(StrView name_, GpuBufferType bufferType_, Int bufferSize_)
		: name(name_), bufferType(bufferType_), bufferSize(bufferSize_) {}

	StrView			name;
	GpuBufferType	bufferType = GpuBufferType::None;
	Int				bufferSize = 0;
};

class DynamicGpuBuffer : public NonCopyable {
public:
	using CreateDesc = DynamicGpuBuffer_CreateDesc;

	void create(StrView name, GpuBufferType bufferType, Int bufferSize) { create(CreateDesc(name, bufferType, bufferSize)); }
	void create(const CreateDesc& desc);
	void reset();

	Int dataSize() const { return _data.size(); }

	void ensureDataCapacity(Int s) { _data.ensureCapacity(s); }

	void setData(ByteSpan src, Int offset = 0);
	void appendData(ByteSpan src);

	// remember to markDirty after write
	MutByteSpan mutSpan() { return _data; }
	void markDirty(IntRange range);

	GpuBuffer*	getUploadedGpuBuffer(class RenderRequest* req);

private:

	String	_name;
	GpuBufferType	_bufferType = GpuBufferType::None;

	Array<Byte>		_data;
	SPtr<GpuBuffer> _gpuBuffer;

	Int				_currentSlotIndex = 0;
	RenderSeqId		_lastUpdateRenderSeqId = 0;

	struct Slot {
		SPtr<GpuBuffer>	uploadBuffer;
		IntRange		dirtyRange;
	};

	IntRange	_dirtyRange = {};
	Array<Slot, 8> _slots;
};

inline
void DynamicGpuBuffer::markDirty(IntRange range) {
	if (range.size() <= 0) return;

	_dirtyRange |= range;

	for (auto& slot : _slots) {
		slot.dirtyRange |= range;
	}
}

inline
void DynamicGpuBuffer::setData(ByteSpan src, Int offset) {
	auto newSize = offset + src.size();
	if (_data.size() < newSize) {
		throw Error_IndexOutOfRange();
	}

	auto range = Range_BeginSize(offset, src.size());
	_data.copyValues(src, offset);
	markDirty(range);
}

inline
void DynamicGpuBuffer::appendData(ByteSpan src) {
	if (src.size() <= 0) return;

	auto range = Range_BeginSize(_data.size(), src.size());
	_data.resize(range.end());
	markDirty(range);
	setData(src, range.begin());
}


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
	RenderDepthType	depthType = RenderDepthType::None;
	Vec2i		frameSize {0,0};
};

class RenderDepthBuffer : public Texture {
	AX_RTTI_INFO(RenderDepthBuffer, Texture)
public:
	using CreateDesc = RenderDepthBuffer_CreateDesc;

	RenderDepthType	depthType() const { return _depthType; }
	Vec2i		frameSize() const { return _frameSize; }

protected:
	RenderDepthBuffer(const CreateDesc& desc);
	RenderDepthType	_depthType = RenderDepthType::None;
	Vec2i		_frameSize {0,0};
};


} // namespace