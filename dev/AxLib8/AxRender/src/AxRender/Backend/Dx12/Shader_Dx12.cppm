module;

export module AxRender:Shader_Dx12;
import :Dx12Util;

#if AX_RENDERER_DX12

export import :Shader_Backend;
export import :RenderSystem_Backend;
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
	ShaderParamSpace_Dx12(const CreateDesc& desc);
	
	Dx12DescriptorTable         _CBV_SRV_UAV_DescTable;
	Dx12DescriptorTable         _samplerDescTable;
};

class ShaderPass_Dx12 : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Dx12, ShaderPass_Backend)
public:
	using Pipeline = ShaderPipeline_Dx12;
	using BindSpace = ShaderParamBindSpace;
	
	using PsoKey = Pipeline::PsoKey;
	
	ShaderPass_Dx12(const CreateDesc& desc);

	template<class PSO_DESC> void _commonPsoDesc(RenderRequest_Dx12* req, PSO_DESC& psoDesc);
	
	Pipeline* getOrAddGraphicsPipeline(RenderRequest_Dx12* req, AxDrawCallDesc& cmd);
	Pipeline* _createVertexShaderPipeline(RenderRequest_Dx12* req, AxDrawCallDesc& cmd, PsoKey& posKey);
	Pipeline* _createMeshShaderPipeline(RenderRequest_Dx12* req, AxDrawCallDesc& cmd, PsoKey& posKey);
	
	bool _bindPipeline(RenderRequest_Dx12* req, AxDrawCallDesc& cmd) const;
	void _createRootSignature(Dx12RootParameterList& rootParamList);

	const ShaderParamSpace_Dx12* getParamSpace_dx12(BindSpace s) const {
		return rttiCastCheck<ShaderParamSpace_Dx12>(getParamSpace(s));
	}

	ShaderParamSpace_Dx12* getOwnParamSpace_dx12(BindSpace s) {
		return rttiCastCheck<ShaderParamSpace_Dx12>(getOwnParamSpace(s));
	}
	
	template<class FUNC>
	void _visitStages(FUNC func) {
		func(_vertexStage  , ShaderStageFlags::Vertex);
		func(_pixelStage   , ShaderStageFlags::Pixel);
		func(_geometryStage, ShaderStageFlags::Geometry);
		func(_computeStage , ShaderStageFlags::Compute);
		func(_meshStage    , ShaderStageFlags::Mesh);
	}

	struct Stage {
		FileMemMap bytecode;
	};

	Stage _vertexStage;
	Stage _pixelStage;
	Stage _geometryStage;
	Stage _computeStage;
	Stage _meshStage;

	struct RootParamBinding {
		RootParamBinding(Dx12RootParamType rootParamType_, BindSpace bindSpace_)
			: rootParamType(rootParamType_), bindSpace(bindSpace_) {}
		
		Dx12RootParamType rootParamType = Dx12RootParamType::None;
		BindSpace         bindSpace     = BindSpace::Invalid;
	};

	Array<RootParamBinding, 16> _rootParamBindings;
	Dx12RootParameterList       _pipelineRootParamList;
	Array<UPtr<Pipeline>, 4>    _pipelineTable;
	ComPtr<ID3D12RootSignature> _rootSignature;
};

class Shader_Dx12 : public Shader_Backend {
	AX_RTTI_INFO(Shader_Dx12, Shader_Backend)
public:
	Shader_Dx12(const CreateDesc& desc) : Base(desc) {}
	
	virtual UPtr<ShaderPass_Backend> onNewPass(const ShaderPass_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_Dx12>(AX_NEW, desc);
	}	
};

} //  namespace

#endif //AX_RENDERER_DX12
