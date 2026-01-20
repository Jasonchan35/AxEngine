module;

export module AxRender:MeshObject;
export import :RenderMesh;

export namespace ax {

class MeshObject : public RenderObject {
	AX_RTTI_INFO(MeshObject, RenderObject)
public:
	using CreateDesc = MeshObject_CreateDesc;

	static SPtr<MeshObject>	s_new(const MemAllocRequest& req);
	static SPtr<MeshObject>	s_new(const MemAllocRequest& req, const CreateDesc& desc);

	RenderMesh	meshData;
	
protected:
	MeshObject(const CreateDesc& desc) : _assetPath(desc.assetPath) {}

	String	_assetPath;
};

} // namespace
