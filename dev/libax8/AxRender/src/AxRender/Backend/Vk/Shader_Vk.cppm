module;

export module AxRender:Shader_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :RenderRequest_Backend;
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

	PsoKey					key;
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

	void createLayout_vk();
	
	AX_VkDescriptorSetLayout	_layout_vk;
};

class ShaderPass_Vk : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_Vk, ShaderPass_Backend)
public:
	using Pipeline = ShaderPipeline_Vk;

	ShaderPass_Vk(const CreateDesc& desc);

	Pipeline* getOrAddPipeline(const Pipeline::PsoKey& key);

	bool _bindPipeline(class RenderRequest_Vk* req, Cmd_DrawCall& cmd) const;

	const AX_VkPipelineLayout& pipelineLayout() const { return _pipelineLayout; }

	ShaderPass_Vk* getCommonPass_vk() { return rttiCastCheck<ShaderPass_Vk>(getCommonPass()); }

	ShaderParamSpace_Vk* getParamSpace_vk(BindSpace type) {
		return rttiCastCheck<ShaderParamSpace_Vk>(getParamSpace(type)); 
	}
	
	ShaderParamSpace_Vk* getCommonParamSpace_vk(BindSpace type) {
		return rttiCastCheck<ShaderParamSpace_Vk>(getCommonParamSpace(type)); 
	}

private:
	AX_VkPipelineLayout			_pipelineLayout;
	Array<UPtr<Pipeline>, 4>	_pipelineTable;

	template<class FUNC>
	void _visitStages(FUNC func) {
		func(_vsStage, ShaderStageFlags::Vertex);
		func(_psStage, ShaderStageFlags::Pixel);
		func(_gsStage, ShaderStageFlags::Geometry);
		func(_csStage, ShaderStageFlags::Compute);
	}

	struct Stage {
		AX_VkShaderModule	vkShaderModule;
	};

	Stage _vsStage;
	Stage _psStage;
	Stage _gsStage;
	Stage _csStage;
};

class Shader_Vk : public Shader_Backend {
	AX_RTTI_INFO(Shader_Vk, Shader_Backend)
public:
	Shader_Vk(const CreateDesc& desc) : Base(desc) {}

	virtual UPtr<ShaderPass_Backend> onNewPass(const ShaderPass_Backend_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_Vk>(AX_ALLOC_REQ, desc);
	}
};

} // namespace
#endif // AX_RENDERER_VK