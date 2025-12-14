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

GpuBuffer_Backend::MapScope GpuBuffer_Backend::mapMemory(IntRange range) {
	if (!bufferRange().contains(range))
		throw Error_IndexOutOfRange();
	return MapScope(this, onMapMemory(range));
}

void GpuBuffer_Backend::flush(IntRange range) {
	if (range.size() <= 0) return;

	if (!bufferRange().contains(range))
		throw Error_IndexOutOfRange();
	onFlush(range);
}

SPtr<RenderColorBuffer_Backend> RenderColorBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderColorBuffer(req, desc));
}


SPtr<RenderDepthBuffer_Backend> RenderDepthBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderDepthBuffer(req, desc));
}

} // namespace