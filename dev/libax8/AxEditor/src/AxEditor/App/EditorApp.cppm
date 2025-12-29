module;

export module AxEditor:EditorApp;
export import :Common;

export namespace AxEditor {

class EditorMainWindow;
class EditorApp : public AxUI::UIApp {
	AX_RTTI_INFO(EditorApp, AxUI::UIApp)
public:
	AX_DOWNCAST_GET_INSTANCE()

	EditorApp();
	virtual ~EditorApp() override;

	virtual void onCreate() override;

private:
	virtual void onPeekMessage() override;

	void _onFileChanged(FileDirWatcher_Result& result);
	void _createDemoScene();

	AxEngine::Engine		_engine;
	UPtr<EditorMainWindow>	_mainWin;

	FileDirWatcher	_watcher;
};

} //namespace
