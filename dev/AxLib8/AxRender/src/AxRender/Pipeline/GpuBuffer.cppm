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
	NameId name;
	GpuBufferType bufferType = GpuBufferType::None;
	Int maxSize  = 0;
	Int pageSize = 64 * 1024;
	Int blockAlignment = 0;
	
	GpuBufferPool_CreateDesc() = default;

	GpuBufferPool_CreateDesc(InNameId      name_,
	                         GpuBufferType bufferType_,
	                         Int           maxSize_,
	                         Int           pageSize_,
	                         Int           blockAlignment_) 
	: name(name_)
	, bufferType(bufferType_)
	, maxSize(maxSize_)
	, pageSize(pageSize_) 
	, blockAlignment(blockAlignment_) {}
};

class GpuBufferPool : public RenderObject {
	AX_RTTI_INFO(GpuBufferPool, RenderObject)
public:
	using CreateDesc = GpuBufferPool_CreateDesc;
	
	static SPtr<GpuBufferPool> s_new(const MemAllocRequest& mem, const CreateDesc& desc);

	static SPtr<GpuBufferPool> s_new(const MemAllocRequest& mem,
	                                 InNameId               name,
	                                 GpuBufferType          bufferType,
	                                 Int                    maxSize,
	                                 Int                    pageSize,
	                                 Int                    blockAlignment)
	{
		return s_new(mem, CreateDesc(name, bufferType, maxSize, pageSize, blockAlignment));
	}

	GpuBufferType bufferType() const { return _bufferType; }
	Int maxSize() const { return _maxSize; }
	Int pageSize() const { return _pageSize; }
	
	AX_INLINE Int calcPageIndex(Int v) const { return Math::alignTo(v, _pageSize) / _pageSize; } 
	
	Int blockAlignment() const { return _blockAlignment; }

protected:
	GpuBufferPool(const CreateDesc& desc);
	
	virtual void onAllocateBlock(GpuBuffer* buf) {}
	virtual void onFreeBlock(GpuBuffer* buf) {}
	
	GpuBufferType _bufferType = GpuBufferType::None;
	Int _maxSize   = 0; // virtual memory address size
	Int _pageSize  = 0;
	Int _blockAlignment = 0;
};

class GpuBuffer_CreateDesc : public NonCopyable {
public:
	GpuBuffer_CreateDesc() = default;

	GpuBuffer_CreateDesc(InNameId      name_,
	                     GpuBufferType bufferType_,
	                     Int           bufferSize_)
		: name(name_)
		, bufferType(bufferType_)
		, bufferSize(bufferSize_)
	{}

	NameId        name;
	GpuBufferType bufferType = GpuBufferType::None;
	Int           bufferSize = 0;
	GpuBufferPool* pool = nullptr;
};

class GpuBuffer : public RenderObject {
	AX_RTTI_INFO(GpuBuffer, RenderObject)
public:
	using CreateDesc = GpuBuffer_CreateDesc;
	
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<GpuBuffer> s_new(const MemAllocRequest& req, InNameId name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}

	GpuBufferType type() const { return _type; }
	Int           size() const { return _size; }
	Int           bufferOffset() const { return _bufferOffset; }
	IntRange      bufferRange() const { return IntRange_StartAndSize(_bufferOffset, _size); }
	
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
	DynamicGpuBuffer_CreateDesc(InNameId name_, GpuBufferType bufferType_, Int bufferSize_, GpuBufferPool* pool_)
		: name(name_), bufferType(bufferType_), bufferSize(bufferSize_), pool(pool_) {}

	NameId			name;
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
	
	void ensureSize(Int newSize) { _cpuBuffer.ensureSize(newSize); }
	void setSize(Int newSize);
	
	MutByteSpan extendSize(Int sizeInBytes);

	// remember to markDirty after write
	MutByteSpan mutSpan() { return _cpuBuffer; }
	void markDirty(IntRange range);

	const GpuBuffer* getUploadedGpuBuffer(class RenderRequest* req) const {
		return ax_const_cast(this)->_getUploadedGpuBuffer(req);
	}
	
	Int gpuBufferOffset() const { return _gpuBuffer ? _gpuBuffer->bufferOffset() : 0; }
	
private:
	GpuBuffer*	_getUploadedGpuBuffer(class RenderRequest* req);

