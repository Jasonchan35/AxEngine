module;

export module AxRender:MeshObject_Backend;
import :MeshObject;
import :RenderObjectTable;

export namespace ax {

class MeshObject_Backend : public MeshObject {
	AX_RTTI_INFO(MeshObject_Backend, MeshObject)
public:
	using ResourceKey = String;
	const ResourceKey& resourceKey() const { return _assetPath; }
	
	MeshObject_Backend(const CreateDesc& desc) 
	: MeshObject(desc)
	, objectSlot(this, false) {}


	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuMeshObject"); }
	using GpuData = AxGpuMeshObject;
	AxGpuMeshObject  _gpuMeshObject;
	GpuData* onGetGpuData(RenderRequest* req);
	
	RenderObjectSlot<This>	objectSlot;
};

}// namespace
 