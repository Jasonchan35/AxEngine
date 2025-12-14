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
};

class MaterialPass_Dx12 : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Dx12, MaterialPass_Backend)
public:
	MaterialPass_Dx12(const CreateDesc& desc) : Base(desc) {}
	virtual bool onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) override { AX_ASSERT_TODO; return false; }
	
private:
	Dx12DescripterHeap_CBV_SRV_UAV	_texDescHeap;
	Dx12DescripterHeap_Sampler		_samplerDescHeap;
	Dx12DescripterHeap_CBV_SRV_UAV	_storageBufDescHeap;
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
