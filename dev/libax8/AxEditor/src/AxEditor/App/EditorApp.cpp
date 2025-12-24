module;

module AxEditor;

import :EditorApp;
import :EditorMainWindow;
#include "AxEditor-Common.h"

namespace ax::AxEditor {

class EditorApp_CreateDesc : public EditorApp::CreateDesc {
public:
	EditorApp_CreateDesc() {
		peekMessage = true;
//		renderSystemDesc.info.api = RenderAPI::Null;
		renderSystemDesc.info.api = RenderAPI::Vk;
		renderSystemDesc.info.api = RenderAPI::Dx12;

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
	AX_LOG("Set CurrentDir to '{}'", curDir);

	_watcher.dgResults.bindUnowned(this, &This::_onFileChanged);
	_watcher.create(curDir);

	_mainWin = UPtr_new<EditorMainWindow>(AX_ALLOC_REQ);
}

void EditorApp::onPeekMessage() {
	if (_mainWin) _mainWin->render();
}

void EditorApp::_onFileChanged(FileDirWatcher_Result& result) {
	if (auto* renderSystem = RenderSystem::s_instance()) {
		renderSystem->onFileChanged(result);
	}
}

} //namespace
