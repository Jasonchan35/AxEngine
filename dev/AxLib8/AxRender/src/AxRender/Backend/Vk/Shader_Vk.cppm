module;

export module AxRender:Shader_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :RenderRequest_Vk;
export import :Shader_Backend;

export namespace ax /*::AxRender*/ {

class ShaderPipeline_Vk : public NonCopyable {
public:
	struct PsoKey {
		VertexLayout  vertexLayout	 = nullptr;
		RenderPrimitiveType primitiveType	 = RenderPrimitiveType::None;
		VkRenderPass  renderPass	 = VK_NULL_HANDLE;

		bool operator==(const PsoKey& r) const {
			return vertexLayout		== r.vertexLayout
				&& primitiveType	== r.primitiveType
				&& renderPass		== r.renderPass;
		}
	};

	PsoKey				key;
	AX_VkPipelineCache	pipelineCache;
	AX_VkPipeline		pipeline;
};

struct VertexInputLayoutDesc_Vk {
	bool init(const ShaderStageInfo& info, VertexLayout vertexLayout);

	VkPipelineVertexInputStateCreateInfo	vertexInputState = {};
	Array<VkVertexInputBindingDescription,		1>		bindingDesc;
	Array<VkVertexInputAttributeDescription,	64>		attrDesc;
};

class ShaderParamSpace_Vk : public ShaderParamSpace_Backend {
	AX_RTTI_INFO(ShaderParamSpace_Vk, ShaderParamSpace_Backend)
public:
	ShaderParamSpace_Vk(const CreateDesc& desc) : Base(desc) {}
	AX_VkDescriptorSetLayout _descSetLayout_vk;
};

class ShaderPass_Vk : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Vk, ShaderPass_Backend)
public:
	using Pipeline = ShaderPipeline_Vk;
	using PsoKey   = Pipeline::PsoKey;

	ShaderPass_Vk(const CreateDesc& desc);

	Pipeline*		getOrAddGraphicsPipeline(RenderRequest_Vk* req, AxDrawCallDesc& cmd);
	UPtr<Pipeline>	_createGraphicsPipeline(RenderRequest_Vk* req, AxDrawCallDesc& cmd, PsoKey& psoKey);
	
	bool _bindPipeline(RenderRequest_Vk* req, AxDrawCallDesc& cmd) const;

	const AX_VkPipelineLayout& pipelineLayout() const { return _pipelineLayout; }

	const ShaderParamSpace_Vk* getParamSpace_vk(BindSpace bs) const {
		return rttiCastCheck<ShaderParamSpace_Vk>(getParamSpace(bs)); 
	}

	ShaderParamSpace_Vk* getOwnParamSpace_vk(BindSpace bs) {
		return rttiCastCheck<ShaderParamSpace_Vk>(getOwnParamSpace(bs)); 
	}
	
	Array<VkDescriptorSetLayout, BindSpace_COUNT>	_allLayouts_vk;
	Int _ownDescSetCount = 0;

private:
	AX_VkPipelineLayout			_pipelineLayout;
	Array<UPtr<Pipeline>, 4>	_pipelineTable;

	template<class FUNC>
	void _visitStages(FUNC func) {
		func(_vertexStage       , ShaderStageFlags::Vertex);
		func(_pixelStage        , ShaderStageFlags::Pixel);
		func(_geometryStage     , ShaderStageFlags::Geometry);
		func(_computeStage      , ShaderStageFlags::Compute);
		func(_meshStage         , ShaderStageFlags::Mesh);
		func(_amplificationStage, ShaderStageFlags::Amplification);
	}

	struct Stage {
		AX_VkShaderModule	vkShaderModule;
	};

	Stage _vertexStage;
	Stage _pixelStage;
	Stage _geometryStage;
	Stage _computeStage;
	Stage _meshStage;
	Stage _amplificationStage;
};

class Shader_Vk : public Shader_Backend {
	AX_RTTI_INFO(Shader_Vk, Shader_Backend)
public:
	Shader_Vk(const CreateDesc& desc) : Base(desc) {}

	virtual UPtr<ShaderPass_Backend> onNewPass(const ShaderPass_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_Vk>(AX_NEW, desc);
	}
};

} // namespace
#endif // AX_RENDERER_VK