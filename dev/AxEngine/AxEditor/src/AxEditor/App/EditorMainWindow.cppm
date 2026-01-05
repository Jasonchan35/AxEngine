module;

export module AxEditor:EditorMainWindow;
export import :EditorWindow;
export import :SceneOutlinerUIPanel;

export namespace AxEditor {

template<class T>
class Rotator_ {
	
};

class EditorMainWindow : public EditorWindow {
	AX_RTTI_INFO(EditorMainWindow, EditorWindow)
public:
	EditorMainWindow();

protected:
	virtual void onWindowCloseButton() override;
	virtual	void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual	void onUIKeyEvent(UIKeyEvent& ev) override;

	struct MyRenderGraph : public DefaultRenderGraph {
		class EditorMainWindow* win = nullptr;
		
		virtual void onBackBufferPass(RenderRequest* req, Span<Input> inputs) override;
	};

	SceneOutlinerUIPanel _sceneOutlinerUIPanel;
	
	SPtr<MyRenderGraph>	_renderGraph;
};

} //namespace
