module AxRender;
import :RenderContext;
import :RenderMesh;
import :RenderRequest_Backend;
import :MeshObject;

namespace ax /*::AxRender*/ {

void RenderRequest::drawMeshRenderer(MeshObjectRenderer* mr) {
	static_cast<RenderRequest_Backend*>(this)->drawMeshRenderer_backend(mr);
}

void RenderRequest::drawMesh(MeshObject* mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;
	if (!mesh) return;
	drawMesh(mesh->meshData, material, materialPass, objectToWorld);
	
#if 1
	if (!mesh->isMeshletValid()) return;

	AxMeshShaderDraw draw;
	// desc.material = material;
	draw.material = RenderStockObjects::s_instance()->materials->meshlet;
	
	draw.materialPassIndex = materialPass;
	draw.objectToWorld = objectToWorld * Mat4f::s_translate(0, 3, 0);
	draw.meshObject = mesh;
	
	u32 meshletCount = ax_safe_cast_from(draw.meshObject->meshletInfo.size());
	u32 groupCount = Math::alignDivTo(meshletCount, AX_HLSL_THREADS_PER_WAVE);
	draw.groupCount = u32x3(groupCount, 1, 1);
	
	meshShaderDraw(draw);
#endif
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

void RenderRequest::indirectMeshShader() {
	static_cast<RenderRequest_Backend*>(this)->indirectMeshShader_backend();
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


} // namespace