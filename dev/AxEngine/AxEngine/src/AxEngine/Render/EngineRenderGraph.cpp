module AxEngine;
import :EngineRenderGraph;

namespace AxEngine {

EngineRenderGraph::EngineRenderGraph() {
	_viewportCamera.setRotation(30, -150);
//	_viewportCamera.setRotation(0, 0);
	_viewportCamera.distance = 20;
	
	auto* stockObjs = RenderStockObjects::s_instance();
	
	RenderMeshEdit(_grid).createGrid(Vertex_PosColor::s_layout(), Vec3f(1,0,0), Vec3f(0,0,1), 1, 50);
	_matSimple3D_Unlit_Color = stockObjs->materials->Simple3D_Unlit_Color;
	_axis = stockObjs->meshes->Axis;
}

void EngineRenderGraph::onUpdate(RenderRequest* req) {
	_viewportCamera.viewport = Rect2f(Vec2f(0,0), Vec2f::s_cast(req->frameSize()));
	req->setCamera(_viewportCamera);
}

void EngineRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	req->drawMesh(_grid, _matSimple3D_Unlit_Color, 0);
	req->drawMesh(_axis->renderMesh, _matSimple3D_Unlit_Color, 0, Mat4f::s_translate(0, 0.01f, 0));
	if (auto* world = Engine::s_instance()->world()) {
		if (auto* meshRenderer = world->meshRendererSystem()) {
			meshRenderer->onRender(req);
		}
	}
}

} // namespace
