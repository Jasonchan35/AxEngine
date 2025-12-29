module AxUI;
import :UIApp;

namespace AxUI {

void UIApp::onCreate() {
	Base::onCreate();

	_renderSystem = RenderSystem::s_create(_renderSystemDesc);
}

} // namespace
