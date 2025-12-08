module;

export module AxEditor.EditorApp;
export import AxUI;

export namespace ax::AxEditor {

class EditorWindow : public AxUI::UIWindow {
	AX_RTTI_INFO(EditorWindow, AxUI::UIWindow)

public:
};

class EditorMainWindow : public EditorWindow {
	AX_RTTI_INFO(EditorMainWindow, EditorWindow)
public:
	EditorMainWindow();

protected:
	virtual void onWindowCloseButton() override;


	struct MyRenderGraph : public AxRender::DefaultRenderGraph {
		class EditorMainWindow* win = nullptr;
	};

	SPtr<MyRenderGraph>	_renderGraph;
};

struct EditorApp_CreateDesc : public AxUI::UIApp_CreateDesc {
	EditorApp_CreateDesc() {
		rendererDesc.info.api = AxRender::RenderApi::Null; 
	}
};

class EditorApp : public AxUI::UIApp {
	AX_RTTI_INFO(EditorApp, AxUI::UIApp)
public:
	using CreateDesc = EditorApp_CreateDesc;
	
	EditorApp() : Base(CreateDesc()) {}
	
	// virtual int onRun() override {
	// 	return 0;
	// }

	virtual void onCreate() override {
		Base::onCreate();
		_mainWin = UPtr_new<EditorMainWindow>(AX_ALLOC_REQ);
	}

	UPtr<EditorMainWindow>	_mainWin;
};

} // namespace
