module;

export module AxEngine:EngineRenderGraph;
export import :Object;
export import :SceneEntity;

export namespace AxEngine {

class EngineRenderGraph : public RenderGraph {
	AX_RTTI_INFO(EngineRenderGraph, RenderGraph)
public:
	EngineRenderGraph();

	virtual void onUpdate(RenderRequest* req) override;
	virtual void onBackBufferPass(RenderRequest* req, Span<Input> inputs) override;
	
	Math::Camera3f& viewportCamera() { return _viewportCamera; }
	
private:
	Math::Camera3f		_viewportCamera;
	SPtr<MeshObject>	_axis;
	RenderMesh			_grid;
	SPtr<Material>		_matSimple3D_Unlit_Color;
};

} // namespace
