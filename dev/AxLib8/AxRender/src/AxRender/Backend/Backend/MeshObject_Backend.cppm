module;

export module AxRender:MeshObject_Backend;
import :MeshObject;
import :RenderObjectSlot_Backend;

export namespace ax {

class MeshObject_Backend : public MeshObject {
	AX_RTTI_INFO(MeshObject_Backend, MeshObject)
public:
	using ResourceKey = String;
	const ResourceKey& resourceKey() const { return _assetPath; }
	
	MeshObject_Backend(const CreateDesc& desc) 
	: MeshObject(desc)
	, objectSlot(this, false) {}


	struct GpuData {
		static NameId s_name() { return AX_NAMEID("axMeshInfo"); }
		AxMeshInfo meshInfo;
	};

	GpuData* onGetGpuData(RenderRequest* req);
	GpuData        _gpuData;
	
	RenderObjectSlot_Backend<This>	objectSlot;
};

}// namespace
 