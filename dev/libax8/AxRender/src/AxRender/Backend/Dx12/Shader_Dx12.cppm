module;

export module AxRender:Shader_Dx12;
import :Dx12Util;

#if AX_RENDERER_DX12

import :Renderer_Backend;
import :Shader_Backend;

namespace ax {

class ShaderPass_Dx12 : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Dx12, ShaderPass_Backend)
public:
	ShaderPass_Dx12(const CreateDesc& desc);

	ByteArray _vsBytecode;
	ByteArray _psBytecode;
	ByteArray _csBytecode;
};

class Shader_Dx12 : public Shader_Backend {
	AX_RTTI_INFO(Shader_Dx12, Shader_Backend)
public:
	Shader_Dx12(const CreateDesc& desc) : Base(desc) {}

	virtual UPtr<ShaderPass_Backend> onNewPass(const ShaderPass_Backend_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_Dx12>(AX_ALLOC_REQ, desc);
	}	
};

}  namespace

#endif //AX_RENDERER_DX12
