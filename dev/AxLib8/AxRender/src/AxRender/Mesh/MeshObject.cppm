module;

export module AxRender:MeshObject;
export import :RenderMesh;
export import :Material;

export namespace ax {

class EditableMesh;

class MeshObject_Meshlet {
public:
	StructuredGpuBuffer_<AxGpuMeshletGroup>   groupBuffer;
	StructuredGpuBuffer_<AxGpuMeshletCluster> clusterBuffer;
	StructuredGpuBuffer_<AxGpuMeshletVert>    vertBuffer;
	StructuredGpuBuffer_<AxGpuMeshletPrim>    primBuffer;
		
	using GpuData = AxGpuMeshObject;
	const GpuData* onGetGpuData(MeshObject* meshObj, RenderRequest* req);
	
	void createBuffers();
	void writeToFile(MeshObject* meshObj, StrView filename);
	void readFromFile(MeshObject* meshObj, StrView filename);
	
	GpuData _gpuData = {};
};

class MeshObject : public RenderObject {
	AX_RTTI_INFO(MeshObject, RenderObject)
public:
	using CreateDesc = MeshObject_CreateDesc;

	static SPtr<MeshObject>	s_new(const MemAllocRequest& req);
	static SPtr<MeshObject>	s_new(const MemAllocRequest& req, const CreateDesc& desc);

	~MeshObject() override;

	RenderMesh			renderMesh;
	MeshObject_Meshlet	meshlet;
	
	void createMeshlet(Span<AxGpuMeshletVert> vertices, Span<u32> indices);
	void writeMeshletToFile(StrView filename) { meshlet.writeToFile(this, filename); }
	void readMeshletFromFile(StrView filename) { meshlet.readFromFile(this, filename); }

	void setBounds(const BBox3f& v) { _bounds = v; objectSlot.markDirty(); }
	const BBox3f& bounds() const { return _bounds; }
	
	using ResourceKey = String;
	const ResourceKey& resourceKey() const { return _assetPath; }

	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuMeshObject"); }
	using GpuData = MeshObject_Meshlet::GpuData;
	const GpuData* onGetGpuData(RenderRequest* req) { return meshlet.onGetGpuData(this, req); }
	
protected:
	friend MeshObject_Meshlet;
	
	String  _assetPath;
	BBox3f  _bounds = BBox3f::s_empty();
	
	MeshObject(const CreateDesc& desc);
	
public:
	using ObjectSlot = RenderObjectSlot<This>; 
	ObjectSlot      objectSlot;
};

} // namespace
