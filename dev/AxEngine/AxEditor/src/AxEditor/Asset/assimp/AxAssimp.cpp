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
	
	
	void openFile(StrView filename) {
		Assimp::Importer importer;
	
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		// Read the file. The second argument specifies post-processing flags.
		const aiScene* scene = importer.ReadFile(TempString(filename).c_str(),
												 aiProcess_Triangulate
												 // | aiProcess_CalcTangentSpace
												 // | aiProcess_SortByPType
												 // | aiProcessPreset_TargetRealtime_MaxQuality
												 // | aiProcess_ConvertToLeftHanded
												 // | aiProcess_GlobalScale
												 // | aiProcess_ForceGenNormals
												 // | aiProcess_DropNormals
												 | aiProcess_PopulateArmatureData
												 | aiProcess_SplitLargeMeshes
												 | aiProcess_JoinIdenticalVertices);

		// Check for errors
		if (!scene || AI_SCENE_FLAGS_INCOMPLETE == scene->mFlags || !scene->mRootNode) {
			auto msg = Fmt("AxAssimp::openFile {}", StrView_c_str(importer.GetErrorString()));
			throw Error_Runtime(msg);
		}
		
		importNode(scene->mRootNode, nullptr, Mat4f::s_identity());
	
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			aiMesh* srcMesh = scene->mMeshes[i];
			importMesh(srcMesh);
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
	
	void importMesh(aiMesh* srcMesh) {
		EditableMesh editMesh;
		editMesh._points.ensureCapacity(srcMesh->mNumVertices);
		editMesh._faces.ensureCapacity(srcMesh->mNumFaces);
		editMesh._faceEdges.ensureCapacity(srcMesh->mNumFaces * 3);
		
		for (auto& srcVert : Span(srcMesh->mVertices, srcMesh->mNumVertices)) {
			editMesh.addPoint(toVec3d(srcVert));
		}
		
		for (auto& srcFace : Span(srcMesh->mFaces, srcMesh->mNumFaces)) {
			Array<Int, 32> indices;
			indices.appendRange(
				Span(srcFace.mIndices, srcFace.mNumIndices), 
				[](auto& v) { return v;	});
			editMesh.addFace(indices);
		}
		
//		auto dstMesh = MeshObject_Backend::s_new(AX_NEW);
//		RenderMeshEdit(dstMesh->meshData).createFromEditableMesh(editMesh);
	}
	
	void importNode(const aiNode* srcNode, SceneEntity* parent, const Mat4f& parentInverseMat) {
		auto entity = SceneEntity::s_new(AX_NEW, parent, toStrView(srcNode->mName));
		
		auto worldMat = toMat4f(srcNode->mTransformation);
		auto localMat = worldMat * parentInverseMat;
		entity->transform.setMatrix(localMat);
		auto worldMatInv = worldMat.inverse();

		entity->ensureChildrenCapacity(srcNode->mNumChildren);
		for (auto* srcChild : Span(srcNode->mChildren, srcNode->mNumChildren)) {
			importNode(srcChild, entity, worldMatInv);
		}
	}
};

void AxAssimp::openFile(StrView filename) {
	AxAssimp_Importer importer;
	importer.openFile(filename);
}

} // namespace
