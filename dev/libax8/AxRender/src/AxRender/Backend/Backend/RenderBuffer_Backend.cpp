module;
module AxRender;
import :RenderBuffer_Backend;
import :Renderer_Backend;

namespace ax::AxRender {

SPtr<RenderColorBuffer_Backend> RenderColorBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderColorBuffer(req, desc));
}


SPtr<RenderDepthBuffer_Backend> RenderDepthBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderDepthBuffer(req, desc));
}

} // namespace
