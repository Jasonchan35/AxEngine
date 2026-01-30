module AxRender;
import :MeshObject_Backend;
import :RenderObjectManager_Backend;

namespace ax {

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<MeshObject_Backend> o;
	RenderObjectManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.assetPath);
	return o;	
}

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req) {
	MeshObject_CreateDesc desc;
	return s_new(req, desc);
}

MeshObject::MeshObject(const CreateDesc& desc)
: _assetPath(desc.assetPath) {
	auto* objMgr = RenderObjectManager_Backend::s_instance();

	    meshlet.create(AX_NEW, "meshlet"    , objMgr->_structBufferPools.axMeshlet);
	meshletVert.create(AX_NEW, "meshletVert", objMgr->_structBufferPools.axMeshletVert);
	meshletPrim.create(AX_NEW, "meshletPrim", objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject::_doUploadToGpu(RenderRequest* req) {
	meshletVert->getUploadedGpuBuffer(req);
	meshletPrim->getUploadedGpuBuffer(req);
	
	AxMeshlet m;
	m.vertCount  = ax_safe_cast_from(meshletVert->count());
	m.vertOffset = ax_safe_cast_from(meshletVert->gpuBufferIndex());
	m.primCount  = ax_safe_cast_from(meshletPrim->count());
	m.primOffset = ax_safe_cast_from(meshletPrim->gpuBufferIndex());
	meshlet->setValue(0, m);
	meshlet->getUploadedGpuBuffer(req);	
}


} // namespace
