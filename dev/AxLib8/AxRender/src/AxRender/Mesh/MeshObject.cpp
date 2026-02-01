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
: _assetPath(desc.assetPath) 
{
}

void MeshObject::createBuffers() {
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	    meshlet.create(AX_NEW, "meshlet"    , objMgr->_structBufferPools.axMeshlet);
	meshletVert.create(AX_NEW, "meshletVert", objMgr->_structBufferPools.axMeshletVert);
	meshletPrim.create(AX_NEW, "meshletPrim", objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject::createFromEditableMesh(const EditableMesh& srcMesh) {
	createBuffers();
	constexpr Int kMaxVertexCountPerMeshlet = AX_HLSL_MESH_SHADER_MAX_VERTEX_COUNT;
	
	meshletInfo.clear();
	auto* curMeshlet = &meshletInfo.emplaceBack();
	*curMeshlet = {};

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
		
		auto dstVertices = meshletVert.extendsData(fvCount);
		auto srcNormals = face.getNormals(srcMesh);
		
		face.getPositions(srcMesh, facePositions);
		for (Int j = 0; j < fvCount; ++j) {
			auto& dstVert = dstVertices[j];
			dstVert.pos      = Vec3f::s_cast(facePositions[j]);
			dstVert.normal   = Vec3f::s_cast(srcNormals[j]);
			dstVert.rawColor = 0xffffffff;
		}

		u32 viBase = curMeshlet->vertCount;
		auto dstIdx  = meshletPrim.extendsData(triCount);
		for (u32 j = 0; j < triCount; ++j) {
			// triangle fan
			dstIdx[j].tri = u32x3(0, j+1, j+2) + viBase;
		}

		curMeshlet->vertCount += fvCount;
		curMeshlet->primCount += triCount;
	}
	
	rttiCastCheck<MeshObject_Backend>(this)->objectSlot.markDirty();
}

} // namespace
