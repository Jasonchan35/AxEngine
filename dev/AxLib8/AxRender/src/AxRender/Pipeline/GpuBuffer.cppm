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
	E(Structured,	) \
	E(StagingToGpu,	) \
	E(StagingToCpu,	) \
	\
	E(RayTracingShaderRecord,) \
	E(RayTracingScratch,) \
	E(RayTracingInstanceDesc,) \
	E(RayTracingAccelStruct,) \
//----
AX_ENUM_CLASS(AX_RENDER_GpuBufferType_ENUM_LIST, GpuBufferType, u8)

class GpuVirtualAllocator;
class DynamicGpuBuffer;

class GpuBuffer_CreateDesc : public NonCopyable {
public:
	GpuBuffer_CreateDesc() = default;

	GpuBuffer_CreateDesc(StrView       name_,
	                     GpuBufferType bufferType_,
	                     Int           bufferSize_)
		: name(name_)
		, bufferType(bufferType_)
		, bufferSize(bufferSize_)
	{}

	String        name;
	GpuBufferType bufferType = GpuBufferType::None;
	Int           bufferSize = 0;
	GpuVirtualMemoryDesc virMemDesc;
};

class GpuBuffer : public RenderObject {
	AX_RTTI_INFO(GpuBuffer, RenderObject)
public:
	using CreateDesc = GpuBuffer_CreateDesc;
	
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, StrView name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}
	
	GpuBufferType bufferType() const			{ return _bufferType; }
	Int           bufferSize() const			{ return _bufferSize; }
	IntRange      bufferRange() const			{ return IntRange(_bufferSize); }
	const GpuVirtualMemoryDesc& virMemDesc() const { return _virMemDesc; }
	
	bool inBound(IntRange range) const { return IntRange(_bufferSize).contains(range); }

	void ensureCapacity(RenderRequest* req, Int newSize) {
		if (_bufferSize >= newSize) return;
		onSetCapacity(req, newSize);
		AX_ASSERT(_bufferSize >= newSize);
	}
	
protected:
	friend class DynamicGpuBuffer;
	GpuBuffer(const CreateDesc& desc);

	virtual void onSetCapacity(RenderRequest* req, Int newCapacity) = 0;

	GpuBufferType        _bufferType = GpuBufferType::None;
	Int                  _bufferSize = 0;
	GpuVirtualMemoryDesc _virMemDesc;
};


class GpuVirtualAllocator_CreateDesc : public NonCopyable {
public:
	GpuBufferType bufferType = GpuBufferType::None;
};

class GpuVirtualAllocator : public RenderObject {
	AX_RTTI_INFO(GpuVirtualAllocator, RenderObject)
public:
	using CreateDesc = GpuVirtualAllocator_CreateDesc;
	GpuVirtualAllocator(const CreateDesc& desc) {}
};

class DynamicGpuBuffer_CreateDesc : public NonCopyable {
public:
	DynamicGpuBuffer_CreateDesc() = default;
	DynamicGpuBuffer_CreateDesc(StrView name_, GpuBufferType bufferType_, Int bufferSize_)
		: name(name_), bufferType(bufferType_), bufferSize(bufferSize_) {}

	String			name;
	GpuBufferType	bufferType = GpuBufferType::None;
	Int				bufferSize = 0;
	GpuVirtualMemoryDesc virMemDesc;
};

class DynamicGpuBuffer : public NonCopyable {
public:
	using CreateDesc = DynamicGpuBuffer_CreateDesc;

	void create(StrView name, GpuBufferType bufferType, Int bufferSize) { create(CreateDesc(name, bufferType, bufferSize)); }
	void create(const CreateDesc& desc);
	void destroy();
	void reset();

	Int dataSize() const { return _data.size(); }
	Int dataCapacity() const { return _data.capacity();}

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

	Array<Byte>          _data;
	IntRange             _dirtyRange = {};
	SPtr<GpuBuffer>      _gpuBuffer;
	GpuVirtualMemoryDesc _virMemDesc;
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

class GpuStructuredBuffer_CreateDesc : public NonCopyable {
public:
	String name;
	Int stride = 0;
	Int capacity = 0;
};

class GpuStructuredBuffer : public RenderObject {
	AX_RTTI_INFO(GpuStructuredBuffer, RenderObject)
public:
	using CreateDesc = GpuStructuredBuffer_CreateDesc;

	static SPtr<GpuStructuredBuffer> s_new(const MemAllocRequest& req, StrView name, Int stride, Int capacity) {
		CreateDesc desc;
		desc.name = name;
		desc.stride = stride;
		desc.capacity = capacity;
		return s_new(req, desc);
	};
	static SPtr<GpuStructuredBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	Int dataCapacity() const { return _buffer.dataCapacity(); }
	
	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(RenderRequest* req) const {
		return _buffer.getUploadedGpuBuffer(req);
	}	

	Int stride() const { return _stride; }
	Int size() const { return _size; }
	Int capacity() const { return _capacity; }

	template<class T>
	void setValue(Int index, const T& value) { setByteData(index, Span(value).toByteSpan()); }
	
	void setByteData(Int index, ByteSpan data) {
		if (index < 0 || index >= _size) throw Error_IndexOutOfRange();
		if (data.size() != _stride) throw Error_Undefined();
		_buffer.setData(data, index * _stride);
	}

protected:
	GpuStructuredBuffer(const CreateDesc& desc);
	DynamicGpuBuffer _buffer;
	Int _stride = 0;
	Int _capacity = 0;
	Int _size = 0;
};

} // namespace