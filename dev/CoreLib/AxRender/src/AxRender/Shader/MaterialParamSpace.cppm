module;
export module AxRender.MaterialParamSpace;
export import AxRender.ShaderParamSpace;

export namespace ax::AxRender {

class MaterialParamSpace_CreateDesc : public NonCopyable {
public:
	const ShaderParamSpace*	paramSpace = nullptr;
};

class MaterialParamSpace : public RenderObject {
	AX_RTTI_INFO(MaterialParamSpace, RenderObject)
public:
	using CreateDesc = MaterialParamSpace_CreateDesc;
protected:
	MaterialParamSpace(const CreateDesc& desc) {}

private:
};

} // namespace