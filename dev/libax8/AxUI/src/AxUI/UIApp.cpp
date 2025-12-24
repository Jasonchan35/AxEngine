module AxUI;
import :UIApp;

namespace ax::AxUI {

void UIApp::onCreate() {
	Base::onCreate();

	_renderSystem = RenderSystem::s_create(_renderSystemDesc);
}

} // namespace
