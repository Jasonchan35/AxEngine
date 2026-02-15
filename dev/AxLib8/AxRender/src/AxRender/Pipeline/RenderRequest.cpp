module AxRender;
import :RenderContext;
import :RenderMesh;
import :RenderRequest_Backend;
import :MeshObject;

namespace ax /*::AxRender*/ {

RenderRequest::RenderRequest() 
: _worldData({})
, _cameraData({})
, _debugData({}) 
{}

void RenderRequest::drawMesh(MeshObject* mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;
	if (!mesh) return;

	drawMesh(mesh->renderMesh, material, materialPass, objectToWorld);
	
	u32 clusterCount = ax_safe_cast_from(mesh->meshlet.clusterBuffer.count());
	if (clusterCount <= 0) return;

	auto* stockObjs = RenderStockObjects::s_instance();
	AxMeshShaderDraw draw;
	draw.material = stockObjs->materials->meshlet;
	draw.materialPassIndex = materialPass;
	draw.objectToWorld = objectToWorld * Mat4f::s_translate(0, 3, 0);
	draw.meshObject = mesh;
	
	u32 dispatchGroupCount  = Math::alignDivTo(clusterCount, AX_HLSL_THREADS_PER_WAVE);
	draw.dispatchGroupCount = u32x3(dispatchGroupCount, 1, 1);
	draw.lodGroupId         = 0;
	
	meshShaderDraw(draw);
	
	if (_debugData.drawNormalLength > 0) {
		draw.material = stockObjs->materials->meshlet_normal;
		meshShaderDraw(draw);
	}
}

void RenderRequest::drawMesh(RenderMesh& mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	for (auto& sm : mesh.subMeshes()) {
		drawSubMesh(sm, material, materialPass, objectToWorld);
	}
}

void RenderRequest::drawSubMesh(RenderSubMesh& subMesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;

	AxVertexShaderDraw draw;
	draw.material = material;
	draw.materialPassIndex = materialPass;
	draw.objectToWorld = objectToWorld;
	draw.setSubMesh(this, subMesh);
	vertexShaderDraw(draw);
}

void RenderRequest::drawWorld() {
	static_cast<RenderRequest_Backend*>(this)->drawWorld_backend();
}

void RenderRequest::meshShaderDraw(AxMeshShaderDraw& draw) {
	static_cast<RenderRequest_Backend*>(this)->meshShaderDraw_backend(draw);
}

void RenderRequest::vertexShaderDraw(AxVertexShaderDraw& draw) {
	static_cast<RenderRequest_Backend*>(this)->vertexShaderDraw_backend(draw);
}

void RenderRequest::setViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	static_cast<RenderRequest_Backend*>(this)->setViewport_backend(rect, minDepth, maxDepth);
}

void RenderRequest::setScissorRect(const Rect2f& rect) {
	static_cast<RenderRequest_Backend*>(this)->setScissorRect_backend(rect);
}

void RenderRequest::setCamera(const Math::Camera3f& camera) {
	static_cast<RenderRequest_Backend*>(this)->setCamera_backend(camera);
}

void RenderRequest::setDebugData(const AxGpuData_Debug& debugData) {
	static_cast<RenderRequest_Backend*>(this)->setDebugData_backend(debugData);
}


} // namespace