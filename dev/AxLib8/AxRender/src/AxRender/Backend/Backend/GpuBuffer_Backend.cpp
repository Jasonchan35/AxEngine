module;
module AxRender;
import :GpuBuffer_Backend;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

SPtr<GpuBuffer_Backend> GpuBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newGpuBuffer(req, desc));
}

void GpuBuffer_Backend::copyData(ByteSpan data, Int offset) {
	auto map = mapMemory(Range_StartAndSize(offset, data.size()));
	map->copyValues(data);
}

void GpuBuffer_Backend::copyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	Int  copySize = srcRange.size();
	auto dstRange = Range_StartAndSize(dstOffset, copySize);
	if (! src->inBound(srcRange)) throw Error_IndexOutOfRange();
	if (!this->inBound(dstRange)) throw Error_IndexOutOfRange();
	onCopyFromGpuBuffer(req, src, srcRange, dstOffset);
}

void GpuBuffer_Backend::flush(IntRange range) {
	if (range.size() <= 0) return;

	if (!bufferRange().contains(range))
		throw Error_IndexOutOfRange();
	onFlush(range);
}

auto GpuStructuredBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) -> SPtr<This> {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newGpuStructuredBuffer(req, desc));
}

} // namespace