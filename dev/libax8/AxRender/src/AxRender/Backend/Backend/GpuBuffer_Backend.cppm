module;
export module AxRender:GpuBuffer_Backend;
export import :Renderer_Backend;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

class GpuBuffer_Backend : public GpuBuffer {
	AX_RTTI_INFO(GpuBuffer_Backend, GpuBuffer)
	
	void _unmapMemory() { onUnmapMemory(); }
public:
	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView name, GpuBufferType type, Int size) {
		return s_new(req, CreateDesc(name, type, size));
	}

	void copyData(ByteSpan data, Int offset = 0);

	void copyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
		Int copySize = srcRange.size();
		auto dstRange = Range_BeginSize(dstOffset, copySize);
		if (! src->inBound(srcRange)) throw Error_IndexOutOfRange();
		if (!this->inBound(dstRange)) throw Error_IndexOutOfRange();
		onCopyFromGpuBuffer(req, src, srcRange, dstOffset);
	}

	using ScopeMapMemory = ScopeDataProxy0<MutByteSpan, This, &This::_unmapMemory>;
	AX_NODISCARD AX_INLINE	ScopeMapMemory mapMemory() { return mapMemory(IntRange(bufferSize())); }
	AX_NODISCARD AX_INLINE	ScopeMapMemory mapMemory(IntRange range);

	void flush(IntRange range);

protected:

	virtual MutByteSpan	onMapMemory(IntRange range) = 0;
	virtual void		onUnmapMemory() = 0;
	virtual void		onFlush(IntRange range) = 0;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) = 0;


	GpuBuffer_Backend(const CreateDesc& desc) : Base(desc) {}
};

AX_INLINE auto GpuBuffer_Backend::mapMemory(IntRange range) -> ScopeMapMemory {
	if (!bufferRange().contains(range))
		throw Error_IndexOutOfRange();
	return ScopeMapMemory(onMapMemory(range), this);
}


} // namespace
