module AxEngine;
import :EngineRenderGraph;

namespace AxEngine {

EngineRenderGraph::EngineRenderGraph() {
	auto* stockObjs = RenderStockObjects::s_instance();
	
	RenderMeshEdit(_grid).createGrid(RenderPlaneAxis::ZX, Vertex_PosColor::s_layout(), 1, 50);
	_mat_simple3d_color = Material::s_new(AX_NEW, "ImportedAssets/Shaders/core/simple3d_color.axShader");
	_axis = stockObjs->meshes->axis;
}

void EngineRenderGraph::onUpdate(RenderRequest* req) {
	req->setCamera(_viewportCamera);
}

void EngineRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	req->drawMesh(_grid, _mat_simple3d_color, 0);
	req->drawMesh(_axis, _mat_simple3d_color, 0);
}

} // namespace
