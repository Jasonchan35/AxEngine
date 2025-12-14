module;

export module AxEditor:EditorMainWindow;
export import :EditorWindow;

export namespace ax::AxEditor {

class EditorMainWindow : public EditorWindow {
	AX_RTTI_INFO(EditorMainWindow, EditorWindow)
public:
	EditorMainWindow();

protected:
	virtual void onWindowCloseButton() override;


	struct MyRenderGraph : public DefaultRenderGraph {
		class EditorMainWindow* win = nullptr;
	};

	SPtr<MyRenderGraph>	_renderGraph;
};

} //namespace
