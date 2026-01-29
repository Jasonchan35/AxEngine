module;

module AxRender;
import :MeshObject_Backend;

namespace ax {

void MeshObject_Backend::onUploadToGpu(RenderRequest* req) {
	meshlet->getUploadedGpuBuffer(req);
	meshletVert->getUploadedGpuBuffer(req);
	meshletPrim->getUploadedGpuBuffer(req);
}

} // namespace
