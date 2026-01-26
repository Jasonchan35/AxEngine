module;
export module AxRender:GpuBuffer_Backend;
export import :RenderSystem_Backend;
export import :RenderObjectSlot_Backend;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

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
	AX_NODISCARD AX_INLINE	ScopedMapMemory mapMemory() { return mapMemory(IntRange(bufferSize())); }
	AX_NODISCARD AX_INLINE	ScopedMapMemory mapMemory(IntRange range) { return ScopedMapMemory(_mapMemory(range), this); }

	void flush(IntRange range);

protected:
	virtual MutByteSpan	onMapMemory(IntRange range) = 0;
	virtual void		onUnmapMemory() = 0;
	virtual void		onFlush(IntRange range) = 0;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) = 0;
	GpuBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

inline MutByteSpan GpuBuffer_Backend::_mapMemory(IntRange range) {
	if (!bufferRange().contains(range))
		throw Error_IndexOutOfRange();
	return onMapMemory(range);
}

class GpuVirtualAllocator_Backend : public GpuVirtualAllocator {
	AX_RTTI_INFO(GpuVirtualAllocator_Backend, GpuVirtualAllocator)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	GpuVirtualAllocator_Backend(const CreateDesc& desc) : Base(desc) {}
	
	virtual void onAllocateGpuMemory(GpuBuffer* dstBuffer, Int size) {}
	virtual void onFreeGpuMemory(GpuBuffer* dstBuffer) {}
};

class GpuStructuredBuffer_Backend : public GpuStructuredBuffer {
	AX_RTTI_INFO(GpuStructuredBuffer_Backend, GpuStructuredBuffer)
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	
	GpuStructuredBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
	
	
};

} // namespace
