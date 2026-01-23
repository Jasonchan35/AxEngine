module;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

module AxEditor;
import :AxAssimp;

namespace AxEditor {

class AxAssimp_Importer : public NonCopyable {
public:
	Array<SPtr<MeshObject>> _meshes;
	JsonValue _metadata;

	void openFile(StrView filename) {
		Assimp::Importer importer;
	
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		// Read the file. The second argument specifies post-processing flags.
		const aiScene* scene = importer.ReadFile(TempString(filename).c_str(),
												 aiProcess_Triangulate
												 // | aiProcess_CalcTangentSpace
												 // | aiProcessPreset_TargetRealtime_MaxQuality
												 // | aiProcess_ConvertToLeftHanded
												 // | aiProcess_GlobalScale
												 // | aiProcess_ForceGenNormals
												 // | aiProcess_DropNormals
												 | aiProcess_SortByPType // only one primitive type per mesh
												 | aiProcess_PopulateArmatureData
												 | aiProcess_SplitLargeMeshes
												 | aiProcess_JoinIdenticalVertices);

		// Check for errors
		if (!scene || AI_SCENE_FLAGS_INCOMPLETE == scene->mFlags || !scene->mRootNode) {
			auto msg = Fmt("AxAssimp::openFile {}", StrView_c_str(importer.GetErrorString()));
			throw Error_Runtime(msg);
		}
		
		importMetaData(_metadata.setToObject(), scene->mMetaData);
		AX_LOG("Metadata {}", _metadata);
		
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			aiMesh* srcMesh = scene->mMeshes[i];
			importMesh(srcMesh);
		}
		
		importNode(scene->mRootNode, nullptr);
	}
	
	static void importMetaData(JsonObject* jsonObj, const aiMetadata* srcData) {
		if (!srcData) return;
		
		for (unsigned int i = 0; i < srcData->mNumProperties; ++i) {
			auto* dstVal = jsonObj->getOrAddMember(toStrView(srcData->mKeys[i]));
			
			auto& srcVal = srcData->mValues[i];
			switch (srcVal.mType) {
				case AI_BOOL         : dstVal->setValue(reinterpret_cast<bool*>(srcVal.mData)); break;
				case AI_INT32        : dstVal->setValue(reinterpret_cast<i32 *>(srcVal.mData)); break;
				case AI_INT64        : dstVal->setValue(reinterpret_cast<i64 *>(srcVal.mData)); break;
				case AI_UINT32       : dstVal->setValue(reinterpret_cast<u32 *>(srcVal.mData)); break;
				case AI_UINT64       : dstVal->setValue(reinterpret_cast<u64 *>(srcVal.mData)); break;
				case AI_FLOAT        : dstVal->setValue(reinterpret_cast<f32 *>(srcVal.mData)); break;
				case AI_DOUBLE       : dstVal->setValue(reinterpret_cast<f64 *>(srcVal.mData)); break;
				case AI_AISTRING     : {
					auto* srcString = static_cast<aiString*>(srcVal.mData);
					dstVal->setValue(toStrView(*srcString));
				} break;
				case AI_AIVECTOR3D   : {
					const auto* srcVertor = static_cast<aiVector3D*>(srcVal.mData);
					auto& dstArray  = *dstVal->setToArray(3);
					dstArray[0] = srcVertor->x;
					dstArray[1] = srcVertor->y;
					dstArray[2] = srcVertor->z;
				} break;
				case AI_AIMETADATA   : {
					auto* srcMetadata = static_cast<aiMetadata*>(srcVal.mData);
					importMetaData(dstVal->setToObject(), srcMetadata);
				} break;
				default: throw Error_Runtime(Fmt("Unknown aiMetadata type: %d", ax_enum_int(srcVal.mType)));
			}
		}
		
	}

	static StrView toStrView(const aiString&     s) { return StrView(s.data, s.length); }
	static Vec3f   toVec3f  (const aiVector3f&   v) { return Vec3f(v.x, v.y, v.z); }
	static Vec3d   toVec3d  (const aiVector3f&   v) { return Vec3d::s_cast(toVec3f(v)); }
	static Quat4f  toQuat4f (const aiQuaternion& q) { return Quat4f(q.x, q.y, q.z, q.w); }
	static Mat4f   toMat4f  (const aiMatrix4x4&  m) {
		return Mat4f(	m.a1, m.b1, m.c1, m.d1,
						m.a2, m.b2, m.c2, m.d2,
						m.a3, m.b3, m.c3, m.d3,
						m.a4, m.b4, m.c4, m.d4);
	}
	
	static RenderPrimitiveType toPrimType(int t) {
		if (t & aiPrimitiveType_TRIANGLE)	return RenderPrimitiveType::Triangles;
		if (t & aiPrimitiveType_LINE    )	return RenderPrimitiveType::Lines;
		if (t & aiPrimitiveType_POINT   )	return RenderPrimitiveType::Points;
		throw Error_Runtime(Fmt("Unknown aiPrimitiveType type: %d", t));
	}
	
	void importMesh(aiMesh* srcMesh) {
		auto meshObject = MeshObject_Backend::s_new(AX_NEW);
		_meshes.emplaceBack(meshObject);
		auto& meshData = meshObject->meshData;
		
		VertexLayout vertexLayout = Vertex_PosNormalUvColor::s_layout();
		RenderPrimitiveType primType = toPrimType(srcMesh->mPrimitiveTypes);
		meshData.create(primType, vertexLayout, VertexIndexType::u16);

		Int numVertices = srcMesh->mNumVertices;
		RenderMeshEdit editMesh(meshData);

		auto editVertices = meshData.editNewVertices(primType, vertexLayout, VertexIndexType::u16, numVertices);

		if (auto enumerator = editVertices.tryEditPosition()) {
			auto dst = enumerator->begin();
			for (auto& srcPos : Span(srcMesh->mVertices, numVertices)) {
				*dst = toVec3f(srcPos);
				++dst;
			}
			if (dst != enumerator->end()) throw Error_Undefined();
		}
		
		switch (primType) {
			case RenderPrimitiveType::Triangles: {
				for (auto& srcFace : Span(srcMesh->mFaces, srcMesh->mNumFaces)) {
					if (srcFace.mNumIndices != 3) continue;
					const auto* vi = srcFace.mIndices;
					u16 indices[] = {
						static_cast<u16>(vi[0]),
						static_cast<u16>(vi[1]),
						static_cast<u16>(vi[2])
					};
					editVertices.addIndices(Span(indices));
				}
			} break;
			default: AX_ASSERT_TODO();
		}
	}

	void importNode(const aiNode* srcNode, SceneEntity* parent) {
		auto entity = SceneEntity::s_new(AX_NEW, parent, toStrView(srcNode->mName));
		
		auto localMat = toMat4f(srcNode->mTransformation);
		entity->transform.setMatrix(localMat);
		
		for (auto& srcMeshIndex : Span(srcNode->mMeshes, srcNode->mNumMeshes)) {
			if (auto meshObj = _meshes.tryGetElement(srcMeshIndex)) {
				auto* meshRenderer = entity->addComponent<CMeshRenderer>(AX_NEW);
				meshRenderer->mesh = *meshObj;
				meshRenderer->material = RenderStockObjects::s_instance()->materials->simple3d_color;
			}
		} 

		entity->ensureChildrenCapacity(srcNode->mNumChildren);
		for (auto* srcChild : Span(srcNode->mChildren, srcNode->mNumChildren)) {
			importNode(srcChild, entity);
		}
	}
};

void AxAssimp::openFile(StrView filename) {
	AxAssimp_Importer importer;
	importer.openFile(filename);
}

} // namespace
