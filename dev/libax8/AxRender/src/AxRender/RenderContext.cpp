module AxRender;
import :RenderContext_Backend;
import :RenderRequest;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

UPtr<RenderContext> RenderContext::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	auto o = RenderSystem_Backend::s_instance()->newRenderContext(req, desc);
	o->onPostCreate(desc);
	return o;
}

RenderContext::RenderContext(const CreateDesc& desc) {
	AX_ASSERT(desc.swapChainDesc.backBufferCount <= AxRenderConfig::kMaxBackBufferCount);
	_renderSystem      = RenderSystem::s_instance(); // for debug
	_window        = desc.window;
	_swapChainDesc = desc.swapChainDesc;
}

} // namespace

