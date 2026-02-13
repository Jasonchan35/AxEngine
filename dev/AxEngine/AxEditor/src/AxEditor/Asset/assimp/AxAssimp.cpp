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
	SPtr<SceneWorld> _world;
	
	JsonValue _metadata;
	Quat4f _constAxisRot;
	
	String _filename;
	
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
		

	SPtr<SceneWorld> openFile(StrView filename) {
		_world = SceneWorld::s_new(AX_NEW);
		
		_filename = filename;
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
		
		for (u32 i = 0; i < scene->mNumMeshes; ++i) {
			importMesh(scene->mMeshes[i]);
		}
		
		for (u32 i = 0; i < scene->mNumLights; ++i) {
			importLight(scene->mLights[i]);
		}
		
		for (u32 i = 0; i < scene->mNumCameras; ++i) {
			importCamera(scene->mCameras[i]);
		}
		
		// importNode(scene->mRootNode, nullptr);
		
		for (auto* srcChild : Span(scene->mRootNode->mChildren, scene->mRootNode->mNumChildren)) {
			importNode(srcChild, nullptr);
		}
		
		return _world;
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
	
	void importRenderMesh(aiMesh* srcMesh, RenderMesh& dstMesh, VertexLayout vertexLayout) {
		auto normalCount = getNormalCount(srcMesh);
		if (!vertexLayout) {
			vertexLayout = VertexLayout::s_make(ax_safe_cast_from(srcMesh->GetNumColorChannels()),
												ax_safe_cast_from(srcMesh->GetNumUVChannels()),
												normalCount);
		}
		
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

	void importLight(aiLight* srcLight) {
		
	}
	
	void importCamera(aiCamera* srcCamera) {
		
	}
	
	void importMesh(aiMesh* srcMesh) {
		auto meshObject = MeshObject::s_new(AX_NEW);
		_meshes.emplaceBack(meshObject);
		// importRenderMesh(srcMesh, meshObject->renderMesh, Vertex_PosColorUv2Normal::s_layout());
		importRenderMeshByEditableMesh(srcMesh, meshObject);
		importMeshlet(srcMesh, meshObject);
	}

	void importMeshlet(aiMesh* srcMesh, MeshObject* dstMesh) {
		dstMesh->setName(toStrView(srcMesh->mName));
		
		auto cacheFilename = Fmt("{}-{}.axMeshlet-cache", _filename, dstMesh->name());
		
		if (File::isNewerThan(cacheFilename, _filename)) {
			dstMesh->readMeshletFromFile(cacheFilename);
			return;
		}
		
		Int numVertices = srcMesh->mNumVertices;
		auto srcVertices = Span(srcMesh->mVertices, numVertices);
		auto srcNormals  = Span(srcMesh->mNormals,  numVertices);
		auto srcFaces    = Span(srcMesh->mFaces,    srcMesh->mNumFaces);
		
		Array<AxGpuMeshletVert> vertices;
		vertices.ensureCapacity(numVertices);
		for (Int i = 0; i < numVertices; ++i) {
			auto& dst = vertices.emplaceBack();
			dst.pos = toLengthVec3f(toVec3f(srcVertices[i]));
			if (srcMesh->HasNormals()) {
				dst.normal = convAxis(srcNormals[i]);
			}
		}
		
		Array<u32> indices;
		for (auto& srcFace : srcFaces) {
			if (srcFace.mNumIndices != 3) {
				AX_ASSERT(false);
				continue;
			}
			for (Int j = 0; j < 3; ++j) {
				indices.emplaceBack(srcFace.mIndices[j]);
			}
		}
		
		dstMesh->createMeshlet(vertices, indices);
		dstMesh->writeMeshletToFile(cacheFilename);
	}
	
	void importRenderMeshByEditableMesh(aiMesh* srcMesh, MeshObject* meshObject) {
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

		dstMesh->updateFaceNormals();
#if 1
		auto vertexLayout = Vertex_PosColorUv2Normal::s_layout();
#else
		auto vertexLayout = VertexLayout::s_make(Math::max(1LL, dstMesh->colorChannelCount()),
		                                         dstMesh->uvChannelCount(),
		                                         VertexNormalCount::Normal);
#endif
		
		RenderMeshEdit(meshObject->renderMesh).createFromEditableMesh(vertexLayout, *dstMesh);
	}

	void importNode(const aiNode* srcNode, SceneEntity* parent) {
		auto entity = SceneEntity::s_new(AX_NEW, _world, parent, toStrView(srcNode->mName));
		importMetaData(entity->metadata.setToObject(), srcNode->mMetaData);
//		AX_LOG("import node [{}] metadata: {}", entity->name(), entity->metadata);
		
		auto trs = toMat4f(srcNode->mTransformation).getTRS();
		if (!parent) {
			entity->setPosition(trs.position);
			entity->setRotation(trs.rotation * _constAxisRot);
			
		} else {
			entity->setPosition(toLengthVec3f(trs.position));
			entity->setRotation(trs.rotation);
		}
		entity->setScale(trs.scale);

//		AX_LOG("AxAssimp: importNode {} {}", entity->name(), entity->transform.rotation.eulerDeg());
		
		auto* stockObjs = RenderStockObjects::s_instance();
		
		for (auto& srcMeshIndex : Span(srcNode->mMeshes, srcNode->mNumMeshes)) {
			if (auto meshObj = _meshes.tryGetElement(srcMeshIndex)) {
				auto* meshRenderer = entity->addComponent<MeshRendererComponent>(AX_NEW);
				meshRenderer->mesh = *meshObj;
				meshRenderer->material	= stockObjs->materials->Simple3D_Blinn_Color; 
			}
		}

		entity->ensureChildrenCapacity(srcNode->mNumChildren);
		for (auto* srcChild : Span(srcNode->mChildren, srcNode->mNumChildren)) {
			importNode(srcChild, entity);
		}
	}
};

SPtr<SceneWorld> AxAssimp::openFile(StrView filename) {
	AxAssimp_Importer importer;
	return importer.openFile(filename);
}

} // namespace
