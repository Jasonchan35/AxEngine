module;

export module AxRender:MeshObject;
export import :RenderMesh;
export import :RenderMath;
export import :Material;

export namespace ax {

class EditableMesh;

class MeshObject_Meshlet {
public:
	StructuredGpuBuffer_<AxGpuData_MeshletGroup>   groupBuffer;
	StructuredGpuBuffer_<AxGpuData_MeshletCluster> clusterBuffer;
	StructuredGpuBuffer_<AxGpuData_MeshletVert>    vertBuffer;
	StructuredGpuBuffer_<AxGpuData_MeshletPrim>    primBuffer;
		
	using GpuData = AxGpuData_MeshObject;
	const GpuData* onGetGpuData(MeshObject* meshObj, RenderRequest* req);
	
	void createBuffers();
	void writeToFile(MeshObject* meshObj, StrView filename);
	void readFromFile(MeshObject* meshObj, StrView filename);
	
	GpuData _gpuData = {};
};

struct AxGpuMeshletVert_Unpacked {
	Vec3f   pos;
	Color4b color;
	Vec3f   normal;
	Vec2f   uv0;
	Vec2f   uv1;
	
	AxGpuData_MeshletVert pack() const {
		AxGpuData_MeshletVert o;
		o.pos               = pos;
		o.color_packed      = ax_pack_color_u32(color);
		o.normal_octahedral = ax_pack_normal_octahedral(normal);
		o.uv0_packed        = ax_pack_uv_u32(uv0);
		o.uv1_packed        = ax_pack_uv_u32(uv1);
		return o;
	}
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
	
	void createMeshlet(Span<AxGpuMeshletVert_Unpacked> vertices, Span<u32> indices);
	void writeMeshletToFile(StrView filename) { meshlet.writeToFile(this, filename); }
	void readMeshletFromFile(StrView filename) { meshlet.readFromFile(this, filename); }

	void setBounds(const BBox3f& v) { _bounds = v; objectSlot.markDirty(); }
	const BBox3f& bounds() const { return _bounds; }
	
	using ResourceKey = String;
	const ResourceKey* resourceKey() const { return _assetPath ? &_assetPath : nullptr; }

	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuData_MeshObject"); }
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
