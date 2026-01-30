module AxRender;
import :MeshObject_Backend;
import :RenderObjectManager_Backend;
import :EditableMesh;

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
}

void MeshObject::Buffers::create() {
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	   meshInfo.create(AX_NEW, "meshInfo"   , objMgr->_structBufferPools.axMeshInfo);
	    meshlet.create(AX_NEW, "meshlet"    , objMgr->_structBufferPools.axMeshlet);
	meshletVert.create(AX_NEW, "meshletVert", objMgr->_structBufferPools.axMeshletVert);
	meshletPrim.create(AX_NEW, "meshletPrim", objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject::createFromEditableMesh(const EditableMesh& srcMesh) {
	buffers.create();
	constexpr Int kMaxVertexCountPerMeshlet = 100; // 32 * 1024;
	
	meshletInfo.clear();
	auto* curMeshlet = &meshletInfo.emplaceBack();

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
			curMeshlet->vertOffset = nextVertOffset;
			curMeshlet->primOffset = nextPrimOffset;
		}
		
		auto dstVert = buffers.meshletVert.extendsData(fvCount);
		
		face.getPositions(srcMesh, facePositions);
		for (Int j = 0; j < fvCount; ++j) {
			dstVert[j].pos = Vec3f::s_cast(facePositions[j]);
		}

		u32 vi = curMeshlet->vertOffset + curMeshlet->vertCount;
		auto dstIdx  = buffers.meshletPrim.extendsData(triCount);
		for (u32 j = 0; j < triCount; ++j) {
			dstIdx[j].tri.set(vi, vi + j + 1, vi + j + 2);
		}

		curMeshlet->vertCount += fvCount;
		curMeshlet->primCount += triCount;
	}
}

void MeshObject::Buffers::_uploadToGpu(MeshObject* meshObj, RenderRequest* req) {
	if (!meshObj->isMeshletValid()) return;
	
	meshletVert.buffer->getUploadedGpuBuffer(req);
	meshletPrim.buffer->getUploadedGpuBuffer(req);

	AxMeshInfo outInfo;
	
	u32 vertOffset    = ax_safe_cast_from(meshletVert.buffer->gpuBufferIndex());
	u32 primOffset    = ax_safe_cast_from(meshletPrim.buffer->gpuBufferIndex());

	auto srcMeshlets = meshObj->meshletInfo.span();
	auto dstMeshlets = meshlet.editData(0, srcMeshlets.size());
	
	for (Int i = 0; i < srcMeshlets.size(); ++i) {
		auto& dst = dstMeshlets[i];
		dst = srcMeshlets[i];
		dst.vertOffset += vertOffset;
		dst.primOffset += primOffset;
	}
	
	meshlet.buffer->getUploadedGpuBuffer(req);

	outInfo.meshletOffset = ax_safe_cast_from(meshlet.buffer->gpuBufferIndex());
	outInfo.meshletCount  = ax_safe_cast_from(meshlet.buffer->count());
	
	meshInfo.setValue(0, outInfo);
	meshInfo.buffer->getUploadedGpuBuffer(req);
}

} // namespace
