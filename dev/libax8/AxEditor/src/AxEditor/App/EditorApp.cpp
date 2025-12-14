module;

module AxEditor;

import :EditorApp;
import :EditorMainWindow;

namespace ax::AxEditor {

class EditorApp_CreateDesc : public EditorApp::CreateDesc {
public:
	EditorApp_CreateDesc() {
		peekMessage = true;
//		rendererDesc.info.api = RenderAPI::Null;
		rendererDesc.info.api = RenderAPI::VK;
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
	if (auto* renderer = Renderer::s_instance()) {
		renderer->onFileChanged(result);
	}
}

} //namespace
