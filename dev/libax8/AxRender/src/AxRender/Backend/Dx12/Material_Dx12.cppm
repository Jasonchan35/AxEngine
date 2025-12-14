module;

export module AxRender:Material_Dx12;
import :Dx12Util;

#if AX_RENDERER_DX12

import :Renderer_Backend;
import :Material_Backend;
import :Shader_Dx12;

namespace ax {

class MaterialPass_Dx12 : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Dx12, MaterialPass_Backend)
public:
	

	void bind	(ID3D12GraphicsCommandList* cmdList, DrawCall& drawCall);
	void bind	(ID3D12GraphicsCommandList* cmdList, ComputeCall& computeCall);

	void onSetShaderPass(ShaderPass* shaderPass);

	ShaderPass_Dx12*	shaderPass() { return rttiCastCheck<axDX12ShaderPass>(Base::shaderPass()); }

private:
	void _bindParams(ID3D12GraphicsCommandList* cmdList);

	axDX12DescripterHeap_CBV_SRV_UAV	_texDescHeap;
	axDX12DescripterHeap_Sampler		_samplerDescHeap;
	axDX12DescripterHeap_CBV_SRV_UAV	_storageBufDescHeap;

};

class Material_Dx12 : public axMaterial {
	AX_RTTI_INFO(Material_Dx12, axMaterial)
public:
	

	Material_Dx12() = default;

	axDX12Shader*	shader() { return rttiCastCheck<axDX12Shader>(Base::shader()); }

	void bind	(ID3D12GraphicsCommandList* cmdList, DrawCall&		drawCall);
	void bind	(ID3D12GraphicsCommandList* cmdList, ComputeCall&	computeCall);
};
} // namespace

#endif //#if AX_RENDERER_DX12
