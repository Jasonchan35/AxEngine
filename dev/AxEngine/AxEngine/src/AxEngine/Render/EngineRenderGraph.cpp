module AxEngine;
import :EngineRenderGraph;

namespace AxEngine {

EngineRenderGraph::EngineRenderGraph() {
	_viewportCamera.setRotation(30, 150);
//	_viewportCamera.setRotation(0, 0);
	_viewportCamera.distance = 20;
	
	auto* stockObjs = RenderStockObjects::s_instance();
	
	RenderMeshEdit(_grid).createGrid(Vertex_PosColor::s_layout(), Vec3f(1,0,0), Vec3f(0,0,1), 1, 50);
	_matSimple3D_Unlit_Color = stockObjs->materials->Simple3D_Unlit_Color;
	_axis = stockObjs->meshes->Axis;
}

void EngineRenderGraph::onUpdate(RenderRequest* req) {
	_viewportCamera.viewport = Rect2f(Vec2f(0,0), Vec2f::s_cast(req->frameSize()));
	req->setCamera(_viewportCamera, _viewportCamera.worldMatrix(req->projectionDesc()));
}


void EngineRenderGraph::test_createAxis() {
	_testMesh.clear();
	
	auto edit = _testMesh.editNewVertices(
		RenderPrimitiveType::Lines, 
		Vertex_PosColor::s_layout(), 
		VertexIndexType::None, 6);
	
	if (auto enumerator = edit.tryEditPosition()) {
		auto dstPos = enumerator->begin();
		*dstPos = Vec3f(0,0,0); ++dstPos;
		*dstPos = _testRot * Vec3f(1,0,0); ++dstPos;
		
		*dstPos = Vec3f(0,0,0); ++dstPos;
		*dstPos = _testRot * Vec3f(0,1,0); ++dstPos;
		
		*dstPos = Vec3f(0,0,0); ++dstPos;
		*dstPos = _testRot * Vec3f(0,0,1); ++dstPos;
		
		if (dstPos != enumerator->end()) throw Error_Undefined();
	}

	if (auto enumerator = edit.tryEditColor0()) {
		auto dstCol = enumerator->begin();
		*dstCol = Color4b::kRed();   ++dstCol;
		*dstCol = Color4b::kRed();   ++dstCol;

		*dstCol = Color4b::kGreen(); ++dstCol;
		*dstCol = Color4b::kGreen(); ++dstCol;

		*dstCol = Color4b::kBlue();  ++dstCol;
		*dstCol = Color4b::kBlue();  ++dstCol;
		
		if (dstCol != enumerator->end()) throw Error_Undefined(); 
	}
}


void EngineRenderGraph::onBackBufferPass(RenderRequest* req, Span<Input> inputs) {
	Base::onBackBufferPass(req, inputs);
	req->drawMesh(_grid, _matSimple3D_Unlit_Color, 0);
	req->drawMesh(_axis->renderMesh, _matSimple3D_Unlit_Color, 0, Mat4f::s_translate(0, 0.01f, 0));
	
	test_createAxis();
	req->drawMesh(_testMesh, _matSimple3D_Unlit_Color, 0, Mat4f::s_translate(0, 2, 0));
	
	if (auto* world = Engine::s_instance()->world()) {
		world->onRender(req);
	}
}

} // namespace
