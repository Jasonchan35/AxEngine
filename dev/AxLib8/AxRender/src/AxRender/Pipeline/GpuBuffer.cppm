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

class DynamicGpuBuffer;

class GpuBufferPool_CreateDesc : public NonCopyable {
public:
	String name;
	GpuBufferType bufferType = GpuBufferType::None;
	Int maxSize  = 0;
	Int pageSize = 64 * 1024;
	Int alignment = 0;
	
	GpuBufferPool_CreateDesc() = default;

	GpuBufferPool_CreateDesc(StrView name_, GpuBufferType bufferType_, Int maxSize_, Int pageSize_, Int alignment_ = 0) 
	: name(name_)
	, bufferType(bufferType_)
	, maxSize(maxSize_)
	, pageSize(pageSize_) 
	, alignment(alignment_){}
};

class GpuBufferPool : public RenderObject {
	AX_RTTI_INFO(GpuBufferPool, RenderObject)
public:
	using CreateDesc = GpuBufferPool_CreateDesc;

	GpuBufferType bufferType() const { return _bufferType; }
	Int maxSize() const { return _maxSize; }
	Int pageSize() const { return _pageSize; }
	
	AX_INLINE Int calcPageIndex(Int v) const { return Math::alignTo(v, _pageSize) / _pageSize; } 
	
protected:
	GpuBufferPool(const CreateDesc& desc);
	
	virtual void onAllocateBlock(GpuBuffer* buf) {}
	virtual void onFreeBlock(GpuBuffer* buf) {}
	
	GpuBufferType _bufferType = GpuBufferType::None;
	Int _maxSize   = 0; // virtual memory address size
	Int _pageSize  = 0;
	Int _alignment = 0;
};

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
	GpuBufferPool* pool = nullptr;
};

class GpuBuffer : public RenderObject {
	AX_RTTI_INFO(GpuBuffer, RenderObject)
public:
	using CreateDesc = GpuBuffer_CreateDesc;
	
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, StrView name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}

	GpuBufferType  type() const { return _type; }
	Int            bufferOffset() const { return _bufferOffset; }
	Int            size() const { return _size; }
	
	GpuBufferPool* pool() { return _pool; }  
	
	bool inBound(IntRange range) const { return IntRange(_size).contains(range); }

protected:
	friend class GpuBufferPool_Backend;
	
	GpuBuffer(const CreateDesc& desc);

	GpuBufferType              _type   = GpuBufferType::None;
	Int                        _size   = 0;
	Int                        _bufferOffset = 0;
	GpuBufferPool*             _pool   = nullptr;
	D3D12MA::VirtualAllocation _virtualAllocation {};
};

class DynamicGpuBuffer_CreateDesc : public NonCopyable {
public:
	DynamicGpuBuffer_CreateDesc() = default;
	DynamicGpuBuffer_CreateDesc(StrView name_, GpuBufferType bufferType_, Int bufferSize_, GpuBufferPool* pool_)
		: name(name_), bufferType(bufferType_), bufferSize(bufferSize_), pool(pool_) {}

	String			name;
	GpuBufferType	bufferType = GpuBufferType::None;
	Int				bufferSize = 0;
	GpuBufferPool*	pool = nullptr;
};

class DynamicGpuBuffer : public NonCopyable {
public:
	using CreateDesc = DynamicGpuBuffer_CreateDesc;

	void create(const CreateDesc& desc);
	void destroy();
	void reset();

	Int dataSize() const { return _cpuBuffer.size(); }
	Int dataCapacity() const { return _cpuBuffer.capacity();}

	void ensureDataCapacity(Int s) { _cpuBuffer.ensureCapacity(s); }

	void setData(ByteSpan src, Int offset = 0);
	void appendData(ByteSpan src);
	
	MutByteSpan extendSize(Int sizeInBytes);

	// remember to markDirty after write
	MutByteSpan mutSpan() { return _cpuBuffer; }
	void markDirty(IntRange range);

	const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
		return ax_const_cast(this)->_getUploadedGpuBuffer(req);
	}
	
private:
	GpuBuffer*	_getUploadedGpuBuffer(class RenderRequest* req);

	String          _name;
	GpuBufferType   _bufferType = GpuBufferType::None;
	GpuBufferPool*  _pool       = nullptr;
	Array<Byte>     _cpuBuffer;
	SPtr<GpuBuffer> _gpuBuffer;
	IntRange        _dirtyRange = {};
};

inline
void DynamicGpuBuffer::markDirty(IntRange range) {
	if (range.size() <= 0) return;
	_dirtyRange |= range;
}

inline
void DynamicGpuBuffer::setData(ByteSpan src, Int offset) {
	auto newSize = offset + src.size();
	if (_cpuBuffer.size() < newSize) {
		throw Error_IndexOutOfRange();
	}

	auto range = Range_StartAndSize(offset, src.size());
	_cpuBuffer.copyValues(src, offset);
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
	auto range = IntRange_StartAndSize(_cpuBuffer.size(), sizeInBytes);
	markDirty(range);
	_cpuBuffer.resize(_cpuBuffer.size() + sizeInBytes);
	return _cpuBuffer.slice(range);
}

class GpuStructuredBuffer_CreateDesc : public NonCopyable {
public:
	String name;
	Int stride = 0;
	Int count = 0;
	GpuBufferPool* pool = nullptr;
};

class GpuStructuredBuffer : public RenderObject {
	AX_RTTI_INFO(GpuStructuredBuffer, RenderObject)
public:
	using CreateDesc = GpuStructuredBuffer_CreateDesc;
	static SPtr<GpuStructuredBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);

	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(RenderRequest* req) const {
		AX_ASSERT_TODO()
		return nullptr;
	}	

	Int stride() const { return _stride; }
	Int count() const { return _count; }

	template<class T>
	void setValue(Int index, const T& value) { setByteData(index, Span(value).toByteSpan()); }
	
protected:
	GpuStructuredBuffer(const CreateDesc& desc);
	SPtr<GpuBuffer> _gpuBuffer;
	Int _stride = 0;
	Int _count = 0;
};

} // namespace