module;
module AxRender;
import :RenderTarget_Backend;
import :Renderer_Backend;

namespace ax /*::AxRender*/ {

SPtr<RenderTargetColorBuffer_Backend> RenderTargetColorBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderTargetColorBuffer(req, desc));
}


SPtr<RenderTargetDepthBuffer_Backend> RenderTargetDepthBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderTargetDepthBuffer(req, desc));
}

} // namespace