	NameId          _name;
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
void DynamicGpuBuffer::setSize(Int newSize) {
	Int oldSize = _cpuBuffer.size();
	_cpuBuffer.resize(newSize);
	
	if (newSize < oldSize) return;
	markDirty(IntRange_StartAndSize(oldSize, newSize - oldSize));
}

inline
MutByteSpan DynamicGpuBuffer::extendSize(Int sizeInBytes) {
	Int oldSize = _cpuBuffer.size();
	setSize(oldSize + sizeInBytes);

	auto range = IntRange_StartAndSize(oldSize, sizeInBytes);
	return _cpuBuffer.slice(range);
}

class StructuredGpuBuffer_CreateDesc : public NonCopyable {
public:
	NameId         name;
	Int            stride = 0;
	GpuBufferPool* pool   = nullptr;
};

class StructuredGpuBuffer : public RenderObject {
public:
	using CreateDesc = StructuredGpuBuffer_CreateDesc;

	template<class T>
	static SPtr<StructuredGpuBuffer> s_new(const MemAllocRequest& req, InNameId name, GpuBufferPool* pool) {
		CreateDesc desc;
		desc.name = name;
		desc.stride = AX_SIZEOF(T);
		desc.pool = pool;
		return s_new(req, desc);
	}
	
	static SPtr<StructuredGpuBuffer> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	AX_INLINE const GpuBuffer* getUploadedGpuBuffer(RenderRequest* req) const {
		return _gpuBuffer.getUploadedGpuBuffer(req);
	}	

	Int stride() const { return _stride; }
	Int count() const { return _gpuBuffer.dataSize() / _stride; }

	template<class T>
	MutSpan<T> editData(Int offset, Int size) {
		if (AX_SIZEOF(T) != _stride) throw Error_Undefined();
		auto byteRange = IntRange_StartAndSize(offset, size) * AX_SIZEOF(T);
		auto reqBufferSizeInBytes = byteRange.stop();
		
		_gpuBuffer.ensureSize(reqBufferSizeInBytes);
		_gpuBuffer.markDirty(byteRange);
		auto byteSpan = _gpuBuffer.mutSpan().slice(byteRange);
		return MutSpan<T>::s_fromMutByteSpan(byteSpan);
	}

	template<class T>
	void setValue(Int index, const T& v) { setValues(index, Span(v)); }
	
	template<class T>
	void setValues(Int index, Span<T> span) {
		if (AX_SIZEOF(T) != _stride) throw Error_Undefined();
		auto mutSpan = editData<T>(index, span.size());
		mutSpan.copyValues(span);
	}
	
	Int gpuBufferIndex() const { return _gpuBuffer.gpuBufferOffset() / _stride; }
	
protected:
	StructuredGpuBuffer(const CreateDesc& desc);
	
	DynamicGpuBuffer _gpuBuffer;
	Int _stride = 0;
};

template<class T>
class StructuredGpuBufferPool_ : public NonCopyable {
public:
	static_assert(Math::isPow2(AX_SIZEOF(T))); // D3DMA allocator require pow of 2
	void create(const MemAllocRequest& req, InNameId name, Int maxSize, Int pageSize) {
		pool = GpuBufferPool::s_new(req, name, GpuBufferType::Structured, maxSize, pageSize, AX_SIZEOF(T));
	}

	operator GpuBufferPool* () { return pool.ptr(); }
	GpuBufferPool* operator->() { return pool.ptr(); }
	
	SPtr<GpuBufferPool> pool;
};

template<class T>
class StructuredGpuBuffer_ : public NonCopyable {
public:
	using Pool = StructuredGpuBufferPool_<T>;
	
	void create(const MemAllocRequest& req, InNameId name, Pool& pool) {
		buffer = StructuredGpuBuffer::s_new<T>(req, name, pool.pool);
	}

	MutSpan<T> editData(Int offset, Int size)	{ return buffer->editData<T>(offset, size); }
	MutSpan<T> extendsData(Int size)			{ return editData(buffer->count(), size); }
	
	void setValue(Int index, const T& v)      	{ return buffer->setValue(index, Span(v)); }
	void setValues(Int index, Span<T> span)   	{ return buffer->setValues(index, span); }

	SPtr<StructuredGpuBuffer> buffer;
};


} // namespace