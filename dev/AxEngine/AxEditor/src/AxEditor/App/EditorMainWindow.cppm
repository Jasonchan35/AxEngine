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

		virtual void onUpdate(RenderRequest* req) override;
		virtual void onBackBufferPass(RenderRequest* req, Span<Input> inputs) override;
		
	};

	SceneOutlinerUIPanel _sceneOutlinerUIPanel;
	InspectorUIPanel _inspectorUIPanel;
	
	SPtr<MyRenderGraph>	_renderGraph;

	void _viewportCameraPanel(RenderRequest* req);
	void _drawGizmo(RenderRequest* req);
	void _statisticsPanel(RenderRequest* req);
	void _updateCullingCamera(RenderRequest* req);
	
	enum class OpMode { None, Translate, Rotate, Scale };
	enum class OpSpace { Local, World };

	Vec3f _translateSnap = Vec3f::s_all(1);
	Vec3f _rotateSnap = Vec3f::s_all(5);
	Vec3f _scaleSnap  = Vec3f::s_all(0.1f);
	
	bool _enableTranslateSnap = false;
	bool _enableRotateSnap = false;
	bool _enableScaleSnap = false;
	
	enum class InputMode {
		None,
		Camera,
		FlyingCamera,
	};
	
	InputMode _inputMode = InputMode::None;
	Vec3f _flyingCameraMoveVector = Vec3f::s_zero();
	float _flyingCameraSpeed = 5.0f;
	
	ImUIGizmoOperation	_gizmoOp    = ImUIGizmoOperation::None;
	ImUIGizmoSpace		_gizmoSpace = ImUIGizmoSpace::World;
	bool _gizmoIsUsing = false;
	Mat4f _gizmoStartWorldMatrix;
	
	float _maxMeshletErrorInPixels = 2.0f;
	float _mouseSpeed = 1.0;
	AxGpuData_Debug _gpuDebugData;
	bool _useCullingCamera = false;
	bool _viewFromCullingCamera = false;
};


} //namespace
