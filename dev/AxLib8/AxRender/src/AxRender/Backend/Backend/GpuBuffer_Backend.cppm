module;
export module AxRender:GpuBuffer_Backend;
export import :RenderSystem_Backend;
export import :RenderObjectTable;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

class GpuBufferPool_Backend : public GpuBufferPool {
	AX_RTTI_INFO(GpuBufferPool_Backend, GpuBufferPool)
public:
	static SPtr<GpuBufferPool_Backend> s_new(const MemAllocRequest& mem, const CreateDesc& desc);

	static SPtr<GpuBufferPool_Backend> s_new(const MemAllocRequest& mem,
	                                         InNameId               name,
	                                         GpuBufferType          bufferType,
	                                         Int                    maxSize,
	                                         Int                    pageSize,
	                                         Int                    blockAlignment)
	{
		return s_new(mem, CreateDesc(name, bufferType, maxSize, pageSize, blockAlignment));
	}
	
	
	virtual void onGpuUpdatePages(RenderRequest_Backend* req) {}

	void _allocateBlock(GpuBuffer* buf);
	void _freeBlock(GpuBuffer* buf);
	
protected:
	GpuBufferPool_Backend(const CreateDesc& desc);
	
	inline void _throwIfError(HRESULT hr) { if (hr < 0) throw Error_Undefined(); }
	
	struct Page {
		Int  _refCount = 0;
		bool _commited = false;
	};
	
	template<class PAGE>
	struct PagePool_ {
		void create(const CreateDesc& desc) {
			auto pageCount = Math::alignTo(desc.maxSize, desc.pageSize) / desc.pageSize;
			_pages.resize(pageCount);
//			AX_LOG("--- Create GpuBufferPool maxSize={} pageSize={} pageCount={}", desc.maxSize, desc.pageSize, pageCount);
		}
		
		void commitPageForRange(IntRange rangeInBytes, Int pageSize) {
			auto s = rangeInBytes.start() / pageSize;
			auto e = Math::alignTo(rangeInBytes.stop(), pageSize) / pageSize;
			for (auto i = s; i < e; ++i) {
				auto& page = _pages[i];
				if (!page._commited) {
					page._commited = true;
					_pendingCommitPages.emplaceBack(i);
				}
				++page._refCount;
			}
		}
		
		void uncommitPageForRange(IntRange rangeInBytes, Int pageSize) {
			auto s = rangeInBytes.start() / pageSize;
			auto e = Math::alignTo(rangeInBytes.stop(), pageSize) / pageSize;
			for (auto i = s; i < e; ++i) {
				auto& page = _pages[i];
				--page._refCount;
				AX_ASSERT(page._refCount >= 0);
			}
		}
		
		void onAllocateBlock(GpuBuffer* buf, Int pageSize) {
			commitPageForRange(IntRange_StartAndSize(buf->_bufferOffsetInBytes, buf->_size), pageSize);
		}
		
		void onFreeBlock(GpuBuffer* buf, Int pageSize) {
			uncommitPageForRange(IntRange_StartAndSize(buf->_bufferOffsetInBytes, buf->_size), pageSize);
		}
		
		Array<Int, 64>	_pendingCommitPages;
		Array<PAGE>		_pages;
	};	
	
	ComPtr<D3D12MA::VirtualBlock>	_virtualBlock;
};

class GpuBuffer_Backend : public GpuBuffer {
	AX_RTTI_INFO(GpuBuffer_Backend, GpuBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}

	void copyData(ByteSpan data, Int offset = 0);
	void copyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset);

	void		_unmapMemory() { onUnmapMemory(); }
	MutByteSpan _mapMemory(IntRange range);
	
	using ScopedMapMemory = ScopedMemFuncProxy0<MutByteSpan, This, &This::_unmapMemory>;
	AX_NODISCARD AX_INLINE	ScopedMapMemory mapMemory() { return mapMemory(IntRange(_size)); }
	AX_NODISCARD AX_INLINE	ScopedMapMemory mapMemory(IntRange range) {
		return ScopedMapMemory(_mapMemory(range), this);
	}

	void flush(IntRange range);

	~GpuBuffer_Backend();
	
protected:
	virtual MutByteSpan	onMapMemory(IntRange range) = 0;
	virtual void		onUnmapMemory() = 0;
	virtual void		onFlush(IntRange range) = 0;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) = 0;
	GpuBuffer_Backend(const CreateDesc& desc);
};

inline MutByteSpan GpuBuffer_Backend::_mapMemory(IntRange range) {
	if (!IntRange(_size).contains(range))
		throw Error_IndexOutOfRange();
	return onMapMemory(range);
}

class StructuredGpuBuffer_Backend : public StructuredGpuBuffer {
	AX_RTTI_INFO(StructuredGpuBuffer_Backend, StructuredGpuBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	StructuredGpuBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

} // namespace
