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
	
	Math::BoomCamera3f& viewportCamera() { return _viewportCamera; }
	
private:
	Math::BoomCamera3f	_viewportCamera;
	SPtr<MeshObject>	_axis;
	RenderMesh			_grid;
	SPtr<Material>		_matSimple3D_Unlit_Color;
};

} // namespace
