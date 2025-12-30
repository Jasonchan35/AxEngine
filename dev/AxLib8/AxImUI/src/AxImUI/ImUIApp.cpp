module AxImUI;
import :ImUIApp;

namespace AxUI {

void ImUIApp::onCreate() {
	Base::onCreate();

	_renderSystem = RenderSystem::s_create(_renderSystemDesc);
}

} // namespace
