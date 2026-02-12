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
	void create(Span<AxGpuMeshletVert> vertices, Span<u32> indices);

	StructuredGpuBuffer_<AxGpuMeshletGroup> meshletGroupBuffer;
	StructuredGpuBuffer_<AxGpuMeshlet>      meshletBuffer;
	StructuredGpuBuffer_<AxGpuMeshletVert>  meshletVertBuffer;
	StructuredGpuBuffer_<AxGpuMeshletPrim>  meshletPrimBuffer;
		
	void createBuffers();

	void setBounds(const BBox3f& v) { _bounds = v; objectSlot.markDirty(); }
	const BBox3f& bounds() const { return _bounds; }
	
	using ResourceKey = String;
	const ResourceKey& resourceKey() const { return _assetPath; }

	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuMeshObject"); }
	using GpuData = AxGpuMeshObject;
	const GpuData* onGetGpuData(RenderRequest* req);
	
protected:
	String  _assetPath;
	BBox3f  _bounds = BBox3f::s_empty();
	GpuData _gpuData = {};
	
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
