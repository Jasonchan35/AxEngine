module AxUI.UIRenderView;

namespace ax::AxUI {

void UIRenderView::createRenderContext(RenderContext::CreateDesc& desc) {
	_renderContext = RenderContext::s_new(AX_ALLOC_REQ, desc);
}

void UIRenderView::onSizeChanged(const Vec2f& size) {
	_renderContext->setFrameSize(Vec2i::s_cast(size));
}

void UIRenderView::render() {
	if (_renderContext) {
		_renderContext->render();
	}
}

} // namespace
