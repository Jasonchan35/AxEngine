module AxImUI;
import :ImUIApp;

namespace ax {

void ImUIApp::onCreate() {
	Base::onCreate();

	_renderSystem = RenderSystem::s_create(_renderSystemDesc);
}

} // namespace
