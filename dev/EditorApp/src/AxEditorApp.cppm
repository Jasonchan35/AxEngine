module;

export module AxEditor.EditorApp;
export import AxNativeUI;

export namespace ax::AxEditor {

class EditorWindow : public AxUI::UIWindow {
	AX_RTTI_INFO(EditorWindow, AxUI::UIWindow) {};

public:
};

class EditorMainWindow : public EditorWindow {
	AX_RTTI_INFO(EditorMainWindow, EditorWindow) {};
public:
	EditorMainWindow();

protected:
	virtual void onWindowCloseButton() override;


	struct MyRenderGraph : public AxRender::DefaultRenderGraph {
		class EditorMainWindow* win = nullptr;
	};

	SPtr<MyRenderGraph>	_renderGraph;
};

class EditorApp : public NativeUIApp {
public:
	virtual int onRun() override {
		return 0;
	}

	virtual void onCreate() override {
		_mainWin = UPtr_new<EditorMainWindow>(AX_ALLOC_REQ);
	}
	
};

} // namespace
