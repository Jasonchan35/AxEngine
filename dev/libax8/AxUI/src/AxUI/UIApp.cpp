module AxUI.UIApp;

namespace ax::AxUI {

void UIApp::onCreate() {
	Base::onCreate();

	_renderer = Renderer::s_create(_rendererCreateDesc);

}

} // namespace
