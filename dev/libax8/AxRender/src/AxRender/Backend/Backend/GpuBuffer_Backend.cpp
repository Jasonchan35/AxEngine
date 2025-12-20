module;
module AxRender;
import :GpuBuffer_Backend;
import :Renderer_Backend;

namespace ax /*::AxRender*/ {

SPtr<GpuBuffer_Backend> GpuBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newGpuBuffer(req, desc));
}

void GpuBuffer_Backend::copyData(ByteSpan data, Int offset) {
	auto map = mapMemory(Range_BeginSize(offset, data.size()));
	map->copyValues(data);
}

void GpuBuffer_Backend::copyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) {
	static auto copyAlignment = Renderer::s_instance()->copyGpuBufferAlignment();
	AX_ASSERT(Math::isAlignedTo(srcRange.begin(), copyAlignment));
	AX_ASSERT(Math::isAlignedTo(srcRange.end()  , copyAlignment));
	AX_ASSERT(Math::isAlignedTo(dstOffset       , copyAlignment));

	Int  copySize = srcRange.size();
	auto dstRange = Range_BeginSize(dstOffset, copySize);
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



} // namespace