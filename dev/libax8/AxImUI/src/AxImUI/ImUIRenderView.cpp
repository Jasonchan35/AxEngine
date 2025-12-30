module AxImUI;
import :ImUIRenderView;

namespace AxUI {

void ImUIRenderView::createRenderContext(RenderContext::CreateDesc& desc) {
	_renderContext = RenderContext::s_new(AX_ALLOC_REQ, desc);
}

void ImUIRenderView::onSizeChanged(const Vec2f& size) {
	_renderContext->setFrameSize(Vec2i::s_cast(size));
}

void ImUIRenderView::render() {
	if (_renderContext) {
		_renderContext->render();
	}
}

} // namespace
