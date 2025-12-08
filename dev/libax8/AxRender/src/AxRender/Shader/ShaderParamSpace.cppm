module;
export module AxRender:ShaderParamSpace;
export import :RenderObject;
export import :DataType;

export namespace ax::AxRender {

class ShaderParamSpace_CreateDesc : public NonCopyable {
public:
	BindSpace	bindSpace = BindSpace::Invalid;
};

class ShaderParamSpace : public RenderObject {
	AX_RTTI_INFO(ShaderParamSpace, RenderObject)
public:
	using CreateDesc = ShaderParamSpace_CreateDesc;

	BindSpace	bindSpace() const { return _bindSpace; }

	static SPtr<ShaderParamSpace> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	SPtr<class MaterialParamSpace> newMaterialParamSpace(const MemAllocRequest& req) const;


protected:
	ShaderParamSpace(const CreateDesc& desc) : _bindSpace(desc.bindSpace) {}

private:
	BindSpace _bindSpace = BindSpace::Invalid;
};

} // namespace