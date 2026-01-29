module;

module AxRender;
import :MeshObject_Backend;

namespace ax {

void MeshObject_Backend::onUploadToGpu(RenderRequest* req) {
	meshletVert->getUploadedGpuBuffer(req);
	meshletPrim->getUploadedGpuBuffer(req);
	
	Meshlet m;
	m.vertCount  = ax_safe_cast_from(meshletVert->count());
	m.vertOffset = ax_safe_cast_from(meshletVert->gpuBufferIndex());
	m.primCount  = ax_safe_cast_from(meshletPrim->count());
	m.primOffset = ax_safe_cast_from(meshletPrim->gpuBufferIndex());
	meshlet->setValue(0, m);
	meshlet->getUploadedGpuBuffer(req);
}

} // namespace
