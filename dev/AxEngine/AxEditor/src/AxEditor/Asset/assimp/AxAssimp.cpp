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
	Array<UPtr<EditableMesh>> _editableMeshes;
	
	JsonValue _metadata;
	Quat4f _constAxisRot;
	
	static constexpr float kLengthScale = 1.0f;
	
	static StrView toStrView(const aiString&     s) { return StrView(s.data, s.length); }
	static Vec3f   toVec3f  (const aiVector3f&   v) { return Vec3f(v.x, v.y, v.z); }
	static Vec3d   toVec3d  (const aiVector3f&   v) { return Vec3d::s_cast(toVec3f(v)); }
	static Vec3d   toVec3d  (const Vec3f&        v) { return Vec3d::s_cast(v); }
	static Color4f toColor4f(const aiColor4D&    v) { return Color4f(v.r, v.g, v.b, v.a); }
	static Quat4f  toQuat4f (const aiQuaternion& q) { return Quat4f(q.x, q.y, q.z, q.w); }
	static Mat4f   toMat4f  (const aiMatrix4x4&  m) {
		return Mat4f(	m.a1, m.b1, m.c1, m.d1,
						m.a2, m.b2, m.c2, m.d2,
						m.a3, m.b3, m.c3, m.d3,
						m.a4, m.b4, m.c4, m.d4);
	}

	static Vec3f   convAxis(const Vec3f& v) { return Vec3f(v.x, v.z, -v.y); }
	static Vec3f   convAxis(const aiVector3f& v) { return convAxis(toVec3f(v)); }
	
	static Vec3f   toLengthVec3f(const Vec3f& v) { return convAxis(v) * kLengthScale; }
	static Vec3d   toLengthVec3d(const Vec3f& v) { return Vec3d::s_cast(toLengthVec3f(v)); }
		

	void openFile(StrView filename) {
		Assimp::Importer importer;
//		importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 256); // mesh shader max 256 vertices
//		importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 256);  // mesh shader max 256 triangles
		
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		// Read the file. The second argument specifies post-processing flags.
		const aiScene* scene = importer.ReadFile(TempString(filename).c_str(),
												 aiProcess_Triangulate
												 // | aiProcess_CalcTangentSpace
												 // | aiProcessPreset_TargetRealtime_MaxQuality
												 // | aiProcess_GlobalScale
												 // | aiProcess_ForceGenNormals
												 // | aiProcess_DropNormals
												 //---
												 // | aiProcess_ConvertToLeftHanded
												 // | aiProcess_MakeLeftHanded
												 // | aiProcess_FlipUVs
												 // | aiProcess_FlipWindingOrder
												 //---
												 | aiProcess_SortByPType // only one primitive type per mesh
												 | aiProcess_PopulateArmatureData
												 // | aiProcess_SplitLargeMeshes
												 | aiProcess_JoinIdenticalVertices);

		// Check for errors
		if (!scene || AI_SCENE_FLAGS_INCOMPLETE == scene->mFlags || !scene->mRootNode) {
			auto msg = Fmt("AxAssimp::openFile {}", StrView_c_str(importer.GetErrorString()));
			throw Error_Runtime(msg);
		}
		
		importMetaData(_metadata.setToObject(), scene->mMetaData);
//		AX_LOG("Metadata {}", _metadata);

		auto convAxisMat = Mat4f::s_direction(convAxis(Vec3f(1, 0, 0)),
		                                      convAxis(Vec3f(0, 1, 0)),
		                                      convAxis(Vec3f(0, 0, 1))).transpose();
		_constAxisRot = convAxisMat.toQuat();
		
		_editableMeshes.ensureCapacity(scene->mNumMeshes);
		_meshes.ensureCapacity(scene->mNumMeshes);
		
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			aiMesh* srcMesh = scene->mMeshes[i];
//			importMesh(srcMesh);
			importEditableMesh(srcMesh);
		}
		
		// importNode(scene->mRootNode, nullptr);
		
		for (auto* srcChild : Span(scene->mRootNode->mChildren, scene->mRootNode->mNumChildren)) {
			importNode(srcChild, nullptr);
		}		
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

	static RenderPrimitiveType toPrimType(int t) {
		if (t & aiPrimitiveType_TRIANGLE)	return RenderPrimitiveType::Triangles;
		if (t & aiPrimitiveType_LINE    )	return RenderPrimitiveType::Lines;
		if (t & aiPrimitiveType_POINT   )	return RenderPrimitiveType::Points;
		throw Error_Runtime(Fmt("Unknown aiPrimitiveType type: %d", t));
	}
	
	VertexNormalCount getNormalCount(aiMesh* srcMesh) {
		auto normals   = srcMesh->mNormals != nullptr;
		auto binormals = srcMesh->mBitangents != nullptr;
		auto tangents  = srcMesh->mTangents != nullptr;
		
		if (tangents) {
			if (!binormals || !normals) throw Error_Undefined();
			return VertexNormalCount::Tangent;
		}
		
		if (binormals) {
			if (!normals) throw Error_Undefined();
			return VertexNormalCount::Binormal;
		}
		
		if (normals) {
			return VertexNormalCount::Normal;
		}
		return VertexNormalCount::None;
	}
	
	void importRenderMesh(aiMesh* srcMesh, RenderMesh& dstMesh) {
		auto normalCount = getNormalCount(srcMesh);
		VertexLayout vertexLayout = VertexLayout::s_make(ax_safe_cast_from(srcMesh->GetNumColorChannels()),
		                                                 ax_safe_cast_from(srcMesh->GetNumColorChannels()),
		                                                 normalCount);
		
		RenderPrimitiveType primType = toPrimType(srcMesh->mPrimitiveTypes);
		dstMesh.create(primType, vertexLayout, VertexIndexType::u16);

		Int numVertices = srcMesh->mNumVertices;
		RenderMeshEdit editMesh(dstMesh);

		auto editVertices = dstMesh.editNewVertices(primType, vertexLayout, VertexIndexType::u16, numVertices);
		if (auto enumerator = editVertices.tryEditPosition()) {
			auto dst = enumerator->begin();
			for (auto& srcValue : Span(srcMesh->mVertices, numVertices)) {
				*dst = toLengthVec3f(toVec3f(srcValue));
				++dst;
			}
			if (dst != enumerator->end()) throw Error_Undefined();
		}
		
		if (auto enumerator = editVertices.tryEditNormal0()) {
			auto dst = enumerator->begin();
			for (auto& srcValue : Span(srcMesh->mNormals, numVertices)) {
				*dst = convAxis(srcValue);
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
	
	void importMeshObject(aiMesh* srcMesh, MeshObject* dstMesh) {
		RenderPrimitiveType primType = toPrimType(srcMesh->mPrimitiveTypes);
		if (primType != RenderPrimitiveType::Triangles) {
			AX_ASSERT_TODO();
			return;
		}

		u32 numVert = srcMesh->mNumVertices;
		u32 numPrim = srcMesh->mNumFaces; 
		
		auto dstVert = dstMesh->meshletVert->editData<AxMeshletVert>(0, numVert);
		auto dstPrim = dstMesh->meshletPrim->editData<AxMeshletPrim>(0, numPrim);
		
		{ // pos
			Int j = 0;
			for (auto& srcValue : Span(srcMesh->mVertices, numVert)) {
				dstVert[j].pos = toLengthVec3f(toVec3f(srcValue));
				dstVert[j].rawColor = 0xffffffff;
				dstVert[j].uv0 = {};
				dstVert[j].uv1 = {};
				++j;
			}
		}
		
		if (srcMesh->HasNormals()) {
			Int j = 0;
			for (auto& srcValue : Span(srcMesh->mNormals, numVert)) {
				dstVert[j].normal = convAxis(srcValue);
				++j;
			}
		}
		
		{
			Int j = 0;
			for (auto& srcFace : Span(srcMesh->mFaces, srcMesh->mNumFaces)) {
				if (srcFace.mNumIndices != 3) continue;
				const auto* src = srcFace.mIndices;
				dstPrim[j].tri.set(src[0], src[1], src[2]);
				++j;
			}
		}
		
		AxMeshlet meshlet = {};
		meshlet.vertCount = numVert;
		meshlet.primCount = numPrim;
		dstMesh->meshlet->setValue(0, meshlet);
	}
	
	void importMesh(aiMesh* srcMesh) {
		auto meshObject = MeshObject_Backend::s_new(AX_NEW);
		_meshes.emplaceBack(meshObject);
		importRenderMesh(srcMesh, meshObject->meshData);
		importMeshObject(srcMesh, meshObject);
	}
	
	void importEditableMesh(aiMesh* srcMesh) {
		auto& dstMesh = _editableMeshes.emplaceNewObject(AX_NEW);
		
		Int numVertices = srcMesh->mNumVertices;
		auto srcFaces    = Span(srcMesh->mFaces,    srcMesh->mNumFaces);
		
		dstMesh->_points.ensureCapacity(numVertices);
		for (auto& src : Span(srcMesh->mVertices, srcMesh->mNumVertices)) {
			dstMesh->addPoint(toLengthVec3d(toVec3f(src)));
		}

		dstMesh->_faces.ensureCapacity(srcFaces.size());
		for (auto& srcFace : srcFaces) {
			Array<Int, 64> fv;
			fv.resize(srcFace.mNumIndices);
			for (Int j = 0; j < srcFace.mNumIndices; ++j) {
				fv[j] = static_cast<Int>(srcFace.mIndices[j]);
			}
			dstMesh->addFace(fv);
		}

			
		if (srcMesh->HasNormals()) {
			auto srcNormals = Span(srcMesh->mNormals, numVertices);
			dstMesh->_fvNormals.resize(dstMesh->faceEdges().size());
			Int writeIndex = 0;
			for (auto& srcFace : srcFaces) {
				for (Int j = 0; j < srcFace.mNumIndices; ++j) {
					auto vertexIndex = static_cast<Int>(srcFace.mIndices[j]);
					auto src = srcNormals[vertexIndex];
					dstMesh->_fvNormals[writeIndex] = toVec3d(convAxis(src));
					++writeIndex;
				}
			}
		}
		
		Int numColorChannels = srcMesh->GetNumColorChannels();
		for (Int chIndex = 0; chIndex < numColorChannels; ++chIndex) {
			auto& dstChannel = dstMesh->addColorChannel(Color4f::kWhite());
			auto srcColors = Span(srcMesh->mColors[chIndex], numVertices);
			Int writeIndex = 0;
			for (auto& srcFace : srcFaces) {
				for (Int j = 0; j < srcFace.mNumIndices; ++j) {
					auto vertexIndex = static_cast<Int>(srcFace.mIndices[j]);
					auto src = srcColors[vertexIndex];
					dstChannel.values[writeIndex] = toColor4f(src);
					++writeIndex;
				}
			}
		}
		
		
		SPtr<MeshObject> meshObject = MeshObject_Backend::s_new(AX_NEW);
		_meshes.emplaceBack(meshObject);

		auto vertexLayout = VertexLayout::s_make(Math::max(1LL, dstMesh->colorChannelCount()),
		                                         dstMesh->uvChannelCount(),
		                                         VertexNormalCount::Normal);
		
		RenderMeshEdit(meshObject->meshData).createFromEditableMesh(vertexLayout, *dstMesh);
	}

	void importNode(const aiNode* srcNode, SceneEntity* parent) {
		auto entity = SceneEntity::s_new(AX_NEW, parent, toStrView(srcNode->mName));
		
		auto& tran = entity->transform;
		auto localMat = toMat4f(srcNode->mTransformation);
		tran.setLocalMatrix(localMat);
		if (!parent) {
			tran.rotation *= _constAxisRot;
		} else {
			tran.position = toLengthVec3f(tran.position);
		}

//		AX_LOG("AxAssimp: importNode {} {}", entity->name(), entity->transform.rotation.eulerDeg());
		
		auto* stockObjs = RenderStockObjects::s_instance();
		
		for (auto& srcMeshIndex : Span(srcNode->mMeshes, srcNode->mNumMeshes)) {
			if (auto meshObj = _meshes.tryGetElement(srcMeshIndex)) {
				auto* meshRenderer = entity->addComponent<CMeshRenderer>(AX_NEW);
				meshRenderer->mesh = *meshObj;
			
				// meshRenderer->material	= srcMeshIndex % 2 
				// 						? stockObjs->materials->meshlet 
				// 						: stockObjs->materials->Simple3D_Blinn_Color;
				
				meshRenderer->material	= stockObjs->materials->Simple3D_Blinn_Color; 
//				meshRenderer->material	= stockObjs->materials->meshlet; 
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
