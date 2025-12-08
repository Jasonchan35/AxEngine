module AxRender;
import :RenderContext_Backend;
import :RenderRequest;
import :Renderer_Backend;

namespace ax::AxRender {

UPtr<RenderContext> RenderContext::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	auto o = Renderer_Backend::s_instance()->newRenderContext(req, desc);
	o->onPostCreate(desc);
	return o;
}

RenderContext::RenderContext(const CreateDesc& desc) {
	AX_ASSERT(desc.backBufferCount <= AxRenderConfig::kMaxBackBufferCount);

	_requestBackBufferCount	= desc.backBufferCount;

	_renderer			= Renderer::s_instance(); // for debug
	_window				= desc.window;
	_vsync				= desc.vsync;

	_colorBufferDesc	= desc.colorBuffer;
	_depthBufferDesc	= desc.depthBuffer;
}

RenderColorBuffer* RenderContext::backColorBuffer(Int i) {
	auto* p = onGetBackBuffer(i);
	return p ? p->_colorBuf.ptr() : nullptr;
}

void RenderContext::onPostCreate(const CreateDesc& desc) {
	
}


} // namespace

