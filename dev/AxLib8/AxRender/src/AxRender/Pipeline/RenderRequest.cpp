module AxRender;
import :RenderContext;
import :RenderMesh;
import :RenderRequest_Backend;
import :MeshObject_Backend;

namespace ax /*::AxRender*/ {

void RenderRequest::drawMesh(MeshObject* mesh_, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!mesh_) return;
	
	auto* mesh = rttiCastCheck<MeshObject_Backend>(mesh_);
	
	auto* backend = static_cast<RenderRequest_Backend*>(this);
	backend->resourcesToKeep.add(mesh);

	drawMesh(mesh->meshData, material, materialPass, objectToWorld);
}

void RenderRequest::drawMesh(RenderMesh& mesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	for (auto& sm : mesh.subMeshes()) {
		drawSubMesh(sm, material, materialPass, objectToWorld);
	}
}

void RenderRequest::drawSubMesh(RenderSubMesh& subMesh, Material* material, Int materialPass, const Mat4f& objectToWorld) {
	if (!material) return;

	Cmd_DrawCall cmd;
	cmd.material = material;
	cmd.materialPassIndex = materialPass;
	cmd.objectToWorld = objectToWorld;
	cmd.setSubMesh(this, subMesh);

	drawCall(cmd);
}

void RenderRequest::drawCall(Cmd_DrawCall& cmd) {
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