module;

export module AxRender:MeshObject;
export import :RenderMesh;
export import :Material;

export namespace ax {

class EditableMesh;


class MeshObject : public RenderObject {
	AX_RTTI_INFO(MeshObject, RenderObject)
public:
	using CreateDesc = MeshObject_CreateDesc;

	static SPtr<MeshObject>	s_new(const MemAllocRequest& req);
	static SPtr<MeshObject>	s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderMesh	meshData;
	
	void createFromEditableMesh(const EditableMesh& srcMesh);
	
	
	bool isMeshletValid() const { return meshletInfo.size() > 0; }
	
	Array<AxMeshlet> meshletInfo;
	
	StructuredGpuBuffer_<AxMeshlet>     meshlet;
	StructuredGpuBuffer_<AxMeshletVert> meshletVert;
	StructuredGpuBuffer_<AxMeshletPrim> meshletPrim;
		
	void createBuffers();

protected:
	MeshObject(const CreateDesc& desc);
	
	String _assetPath;
};

class MeshObjectRenderer_CreateDesc {};

class MeshObjectRenderer : public NonCopyable {
public:
	Mat4f				worldMatrix;
	SPtr<MeshObject>	mesh;
	SPtr<Material>		material;
};

} // namespace
