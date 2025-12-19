module;

export module AxRender:Shader_Dx12;
import :Dx12Util;

#if AX_RENDERER_DX12

export import :Shader_Backend;
export import :Renderer_Backend;
export import :RenderRequest_Dx12;

namespace ax {

class ShaderPipeline_Dx12 : public NonCopyable {
public:
	struct PsoKey {
		VertexLayout        vertexLayout   = nullptr;
		RenderPrimitiveType primitiveType  = RenderPrimitiveType::None;
		bool                debugWireframe = false;
		// TODO: add render pass colorType and depthType as part of the key

		bool operator==(const PsoKey& r) const {
			return vertexLayout		== r.vertexLayout
				&& primitiveType	== r.primitiveType
				&& debugWireframe	== r.debugWireframe;
		}
	};

	PsoKey	key;
	ComPtr<ID3D12PipelineState> pipelineState;
};

struct VertexInputLayoutDesc_Dx12 {
	bool init(const ShaderStageInfo& info, VertexLayout vertexLayout);
	Array<D3D12_INPUT_ELEMENT_DESC, 64>	desc_dx12;
};

class ShaderParamSpace_Dx12 : public ShaderParamSpace_Backend {
	AX_RTTI_INFO(ShaderParamSpace_Dx12, ShaderParamSpace_Backend)
public:
	ShaderParamSpace_Dx12(const CreateDesc& desc) : Base(desc) {}

	Dx12DescriptorTable	_constBufferDescTable;
	Dx12DescriptorTable	_textureDescTable;
	Dx12DescriptorTable	_samplerDescTable;
	
	void createDescTable();
};

class ShaderPass_Dx12 : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Dx12, ShaderPass_Backend)
public:
	using Pipeline = ShaderPipeline_Dx12;
	using BindSpace = ShaderParamBindSpace;
	
	ShaderPass_Dx12(const CreateDesc& desc);

	Pipeline* getOrAddPipeline(RenderRequest_Dx12* req, const Pipeline::PsoKey& key);
	
	bool _bindPipeline(RenderRequest_Dx12* req, Cmd_DrawCall& cmd) const;
	void _createRootSignature();

	ShaderParamSpace_Dx12* getParamSpace_dx12(BindSpace type) {
		return rttiCastCheck<ShaderParamSpace_Dx12>(getParamSpace(type));
	}

	ShaderParamSpace_Dx12* getCommonParamSpace_dx12(BindSpace type) {
		return rttiCastCheck<ShaderParamSpace_Dx12>(getCommonParamSpace(type));
	}
	
	
	template<class FUNC>
	void _visitStages(FUNC func) {
		func(_vsStage, ShaderStageFlags::Vertex);
		func(_psStage, ShaderStageFlags::Pixel);
		func(_gsStage, ShaderStageFlags::Geometry);
		func(_csStage, ShaderStageFlags::Compute);
	}

	struct Stage {
		FileMemMap bytecode;
	};

	Stage _vsStage;
	Stage _psStage;
	Stage _gsStage;
	Stage _csStage;

	using DescTableRootIndices = FixedArray<UINT, ax_enum_int(BindSpace::_COUNT)>;

	DescTableRootIndices        _constBufferDescTableRootIndices;
	DescTableRootIndices        _textureDescTableRootIndices;
	DescTableRootIndices        _samplerDescTableRootIndices;
	
	Dx12RootParameterList       _pipelineRootParamList;
	Array<UPtr<Pipeline>, 4>    _pipelineTable;
	ComPtr<ID3D12RootSignature> _rootSignature;
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
