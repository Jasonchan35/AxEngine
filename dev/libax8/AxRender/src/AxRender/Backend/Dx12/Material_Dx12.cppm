module;

export module AxRender:Material_Dx12;
import :Dx12DescripterHeap;

#if AX_RENDERER_DX12

import :Renderer_Backend;
import :Material_Backend;
import :Shader_Dx12;

namespace ax {

class MaterialParamSpace_Dx12 : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_Dx12, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_Dx12(const CreateDesc& desc) : Base(desc) {}

	const ShaderParamSpace_Dx12* shaderParamSpace() const { return rttiCastCheck<ShaderParamSpace_Dx12>(_shaderParamSpace.ptr()); }

	void _onDrawcall(RenderRequest_Dx12* req, const ShaderPass_Dx12* shdPass);

protected:
	virtual bool onSetParam(SamplerParam& param, Int index, Sampler* sampler) override;

private:
	Dx12DescripterHeap_Sampler		_samplerDescHeap;
	Dx12DescripterHeap_CBV_SRV_UAV	_texDescHeap;
	Dx12DescripterHeap_CBV_SRV_UAV	_storageBufDescHeap;
};

class MaterialPass_Dx12 : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Dx12, MaterialPass_Backend)
public:
	MaterialPass_Dx12(const CreateDesc& desc) : Base(desc) {}
	
	const ShaderPass_Dx12* shaderPass_dx12() const { return rttiCastCheck<ShaderPass_Dx12>(shaderPass()); }
	
	virtual bool onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) override;
};

class Material_Dx12 : public Material_Backend {
	AX_RTTI_INFO(Material_Dx12, Material_Backend)
public:
	Material_Dx12(const CreateDesc& desc) : Base(desc) {}

	Shader_Dx12*	shader_dx12() { return rttiCastCheck<Shader_Dx12>(Base::shader()); }

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_Backend_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Dx12>(AX_ALLOC_REQ, desc);
	}
};

} // namespace

#endif //#if AX_RENDERER_DX12
