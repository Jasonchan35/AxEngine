module;

module AxEditor;

import :EditorApp;
import :EditorMainWindow;
import :AxOpenUSD;
import :AxAssimp;

namespace AxEditor {

class EditorApp_CreateDesc : public EditorApp::CreateDesc {
public:
	EditorApp_CreateDesc() {
		peekMessage = true;
//		renderSystemDesc.info.api = RenderAPI::Null;
		renderSystemDesc.info.api = RenderAPI::Dx12;
//		renderSystemDesc.info.api = RenderAPI::Vk;

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
	
//	_createDemoScene();
	_testLoadFbx();
//	_testLoadOpenUsd();
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
	auto* world = Engine::s_instance()->world();
	
	for (Int i = 0; i < 5; ++i) {
		auto entity = SceneEntity::s_new(AX_NEW, world, nullptr, Fmt("test_{}", i));
		entity->setPosition(Vec3f(static_cast<f32>(i) * 2.0f, 0, 0));

		auto* comp     = entity->addComponent<MeshRendererComponent>(AX_NEW);
		comp->mesh     = stockObjs->meshes->Cube;
		comp->material = stockObjs->materials->Simple3D_Unlit_Color;
		
		for (Int j = 0; j < 3; ++j) {
			SceneEntity::s_new(AX_NEW, world, entity, Fmt("child_{}.{}", i, j));
		}
	}
}

void EditorApp::_testLoadOpenUsd() {
	AxOpenUSD usd;
	usd.openFile("Assets/Scenes/Kitchen_set/Kitchen_set.usd");
}

void EditorApp::_testLoadFbx() {
	bool enableCache = true;
	
	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/test.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/test2.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/test3.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/McGuire/sportsCar/sportsCar.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/McGuire/Exterior/exterior.fbx";

	String cacheFolder = Fmt("{}.axWorld", inFilename);
	String cacheRootFile = Fmt("{}/_root.axWorld", cacheFolder);

	SPtr<SceneWorld> world;
	if (enableCache && File::isNewerThan(cacheRootFile, inFilename)) {
		world = SceneWorld::s_new(AX_NEW);
		world->readFromFile(cacheFolder);
	} else {
		AxAssimp assimp;
		world = assimp.openFile(inFilename);
		world->writeToFile(cacheFolder);
	}
	_engine.setWorld(world);
	
	{ // create lights
		for (Int i = 0; i < 1; ++i) {
			auto entity = SceneEntity::s_new(AX_NEW, world, nullptr, Fmt("Light_{}", i));
			entity->setPosition(Vec3f(static_cast<f32>(i) * 2.0f, 2, -2));
			entity->addComponent<LightComponent>(AX_NEW);
		}
	}
	
}

} //namespace
