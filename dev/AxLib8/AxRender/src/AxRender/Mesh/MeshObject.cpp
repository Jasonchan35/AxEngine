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
	_meshObjectInfo.bounds = BBox3f::s_empty();
}

auto MeshObject::onGetGpuData(RenderRequest* req) -> const GpuData* {
	_meshObjectInfo.lodGroupCount = ax_safe_cast_from(meshLodGroup.count());
	_meshObjectInfo.meshletCount  = ax_safe_cast_from(meshlet.count());
	
	if (_meshObjectInfo.lodGroupCount <= 0) return nullptr;

	 meshletVert.buffer->getUploadedGpuBuffer(req);
	 meshletPrim.buffer->getUploadedGpuBuffer(req);

	u32  newVertOffset = ax_safe_cast_from(meshletVert.buffer->gpuBufferIndex());
	u32  oldVertOffset = _meshObjectInfo.vertOffset;

	u32  newPrimOffset = ax_safe_cast_from(meshletPrim.buffer->gpuBufferIndex());
	u32  oldPrimOffset = _meshObjectInfo.primOffset;
	
	for (AxGpuMeshlet& dst : meshlet.editData(0, _meshObjectInfo.meshletCount)) {
		dst.draw.vertOffset += newVertOffset - oldVertOffset;
		dst.draw.primOffset += newPrimOffset - oldPrimOffset;
	}
	_meshObjectInfo.vertOffset = newVertOffset;
	_meshObjectInfo.primOffset = oldVertOffset;
	
	meshlet.buffer->getUploadedGpuBuffer(req);
	u32 newMeshletOffset = ax_safe_cast_from(meshlet.buffer->gpuBufferIndex());
	u32 oldMeshletOffset = _meshObjectInfo.meshletOffset;
	
	for (AxGpuMeshLodGroup& dst : meshLodGroup.editData(0, _meshObjectInfo.lodGroupCount)) {
		dst.meshletOffset += newMeshletOffset - oldMeshletOffset;
	}
	_meshObjectInfo.meshletOffset = newMeshletOffset;
	meshLodGroup.buffer->getUploadedGpuBuffer(req);
	
	return &_meshObjectInfo;
}

void MeshObject::createBuffers() {
	_meshObjectInfo = {};
	
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	meshLodGroup.create(AX_NEW, "axGpuMeshLodGroup", objMgr->_structBufferPools.axMeshLodGroup);
	     meshlet.create(AX_NEW, "axGpuMeshlet"     , objMgr->_structBufferPools.axMeshlet);
	 meshletVert.create(AX_NEW, "axGpuMeshletVert" , objMgr->_structBufferPools.axMeshletVert);
	 meshletPrim.create(AX_NEW, "axGpuMeshletPrim" , objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject::createFromEditableMesh(const EditableMesh& srcMesh) {
	createBuffers();
	constexpr Int kMaxVertexCountPerMeshlet = AX_HLSL_MESH_SHADER_MAX_VERT_COUNT;
	
	Array<AxGpuMeshlet> meshletInfo;
	
	meshletInfo.clear();
	auto* curMeshlet = &meshletInfo.emplaceBack();
	*curMeshlet = {};
	
	BBox3f bounds = BBox3f::s_empty();
	for (auto& pt : srcMesh.points()) {
		bounds.includePoint(Vec3f::s_cast(pt.pos));
	}
	_meshObjectInfo.bounds = bounds;

	Array<Vec3d, 64> facePositions;
	for (auto& face : srcMesh.faces()) {
		auto fvCount = static_cast<u32>(face.pointCount());
		if (fvCount < 3) { AX_ASSERT(false); continue; }
		u32 triCount = fvCount - 2;
		
		if (curMeshlet->draw.vertCount + fvCount > kMaxVertexCountPerMeshlet) {
			if (fvCount > kMaxVertexCountPerMeshlet) throw Error_Undefined(Fmt("Face vertex count > kMaxVertexCountPerMeshlet"));
			
			u32 nextVertOffset = curMeshlet->draw.vertCount + curMeshlet->draw.vertOffset;
			u32 nextPrimOffset = curMeshlet->draw.primCount + curMeshlet->draw.primOffset;
			
			curMeshlet = &meshletInfo.emplaceBack();
			*curMeshlet = {};
			curMeshlet->draw.vertOffset = nextVertOffset;
			curMeshlet->draw.primOffset = nextPrimOffset;
		}
		
		auto dstVertices = meshletVert.extendsData(fvCount);
		auto srcNormals = face.getNormals(srcMesh);
		
		face.getPositions(srcMesh, facePositions);
		for (Int j = 0; j < fvCount; ++j) {
			auto& dstVert = dstVertices[j];
			dstVert.pos      = Vec3f::s_cast(facePositions[j]);
			dstVert.normal   = Vec3f::s_cast(srcNormals[j]);
			dstVert.rawColor = 0xffffffff;
		}

		u32 viBase = curMeshlet->draw.vertCount;
		auto dstIdx  = meshletPrim.extendsData(triCount);
		for (u32 j = 0; j < triCount; ++j) {
			// triangle fan
			dstIdx[j].tri = u32x3(0, j+1, j+2) + viBase;
		}

		curMeshlet->draw.vertCount += fvCount;
		curMeshlet->draw.primCount += triCount;
	}
	
	meshlet.appendValues(meshletInfo);
	
	AxGpuMeshLodGroup lodGroup = {};
	lodGroup.meshletOffset = 0;
	lodGroup.meshletCount  = ax_safe_cast_from(meshletInfo.size());
	meshLodGroup.appendValues(lodGroup);
	
	objectSlot.markDirty();
}

void MeshObject::create(Span<AxGpuMeshletVert> vertices, Span<u32> indices) {
	createBuffers();
	ClusterGenerator gen;
	gen.nanite(*this, vertices, indices);
	objectSlot.markDirty();
}

} // namespace
