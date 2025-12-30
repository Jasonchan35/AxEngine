module;
module AxRender;
import :RenderSystem_Backend;
import :RenderPass_Backend;

namespace ax /*::AxRender*/ {

auto RenderPassColorBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc&      desc) -> SPtr<This> {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newRenderPassColorBuffer(req, desc));
}


SPtr<RenderPassDepthBuffer_Backend> RenderPassDepthBuffer_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newRenderPassDepthBuffer(req, desc));
}

SPtr<RenderPass_Backend> RenderPass_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newRenderPass(req, desc));
}


} // namespace
