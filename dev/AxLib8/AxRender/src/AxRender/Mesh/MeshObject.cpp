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
	_gpuData = {};
}

auto MeshObject::onGetGpuData(RenderRequest* req) -> const GpuData* {
	_gpuData.boundsMin = _bounds.min;
	_gpuData.boundsMax = _bounds.max;

	_gpuData.meshletGroupCount  = ax_safe_cast_from(meshletGroupBuffer.count());
	_gpuData.meshletCount       = ax_safe_cast_from(meshletBuffer.count());
	
	if (_gpuData.meshletCount <= 0) return nullptr;

	u32  newVertOffset = ax_safe_cast_from(meshletVertBuffer.uploadAndGetOffset(req));
	u32  oldVertOffset = _gpuData.vertOffset;

	u32  newPrimOffset = ax_safe_cast_from(meshletPrimBuffer.uploadAndGetOffset(req));
	u32  oldPrimOffset = _gpuData.primOffset;
	
	for (AxGpuMeshlet& dst : meshletBuffer.editData(0, _gpuData.meshletCount)) {
		dst.vertOffset += newVertOffset - oldVertOffset;
		dst.primOffset += newPrimOffset - oldPrimOffset;
	}
	_gpuData.vertOffset = newVertOffset;
	_gpuData.primOffset = oldVertOffset;

	u32 newMeshletOffset = ax_safe_cast_from(meshletBuffer.uploadAndGetOffset(req));
	u32 oldMeshletOffset = _gpuData.meshletOffset;
	
	for (AxGpuMeshletGroup& dst : meshletGroupBuffer.editData(0, _gpuData.meshletGroupCount)) {
		dst.meshletOffset += newMeshletOffset - oldMeshletOffset;
	}
	_gpuData.meshletOffset = newMeshletOffset;
	
	_gpuData.meshletGroupOffset = ax_safe_cast_from(meshletGroupBuffer.uploadAndGetOffset(req));
	
	return &_gpuData;
}

void MeshObject::createBuffers() {
	_gpuData = {};
	
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	meshletGroupBuffer.create(AX_NEW, "axGpuMeshletGroup", objMgr->_structBufferPools.axMeshletGroup);
	     meshletBuffer.create(AX_NEW, "axGpuMeshlet"     , objMgr->_structBufferPools.axMeshlet);
	 meshletVertBuffer.create(AX_NEW, "axGpuMeshletVert" , objMgr->_structBufferPools.axMeshletVert);
	 meshletPrimBuffer.create(AX_NEW, "axGpuMeshletPrim" , objMgr->_structBufferPools.axMeshletPrim);
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
	_bounds = bounds;

	Array<Vec3d, 64> facePositions;
	for (auto& face : srcMesh.faces()) {
		auto fvCount = static_cast<u32>(face.pointCount());
		if (fvCount < 3) { AX_ASSERT(false); continue; }
		u32 triCount = fvCount - 2;
		
		if (curMeshlet->vertCount + fvCount > kMaxVertexCountPerMeshlet) {
			if (fvCount > kMaxVertexCountPerMeshlet) throw Error_Undefined(Fmt("Face vertex count > kMaxVertexCountPerMeshlet"));
			
			u32 nextVertOffset = curMeshlet->vertCount + curMeshlet->vertOffset;
			u32 nextPrimOffset = curMeshlet->primCount + curMeshlet->primOffset;
			
			curMeshlet = &meshletInfo.emplaceBack();
			*curMeshlet = {};
			curMeshlet->vertOffset = nextVertOffset;
			curMeshlet->primOffset = nextPrimOffset;
		}
		
		auto dstVertices = meshletVertBuffer.extendsData(fvCount);
		auto srcNormals = face.getNormals(srcMesh);
		
		face.getPositions(srcMesh, facePositions);
		for (Int j = 0; j < fvCount; ++j) {
			auto& dstVert = dstVertices[j];
			dstVert.pos      = Vec3f::s_cast(facePositions[j]);
			dstVert.normal   = Vec3f::s_cast(srcNormals[j]);
			dstVert.rawColor = 0xffffffff;
		}

		u32 viBase = curMeshlet->vertCount;
		auto dstIdx  = meshletPrimBuffer.extendsData(triCount);
		for (u32 j = 0; j < triCount; ++j) {
			// triangle fan
			dstIdx[j].tri = u32x3(0, j+1, j+2) + viBase;
		}

		curMeshlet->vertCount += fvCount;
		curMeshlet->primCount += triCount;
	}
	
	meshletBuffer.appendValues(meshletInfo);
	
	AxGpuMeshletGroup meshletGroup = {};
	meshletGroup.meshletOffset = 0;
	meshletGroup.meshletCount  = ax_safe_cast_from(meshletInfo.size());
	meshletGroupBuffer.appendValues(meshletGroup);
	
	objectSlot.markDirty();
}

void MeshObject::create(Span<AxGpuMeshletVert> vertices, Span<u32> indices) {
	createBuffers();
	ClusterGenerator gen;
	gen.nanite(*this, vertices, indices);
	objectSlot.markDirty();
}

} // namespace
