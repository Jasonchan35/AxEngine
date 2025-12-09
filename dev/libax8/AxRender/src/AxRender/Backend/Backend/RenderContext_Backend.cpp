module;
module AxRender;
import :RenderRequest_Backend;
import :GpuBuffer;
import :Renderer_Backend;

namespace ax::AxRender {

RenderContext_Backend::RenderContext_Backend(const CreateDesc& desc) 
: Base(desc)
{
}

void RenderContext_Backend::onPostCreate(const CreateDesc& desc) {
	auto* r = Renderer_Backend::s_instance();
	imgui.create(r->imguiFontAtlas());
	Base::onPostCreate(desc);
}

void RenderContext_Backend::onRender() {
	auto* renderer = Renderer_Backend::s_instance();

	auto* req = renderer->nextRenderRequest();

	auto* backBufferRenderPass = onAcquireBackBufferRenderPass(req);
	if (!backBufferRenderPass) return;

	imgui.onBeginRender(_frameSize); // TODO: move to frame update, to enable draw ui in update loop as well

	req->frameBegin(this, backBufferRenderPass);
	_renderGraph->_render(req);
	req->frameEnd();

	onPresentSurface(req);
}

void RenderContext_Backend::onUIMouseEvent(NativeUIMouseEvent& ev) {
	ev.pos = worldToLocalPos(ev.worldPos);
	imgui.onUIMouseEvent(ev);
}

void RenderContext_Backend::onUIKeyEvent(NativeUIKeyEvent& ev) {
	imgui.onUIKeyEvent(ev);
}

RenderContext_Backend::~RenderContext_Backend() {
	imgui.saveIniFile();
}

void RenderContext_Backend::onSetImGuiIniFilename(StrView name) {
	Base::onSetImGuiIniFilename(name);
	imgui.setIniFilename(name);
}

void RenderContext_Backend::onLoadImGuiIniFile() {
	imgui.loadIniFile();
}

void RenderContext_Backend::onSaveImGuiIniFile() {
	imgui.saveIniFile();
}

} // namespace
