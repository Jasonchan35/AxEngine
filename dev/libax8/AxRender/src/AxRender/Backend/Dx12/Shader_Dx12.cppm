module;

export module AxRender:Shader_Dx12;
import :Dx12Util;

#if AX_RENDERER_DX12

import :Renderer_Backend;
import :Shader_Backend;

namespace ax {

class ShaderPipeline_Dx12 : public NonCopyable {
public:
	struct Key {
		VertexLayout        vertexLayout   = nullptr;
		RenderPrimitiveType primitiveType  = RenderPrimitiveType::None;
		bool                debugWireframe = false;

		bool operator==(const Key& r) const {
			return vertexLayout		== r.vertexLayout
				&& primitiveType	== r.primitiveType
				&& debugWireframe	== r.debugWireframe;
		}
	};

	Key	key;
	ComPtr<ID3D12PipelineState> pipelineState;
};

class ShaderPass_Dx12 : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Dx12, ShaderPass_Backend)
public:
	ShaderPass_Dx12(const CreateDesc& desc);

	template<class FUNC>
	void _visitStage(FUNC func) {
		func(_vsStage, ShaderStageFlags::Vertex);
		func(_csStage, ShaderStageFlags::Pixel);
		func(_gsStage, ShaderStageFlags::Geometry);
		func(_csStage, ShaderStageFlags::Compute);
	}

	struct Stage {
		ByteArray _bytecode;
	};

	Stage _vsStage;
	Stage _psStage;
	Stage _gsStage;
	Stage _csStage;
};

class Shader_Dx12 : public Shader_Backend {
	AX_RTTI_INFO(Shader_Dx12, Shader_Backend)
public:
	Shader_Dx12(const CreateDesc& desc) : Base(desc) {}

	virtual UPtr<ShaderPass_Backend> onNewPass(const ShaderPass_Backend_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_Dx12>(AX_ALLOC_REQ, desc);
	}	
};

} //  namespace

#endif //AX_RENDERER_DX12
