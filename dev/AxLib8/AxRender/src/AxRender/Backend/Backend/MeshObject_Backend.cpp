module;

module AxRender;
import :MeshObject_Backend;
import :RenderRequest_Backend;

namespace ax {

MeshObject_Backend::GpuData* MeshObject_Backend::onGetGpuData(RenderRequest* req) {
	if (meshletInfo.size() <= 0) return nullptr;
		
	meshletVert.buffer->getUploadedGpuBuffer(req);
	meshletPrim.buffer->getUploadedGpuBuffer(req);

	u32  vertOffset  = ax_safe_cast_from(meshletVert.buffer->gpuBufferIndex());
	u32  primOffset  = ax_safe_cast_from(meshletPrim.buffer->gpuBufferIndex());
	auto srcMeshlets = meshletInfo.span();
	auto dstMeshlets = meshlet.editData(0, srcMeshlets.size());
	
	for (Int i = 0; i < srcMeshlets.size(); ++i) {
		auto& dst = dstMeshlets[i];
		dst = srcMeshlets[i];
		dst.draw.vertOffset += vertOffset;
		dst.draw.primOffset += primOffset;
	}
	
	meshlet.buffer->getUploadedGpuBuffer(req);

	_gpuMeshObject.meshletOffset  = ax_safe_cast_from(meshlet.buffer->gpuBufferIndex());
	_gpuMeshObject.meshletCount   = ax_safe_cast_from(meshlet.buffer->count());
	_gpuMeshObject.totalVertCount = ax_safe_cast_from(meshletVert.buffer->count());
	_gpuMeshObject.totalPrimCount = ax_safe_cast_from(meshletPrim.buffer->count());
	return &_gpuMeshObject;
}

} // namespace
