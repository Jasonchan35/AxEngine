module;
export module AxRender:GpuBuffer;
export import :Renderer;
export import :Texture;

export namespace ax /*::AxRender*/ {

#define AX_RENDER_GpuBufferType_ENUM_LIST(E) \
	E(None,			) \
	E(Vertex,		) \
	E(Index,		) \
	E(Const,		) \
	E(Storage,		) \
	E(StagingToGpu,	) \
	E(StagingToCpu,	) \
	\
	E(RayTracingShaderRecord,) \
	E(RayTracingScratch,) \
	E(RayTracingInstanceDesc,) \
	E(RayTracingAccelStruct,) \
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

	bool inBound(IntRange range) const { return IntRange(_bufferSize).contains(range); }

protected:
	GpuBuffer(const CreateDesc& desc);
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
	const GpuBuffer* gpuBuffer() const { return _gpuBuffer; }

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
	
	MutByteSpan extendSize(Int sizeInBytes);

	// remember to markDirty after write
	MutByteSpan mutSpan() { return _data; }
	void markDirty(IntRange range);

	const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
		return ax_const_cast(this)->_getUploadedGpuBuffer(req);
	}
	
private:
	GpuBuffer*	_getUploadedGpuBuffer(class RenderRequest* req);

	String	_name;
	GpuBufferType	_bufferType = GpuBufferType::None;

	Array<Byte>			_data;
	SPtr<GpuBuffer>		_gpuBuffer;

	IntRange			_dirtyRange = {};
};

inline
void DynamicGpuBuffer::markDirty(IntRange range) {
	if (range.size() <= 0) return;
	_dirtyRange |= range;
}

inline
void DynamicGpuBuffer::setData(ByteSpan src, Int offset) {
	auto newSize = offset + src.size();
	if (_data.size() < newSize) {
		throw Error_IndexOutOfRange();
	}

	auto range = Range_StartAndSize(offset, src.size());
	_data.copyValues(src, offset);
	markDirty(range);
}

inline
void DynamicGpuBuffer::appendData(ByteSpan src) {
	if (src.size() <= 0) return;
	auto span = extendSize(src.size());
	span.copyValues(src);
}

inline
MutByteSpan DynamicGpuBuffer::extendSize(Int sizeInBytes) {
	auto range = IntRange_StartAndSize(_data.size(), sizeInBytes);
	markDirty(range);
	_data.resize(_data.size() + sizeInBytes);
	return _data.slice(range);
}

} // namespace