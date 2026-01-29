module;

export module AxRender:MeshObject;
export import :RenderMesh;

export namespace ax {

struct MeshletVert {
	Vec3f   pos;
	u32     rawColor;
	Vec2f   uv0;
	Vec2f   uv1;
	// Vec2f   rawNormal;
	Vec3f   normal;
};

struct MeshletPrim {
	Vec3u32	tri;
};

struct Meshlet {
	u32 vertCount  = 0;
	u32 vertOffset = 0;
	u32 primCount  = 0;
	u32 primOffset = 0;
};

class MeshObject : public RenderObject {
	AX_RTTI_INFO(MeshObject, RenderObject)
public:
	using CreateDesc = MeshObject_CreateDesc;

	static SPtr<MeshObject>	s_new(const MemAllocRequest& req);
	static SPtr<MeshObject>	s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderMesh	meshData;

	SPtr<StructuredGpuBuffer> meshlet;
	SPtr<StructuredGpuBuffer> meshletVert;
	SPtr<StructuredGpuBuffer> meshletPrim;
	
	AX_INLINE void _uploadToGpu(RenderRequest* req) {
		if (!_needUploadToGpu) return;
		_needUploadToGpu = false;
		onUploadToGpu(req);
	}

protected:
	MeshObject(const CreateDesc& desc) : _assetPath(desc.assetPath) {}
	
	virtual void onUploadToGpu(RenderRequest* req) {}
	
	bool _needUploadToGpu = true;
	String	_assetPath;
};

} // namespace
