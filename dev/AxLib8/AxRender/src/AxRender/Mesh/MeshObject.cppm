module;

export module AxRender:MeshObject;
export import :RenderMesh;

export namespace ax {

struct AxMeshletVert {
	Vec3f   pos;
	u32     rawColor;
	Vec2f   uv0;
	Vec2f   uv1;
	// Vec2f   rawNormal;
	Vec3f   normal;
	u32 unused[5];
};

struct AxMeshletPrim {
	Vec3u32	tri;
	u32 padding;
};

struct AxMeshlet {
	u32 vertOffset = 0;
	u32 vertCount  = 0;
	u32 primOffset = 0;
	u32 primCount  = 0;
};

class MeshObject : public RenderObject {
	AX_RTTI_INFO(MeshObject, RenderObject)
public:
	using CreateDesc = MeshObject_CreateDesc;

	static SPtr<MeshObject>	s_new(const MemAllocRequest& req);
	static SPtr<MeshObject>	s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderMesh	meshData;

	StructuredGpuBuffer_<AxMeshlet>     meshlet;
	StructuredGpuBuffer_<AxMeshletVert> meshletVert;
	StructuredGpuBuffer_<AxMeshletPrim> meshletPrim;
	
	AX_INLINE void _uploadToGpu(RenderRequest* req) {
		if (!_needUploadToGpu) return;
		_needUploadToGpu = false;
		_doUploadToGpu(req);
	}

protected:
	MeshObject(const CreateDesc& desc);
	void _doUploadToGpu(RenderRequest* req);
	
	bool _needUploadToGpu = true;
	String	_assetPath;
};

} // namespace
