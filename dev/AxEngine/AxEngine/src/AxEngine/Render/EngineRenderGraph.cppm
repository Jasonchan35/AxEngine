module;

export module AxEngine:EngineRenderGraph;
export import :Object;

export namespace AxEngine {

class EngineRenderGraph : public RenderGraph {
	AX_RTTI_INFO(EngineRenderGraph, RenderGraph)
public:

	virtual void onUpdate(RenderRequest* req) override;
	virtual void onBackBufferPass(RenderRequest* req, Span<Input> inputs) override;
	
};

} // namespace
