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
	
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/test.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/test2.fbx";
	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/test3.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/McGuire/sportsCar/sportsCar.fbx";
//	String inFilename = "ImportedAssets/JxLocalTemp/Assets/Scenes/test/McGuire/Exterior/exterior.fbx";

	String cacheFolder = Fmt("{}.axWorld", inFilename);
	String cacheRootFile = Fmt("{}/_root.axWorld", cacheFolder);

	SPtr<SceneWorld> world;
	if (enableCache && File::isNewerThan(cacheRootFile, inFilename)) {
		auto cacheWorld = SceneWorld::s_new(AX_NEW);
		if (cacheWorld->readFromFile(cacheFolder)) {
			world = cacheWorld;
		}
	}

	if (!world) {
		AxAssimp assimp;
		world = assimp.openFile(inFilename);
		world->writeToFile(cacheFolder);
	}
	_engine.setWorld(world);
	
	{ // create lights
		for (Int i = 0; i < 10; ++i) {
			auto entity = SceneEntity::s_new(AX_NEW, world, nullptr, Fmt("Light_{}", i));
			constexpr Int row = 3;
			constexpr Vec3f distance(20, 0, -20);
			auto id = Vec3i(i % row, 0, i / row);
			entity->setPosition(Vec3f::s_cast(id) * distance + Vec3f(0,5,0));
			
			auto* comp = entity->addComponent<LightComponent>(AX_NEW);
			
			if (i == 0) {
				comp->lightObj->setRadius(100000);
			} else {
				comp->lightObj->setRadius(20);
			}
			comp->lightObj->setIntensity(0.8f);
		}
	}
		
	{
		auto entity = SceneEntity::s_new(AX_NEW, world, nullptr, Fmt("CullingCamera"));
		auto* comp = entity->addComponent<CameraComponent>(AX_NEW);
		
		comp->cameraObj->camera.nearClip = 0.1f;
		comp->cameraObj->camera.farClip  = 20.0f;
		comp->cameraObj->camera.fieldOfView = 35.0f;
		
		_mainWin->_cullingCameraComp = comp;
	}
	
//	_cloneEntities("sportsCar", 50,  10, Vec3f(5, 0, -6));
//	_cloneEntities("Suzanne",   400, 20, Vec3f(3, 0, -3));
}

void EditorApp::_cloneEntities(InNameId name, Int count, Int row, const Vec3f& distance) {
	auto* world = _engine.world();
	if (!world) return;
	
	auto* srcEntity = world->root()->findChild(name, false);
	if (srcEntity) {
		for (Int i = 1; i < count; ++i) {
			auto entity = SceneEntity::s_new(AX_NEW, world, nullptr, Fmt("clone_{}", i));
			auto id = Vec3i(i % row, 0, i / row);
			entity->setPosition(Vec3f::s_cast(id) * distance);
			entity->setRotation(srcEntity->rotation());

			auto compCount = srcEntity->componentCount();
			for (Int c = 0; c < compCount; ++c) {
				auto* srcComp = srcEntity->componentAt(c);
				
				if (auto* srcMr = rttiCastCheck<MeshRendererComponent>(srcComp)) {
					auto dstMr = entity->addComponent<MeshRendererComponent>(AX_NEW);
					dstMr->mesh = srcMr->mesh;
					dstMr->material = srcMr->material;
				}
			}
			
		}
	}
}

} //namespace
