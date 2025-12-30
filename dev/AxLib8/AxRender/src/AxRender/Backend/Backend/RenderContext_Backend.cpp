module;
module AxRender;
import :RenderRequest_Backend;
import :GpuBuffer;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

RenderContext_Backend::RenderContext_Backend(const CreateDesc& desc) 
: Base(desc)
{
}

void RenderContext_Backend::onPostCreate(const CreateDesc& desc) {
	auto* r = RenderSystem_Backend::s_instance();
	imgui.create(r->imguiFontAtlas());
	Base::onPostCreate(desc);
}

void RenderContext_Backend::onRender() {
	auto* renderSystem = RenderSystem_Backend::s_instance();

	auto* req = renderSystem->nextRenderRequest();

	auto* backBufferRenderPass = onAcquireBackBufferRenderPass(req);
	if (!backBufferRenderPass) return;

	auto& renderGraphBackPass = _renderGraph->backBufferPass();
	if (auto* colorAtt0 = backBufferRenderPass->colorAttachment(0)) {
		colorAtt0->desc = renderGraphBackPass.color0.desc();
	}

	if (auto& depthAtt = backBufferRenderPass->depthAttachment()) {
		depthAtt.desc = renderGraphBackPass.depthBuffer().desc();
	}

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
