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
	void createFromEditableMesh2(const EditableMesh& srcMesh);

	bool isMeshletValid() const { return meshletInfo.size() > 0; }
	
	Array<AxGpuMeshlet> meshletInfo;
	StructuredGpuBuffer_<AxGpuMeshlet>     meshlet;
	StructuredGpuBuffer_<AxGpuMeshletVert> meshletVert;
	StructuredGpuBuffer_<AxGpuMeshletPrim> meshletPrim;
		
	void createBuffers();
	
	BBox3f& bounds() { return _gpuData.bounds; }
	
	using ResourceKey = String;
	const ResourceKey& resourceKey() const { return _assetPath; }

	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuMeshObject"); }
	using GpuData = AxGpuMeshObject;
	const GpuData* onGetGpuData(RenderRequest* req);
	
protected:
	String          _assetPath;
	AxGpuMeshObject _gpuData;
	
	MeshObject(const CreateDesc& desc);
	
public:
	using ObjectSlot = RenderObjectSlot<This>; 
	ObjectSlot      objectSlot;
};

class MeshObjectRenderer_CreateDesc {};

class MeshObjectRenderer : public RttiObject {
	AX_RTTI_INFO(MeshObjectRenderer, NoBaseClass)
public:
	using ResourceKey = TagNoInit_T;

	SPtr<MeshObject>	mesh;
	SPtr<Material>		material;

	MeshObjectRenderer() : objectSlot(this) {}
	
	static constexpr Int s_gpuBufferMaxSize()  { return RenderObject::s_gpuBufferMaxSize(); }
	static constexpr Int s_gpuBufferPageSize() { return RenderObject::s_gpuBufferPageSize(); }
	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuMeshObjectRenderer"); }
	
	using GpuData = AxGpuMeshObjectRenderer;
	GpuData _gpuData;
	const GpuData* onGetGpuData(RenderRequest* req) {
		_gpuData.meshObjectId = mesh ? mesh->objectSlot.slotId() : 0;
		return &_gpuData;
	}
	
public:
	using ObjectSlot = RenderObjectSlot<This>;
	ObjectSlot objectSlot;
};

} // namespace
