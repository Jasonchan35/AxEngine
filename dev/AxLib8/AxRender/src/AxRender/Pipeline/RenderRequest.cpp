module AxRender;
import :RenderContext;
import :RenderMesh;
import :RenderRequest_Backend;
import :MeshObject_Backend;

namespace ax /*::AxRender*/ {

void RenderRequest::drawMesh(MeshObject* mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;
	if (!mesh) return;

	drawMesh(mesh->meshData, material, materialPass, objectToWorld);
#if 1
	if (!mesh->isMeshletValid()) return;

	AxDrawCallDesc desc;
	// desc.material = material;
	desc.material = RenderStockObjects::s_instance()->materials->meshlet;
	
	desc.materialPassIndex = materialPass;
	desc.objectToWorld = objectToWorld * Mat4f::s_translate(0, 3, 0);
	desc.meshObject = mesh;

	drawCall(desc);
#endif
}

void RenderRequest::drawMesh(RenderMesh& mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	for (auto& sm : mesh.subMeshes()) {
		drawSubMesh(sm, material, materialPass, objectToWorld);
	}
}

void RenderRequest::drawSubMesh(RenderSubMesh& subMesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;

	AxDrawCallDesc desc;
	desc.material = material;
	desc.materialPassIndex = materialPass;
	desc.objectToWorld = objectToWorld;
	desc.setSubMesh(this, subMesh);
	drawCall(desc);
}

void RenderRequest::drawCall(AxDrawCallDesc& cmd) {
	static_cast<RenderRequest_Backend*>(this)->drawCall_backend(cmd);
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