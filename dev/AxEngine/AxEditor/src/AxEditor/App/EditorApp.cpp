module;

module AxEditor;

import :EditorApp;
import :EditorMainWindow;

namespace AxEditor {

class EditorApp_CreateDesc : public EditorApp::CreateDesc {
public:
	EditorApp_CreateDesc() {
		peekMessage = true;
//		renderSystemDesc.info.api = RenderAPI::Null;
		renderSystemDesc.info.api = RenderAPI::Dx12;
		renderSystemDesc.info.api = RenderAPI::Vk;

		using App = NativeUIApp;
		using KeyCode = NativeUIKeyCode;
		
		// if (App::s_getAsyncKeyState(KeyCode::Ctrl)) {
		// 	renderSystemDesc.info.api = RenderAPI::Dx12;
		// } else if (App::s_getAsyncKeyState(KeyCode::Shift)) {
		// 	renderSystemDesc.info.api = RenderAPI::Vk;
		// }
	}
};

EditorApp::EditorApp() 
: Base(EditorApp_CreateDesc()) {
	auto dir = FilePath::dirname(currentExecuteFilename());
	
	#if AX_OS_MACOSX
		FilePath::setCurrentDir(Fmt("{}/../../../", dir));
	#elif AX_OS_WINDOWS
		FilePath::setCurrentDir(Fmt("{}/../../../../../deploy/", dir));
	#endif
}

EditorApp::~EditorApp() {
}

void EditorApp::onCreate() {
	Base::onCreate();
//	AX_LOG("CurrentDir = {}",FilePath::currentDir());

	auto curDir = FilePath::currentDir();
	AX_LOG("CurrentDir = \"{}\"", curDir);

	_watcher.dgResults.bindUnowned(this, &This::_onFileChanged);
	_watcher.create(curDir);

	_mainWin = UPtr_new<EditorMainWindow>(AX_NEW);
	
	Engine::CreateDesc engineDesc;
	engineDesc.inEditor = true;
	_engine.create(engineDesc);
	
	_createDemoScene();
}

void EditorApp::onPeekMessage() {
	if (_mainWin) _mainWin->render();
}

void EditorApp::_onFileChanged(FileDirWatcher_Result& result) {
	if (auto* renderSystem = RenderSystem::s_instance()) {
		renderSystem->onFileChanged(result);
	}
}

void EditorApp::_createDemoScene() {
	auto* stockObjs = RenderStockObjects::s_instance();
	
	for (Int i = 0; i < 5; ++i) {
		auto entity = SceneEntity::s_new(AX_NEW, nullptr, Fmt("test_{}", i));
		entity->transform.position = Vec3f(static_cast<f32>(i) * 2.0f, 0, 0);

		auto* meshRenderer     = entity->addComponent<CMeshRenderer>(AX_NEW);
		meshRenderer->mesh     = stockObjs->meshes->cube;
		meshRenderer->material = stockObjs->materials->simple3d_color;
		
		for (Int j = 0; j < 3; ++j) {
			SceneEntity::s_new(AX_NEW, entity, Fmt("child_{}.{}", i, j));
		}
	}
}

} //namespace
