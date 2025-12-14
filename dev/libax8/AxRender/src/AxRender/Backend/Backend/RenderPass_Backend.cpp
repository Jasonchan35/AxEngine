module;
module AxRender;
import :Renderer_Backend;
import :RenderPass_Backend;

namespace ax /*::AxRender*/ {

SPtr<RenderPass_Backend> RenderPass_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newRenderPass(req, desc));
}


} // namespace
