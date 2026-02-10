module;

export module AxEditor:EditorMainWindow;
export import :EditorWindow;
export import :SceneOutlinerUIPanel;
export import :InspectorUIPanel;

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

	struct MyRenderGraph : public EngineRenderGraph {
		AX_RTTI_INFO(MyRenderGraph, EngineRenderGraph)
	public:
		class EditorMainWindow* _owner = nullptr;
		
		virtual void onBackBufferPass(RenderRequest* req, Span<Input> inputs) override;
		
	};

	SceneOutlinerUIPanel _sceneOutlinerUIPanel;
	InspectorUIPanel _inspectorUIPanel;
	
	SPtr<MyRenderGraph>	_renderGraph;
	SceneWorld	_sceneWorld;

	void _cameraDebugPanel(RenderRequest* req);
	void _drawGizmo(RenderRequest* req);
	
	enum class OpMode { None, Translate, Rotate, Scale };
	enum class OpSpace { Local, World };

	Vec3f _translateSnap = Vec3f::s_all(1);
	Vec3f _rotateSnap = Vec3f::s_all(5);
	Vec3f _scaleSnap  = Vec3f::s_all(0.1f);
	
	bool _enableTranslateSnap = false;
	bool _enableRotateSnap = false;
	bool _enableScaleSnap = false;
	
	ImUIGizmoOperation	_opMode  = ImUIGizmoOperation::Universal;
	ImUIGizmoSpace		_opSpace = ImUIGizmoSpace::Local;
	bool _gizmoIsUsing = false;
	Mat4f _gizmoStartWorldMatrix;
	
	Int _lodBias = 0;
};

} //namespace
