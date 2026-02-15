module;

export module AxRender:LightObject;
export import :RenderDataType;
export import :RenderObjectTable;

export namespace ax::AxRender {

class LightObject : public RenderObject {
	AX_RTTI_INFO(LightObject, RenderObject)
public:
	static SPtr<LightObject> s_new(const MemAllocRequest& req) { return SPtr<LightObject>(new(req) LightObject()); }
	
	using ResourceKey = Int;
	const ResourceKey* resourceKey() const { return nullptr; }
	
	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuData_LightObject"); }
	using GpuData = AxGpuData_LightObject;
	const GpuData* onGetGpuData(RenderRequest* req) { return &_gpuData; }
	
protected:
	LightObject() : objectSlot(this) {}
	GpuData    _gpuData = {};
	
public:
	using ObjectSlot = RenderObjectSlot<This>;
	ObjectSlot objectSlot;
};

} // namespace
