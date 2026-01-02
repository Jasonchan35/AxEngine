module;

export module AxRender:DefaultRenderGraph;
export import :RenderMesh;
export import :Material;
export import :RenderGraph;

export namespace ax /*::AxRender*/ {

class DefaultRenderGraph : public RenderGraph {
	AX_RTTI_INFO(DefaultRenderGraph, RenderGraph)
public:

	struct GBufferPass : public Pass {
		GBufferPass(This* graph) : Pass(graph, "gbuffer", &This::onGBufferPass) {}

		ColorBuffer color0 {this, "color0", ColorType::RGBAb};
		ColorBuffer color1 {this, "color1", ColorType::RGBAb};
	} gbuffer {this};

	struct LightPass : public Pass {
		LightPass(This* graph) : Pass(graph, "LightPass", &This::onLightingPass) {}

		ColorBuffer color0 {this, "color0", ColorType::RGBAf};
	} lighting {this};

	DefaultRenderGraph();

	virtual void onUpdate(RenderRequest* req) override;

	virtual void onBackBufferPass	(RenderRequest* req, Span<Input> inputs) override;
	virtual void onGBufferPass		(RenderRequest* req, Span<Input> inputs);
	virtual void onLightingPass		(RenderRequest* req, Span<Input> inputs);

private:
	SPtr<Material>	_testMaterial;
	SPtr<Material>	_testMeshMaterial;
	RenderMesh		_testMesh;
	SPtr<Texture2D>	_testTex0;
	SPtr<Texture2D> _testTex1;
	SPtr<Sampler>	_testSampler;
	
	RenderMesh		_testCube;
	Math::Camera3f	_camera;
};

} // namespace
