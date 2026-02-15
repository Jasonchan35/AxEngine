module;

export module AxRender:LightObject;
export import :RenderDataType;
export import :RenderObjectTable;

export namespace ax {

class LightObject : public RenderObject {
	AX_RTTI_INFO(LightObject, RenderObject)
public:
	static SPtr<LightObject> s_new(const MemAllocRequest& req) { return SPtr<LightObject>(new(req) LightObject()); }
	
	using ResourceKey = Int;
	const ResourceKey* resourceKey() const { return nullptr; }
	
	static NameId s_gpuBufferName() { return AX_NAMEID("axGpuData_LightObject"); }
	using GpuData = AxGpuData_LightObject;
	const GpuData* onGetGpuData(RenderRequest* req) { return &_gpuData; }
	
	void setWorldPos(const Vec3f& v);
	void setColor(const Color3f& color);

protected:
	LightObject();
	GpuData    _gpuData = {};
	
public:
	using ObjectSlot = RenderObjectSlot<This>;
	ObjectSlot objectSlot;
};

void LightObject::setWorldPos(const Vec3f& v) {
	if (_gpuData.worldPos == v) return;
	_gpuData.worldPos = v; 
	objectSlot.markDirty();
}

void LightObject::setColor(const Color3f& color) {
	if (_gpuData.color == color) return;
	_gpuData.color = color;
	objectSlot.markDirty();
}

LightObject::LightObject()
: objectSlot(this) {
	setColor(Color3f(1,1,1));
}

} // namespace
