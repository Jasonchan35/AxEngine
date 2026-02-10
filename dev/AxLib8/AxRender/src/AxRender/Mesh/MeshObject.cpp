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
	_gpuData.bounds = BBox3f::s_empty();
}

auto MeshObject::onGetGpuData(RenderRequest* req) -> const GpuData*{
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

	_gpuData.meshletOffset  = ax_safe_cast_from(meshlet.buffer->gpuBufferIndex());
	_gpuData.meshletCount   = ax_safe_cast_from(meshlet.buffer->count());
	_gpuData.totalVertCount = ax_safe_cast_from(meshletVert.buffer->count());
	_gpuData.totalPrimCount = ax_safe_cast_from(meshletPrim.buffer->count());
	return &_gpuData;
}

void MeshObject::createBuffers() {
	auto* objMgr = RenderObjectManager_Backend::s_instance();
	    meshlet.create(AX_NEW, "axGpuMeshlet"    , objMgr->_structBufferPools.axMeshlet);
	meshletVert.create(AX_NEW, "axGpuMeshletVert", objMgr->_structBufferPools.axMeshletVert);
	meshletPrim.create(AX_NEW, "axGpuMeshletPrim", objMgr->_structBufferPools.axMeshletPrim);
}

void MeshObject::createFromEditableMesh(const EditableMesh& srcMesh) {
	createBuffers();
	constexpr Int kMaxVertexCountPerMeshlet = AX_HLSL_MESH_SHADER_MAX_VERT_COUNT;
	
	meshletInfo.clear();
	auto* curMeshlet = &meshletInfo.emplaceBack();
	*curMeshlet = {};
	
	BBox3f bounds = BBox3f::s_empty();
	for (auto& pt : srcMesh.points()) {
		bounds.includePoint(Vec3f::s_cast(pt.pos));
	}
	_gpuData.bounds = bounds;

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
	
	objectSlot.markDirty();
}

void MeshObject::createFromEditableMesh2(const EditableMesh& srcMesh) {
	createBuffers();
	
	std::vector<ClusterGenerator::Vertex> vertices;
	std::vector<u32> indices;
	
//	auto dstVertSpan = meshletVert.editData(0, srcMesh.points().size());
//	Int j = 0;
	vertices.reserve(srcMesh.points().size());
	for (auto& pt : srcMesh.points()) {
		auto& v = vertices.emplace_back();
		auto pos    = Vec3f::s_cast(pt.pos);
		auto normal = Vec3f::s_cast(pt.normal);
		v.px = pos.x;
		v.py = pos.y;
		v.pz = pos.z;
		v.nx = normal.x;
		v.ny = normal.y;
		v.nz = normal.z;
/*		
		auto& dstVert = dstVertSpan[j]; 
		dstVert.pos    = pos;
		dstVert.normal = normal;
		dstVert.rawColor = 0xFFFFFFFF;
		++j;
		*/
	}

	indices.reserve(srcMesh.faces().size() * 3);
	for (auto& face : srcMesh.faces()) {
		auto faceEdges = face.getFaceEdges(srcMesh);
		auto triCount = faceEdges.size() - 2;
		for (Int i = 0; i < triCount; ++i) {
			indices.emplace_back(static_cast<u32>(faceEdges[0  ].pointId()));
			indices.emplace_back(static_cast<u32>(faceEdges[i+1].pointId()));
			indices.emplace_back(static_cast<u32>(faceEdges[i+2].pointId()));
		}
	}
	
	ClusterGenerator gen;
	gen.nanite(*this, vertices, indices);
	
	objectSlot.markDirty();
}

} // namespace
