module;

export module AxRender:CameraObject;
export import :RenderObjectTable;

export namespace ax {

class CameraObject : public RenderObject {
	AX_RTTI_INFO(CameraObject, RenderObject)
public:
	static SPtr<This> s_new(const MemAllocRequest& req) { return SPtr<This>(new(req) This()); }
	
	using ResourceKey = Int;
	const ResourceKey* resourceKey() const { return nullptr; }
	
	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuData_CameraObject"); }
	using GpuData = AxGpuData_CameraObject;
	const GpuData* onGetGpuData(RenderRequest* req) { return &_gpuData; }

	Math::Camera3f camera;
	
protected:
	CameraObject();
	GpuData    _gpuData = {};
	
public:
	using ObjectSlot = RenderObjectSlot<This>;
	ObjectSlot objectSlot;
};

CameraObject::CameraObject()
: objectSlot(this)
{
}

} // namespace
