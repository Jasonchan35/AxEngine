module;

module AxRender;
import :MeshObject;
import :RenderObjectManager_Backend;
import :EditableMesh;
import :ClusterGenerator;

namespace ax {

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<MeshObject_Backend> o;
	RenderObjectManager_Backend::s_instance()->getOrNewObject(o, req, desc, desc.assetPath);
	return o;
}

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req) {
	MeshObject_CreateDesc desc;
	return s_new(req, desc);
}

MeshObject::MeshObject(const CreateDesc& desc)
	: _assetPath(desc.assetPath)
	, objectSlot(this) 
{
}

auto MeshObject_Meshlet::onGetGpuData(MeshObject* meshObj, RenderRequest* req) -> const GpuData* {
	_gpuData.boundsMin = meshObj->_bounds.min;
	_gpuData.boundsMax = meshObj->_bounds.max;

	_gpuData.meshletGroupCount   =  ax_safe_cast_from(groupBuffer.count());
	_gpuData.meshletClusterCount = ax_safe_cast_from(clusterBuffer.count());
	
	if (_gpuData.meshletClusterCount <= 0) return nullptr;

	u32  newVertOffset = ax_safe_cast_from(vertBuffer.uploadAndGetOffset(req));
	u32  oldVertOffset = _gpuData.vertOffset;

	u32  newPrimOffset = ax_safe_cast_from(primBuffer.uploadAndGetOffset(req));
	u32  oldPrimOffset = _gpuData.primOffset;
	
	for (AxGpuMeshletCluster& dst : clusterBuffer.editData(0, _gpuData.meshletClusterCount)) {
		dst.vertOffset += newVertOffset - oldVertOffset;
		dst.primOffset += newPrimOffset - oldPrimOffset;
	}
	_gpuData.vertOffset = newVertOffset;
	_gpuData.primOffset = oldVertOffset;

	u32 newMeshletOffset = ax_safe_cast_from(clusterBuffer.uploadAndGetOffset(req));
	u32 oldMeshletOffset = _gpuData.meshletClusterOffset;
	
	for (AxGpuMeshletGroup& dst : groupBuffer.editData(0, _gpuData.meshletGroupCount)) {
		dst.clusterOffset += newMeshletOffset - oldMeshletOffset;
	}
	_gpuData.meshletClusterOffset = newMeshletOffset;
	
	_gpuData.meshletGroupOffset = ax_safe_cast_from(groupBuffer.uploadAndGetOffset(req));
	
	return &_gpuData;
}

void MeshObject_Meshlet::createBuffers() {
	_gpuData = {};
	
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	  groupBuffer.create(AX_NEW, "axGpuMeshletGroup"  , objMgr->_structBufferPools.axMeshletGroup);
	clusterBuffer.create(AX_NEW, "axGpuMeshletCluster", objMgr->_structBufferPools.axMeshletCluster);
	   vertBuffer.create(AX_NEW, "axGpuMeshletVert"   , objMgr->_structBufferPools.axMeshletVert);
	   primBuffer.create(AX_NEW, "axGpuMeshletPrim"   , objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject_Meshlet::writeToFile(MeshObject* meshObj, StrView filename) {
	FileStream fs;
	fs.openWrite(filename, true);
	fs.writeBytes(Span(meshObj->_bounds).toByteSpan());
	
	groupBuffer.writeToFile(fs);
	clusterBuffer.writeToFile(fs);
	vertBuffer.writeToFile(fs);
	primBuffer.writeToFile(fs);
}

void MeshObject_Meshlet::readFromFile(MeshObject* meshObj, StrView filename) {
	createBuffers();
	
	FileStream fs;
	fs.openRead(filename);
	
	BBox3f bounds;
	fs.readBytes(MutSpan(bounds).toMutByteSpan());
	meshObj->setBounds(bounds);
	
	groupBuffer.readFromFile(fs);
	clusterBuffer.readFromFile(fs);
	vertBuffer.readFromFile(fs);
	primBuffer.readFromFile(fs);
}

void MeshObject::createMeshlet(Span<AxGpuMeshletVert> vertices, Span<u32> indices) {
	meshlet.createBuffers();
	ClusterGenerator gen;
	gen.nanite(*this, vertices, indices);
	objectSlot.markDirty();
}

} // namespace
