module;

export module AxRender:MeshObject;
export import :RenderMesh;

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
	
	struct Buffers {
		StructuredGpuBuffer_<AxMeshInfo>    meshInfo;
		StructuredGpuBuffer_<AxMeshlet>     meshlet;
		StructuredGpuBuffer_<AxMeshletVert> meshletVert;
		StructuredGpuBuffer_<AxMeshletPrim> meshletPrim;
		
		void create();
		void _uploadToGpu(MeshObject* meshObj, RenderRequest* req);
	} buffers;
	
	AX_INLINE void _uploadToGpu(RenderRequest* req) {
		if (!_needUploadToGpu) return;
		_needUploadToGpu = false;
		buffers._uploadToGpu(this, req);
	}

protected:
	MeshObject(const CreateDesc& desc);

	bool   _needUploadToGpu = true;
	String _assetPath;
};

} // namespace
