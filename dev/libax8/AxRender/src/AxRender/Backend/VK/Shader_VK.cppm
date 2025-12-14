module;

export module AxRender:Shader_VK;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :RenderRequest_Backend;
export import :Shader_Backend;

export namespace ax::AxRender {

class ShaderParamSpace_VK : public ShaderParamSpace_Backend {
	AX_RTTI_INFO(ShaderParamSpace_VK, ShaderParamSpace_Backend)
public:
	ShaderParamSpace_VK(const CreateDesc& desc) : Base(desc) {}

	VkDescriptorSetLayout	createDescriptorSetLayout();


	VkDescriptorSetLayout	descriptorSetLayout() const { return _descriptorSetLayout.handle(); }

private:
	AX_VkDescriptorSetLayout	_descriptorSetLayout;
};


class ShaderPipeline_VK : public NonCopyable {
public:
	struct Key {
		VertexLayout  vertexLayout	 = nullptr;
		RenderPrimitiveType primitiveType	 = RenderPrimitiveType::None;
		bool		  debugWireframe = false;
		VkRenderPass  renderPass	 = VK_NULL_HANDLE;

		bool operator==(const Key& r) const {
			return vertexLayout		== r.vertexLayout
				&& primitiveType	== r.primitiveType
				&& debugWireframe	== r.debugWireframe
				&& renderPass		== r.renderPass;
		}
	};

	Key					key;
	AX_VkPipelineCache	pipelineCache;
	AX_VkPipeline		pipeline;
};

struct VertexInputLayoutDesc_VK {
	bool init(const ShaderStageInfo& info, VertexLayout vertexLayout);

	VkPipelineVertexInputStateCreateInfo	vertexInputState = {};
	Array<VkVertexInputBindingDescription,		1>		bindingDesc;
	Array<VkVertexInputAttributeDescription,	64>		attrDesc;
};

class ShaderPass_VK : public ShaderPass_Backend {
	AX_RTTI_INFO(ShaderPass_VK, ShaderPass_Backend)
public:
	using Pipeline = ShaderPipeline_VK;

	ShaderPass_VK(const CreateDesc& desc);

	ShaderPipeline_VK* getOrAddPipeline(const Pipeline::Key& key);

	bool _bindPipeline(class RenderRequest_VK* req, Cmd_DrawCall& cmd) const;

	const AX_VkPipelineLayout& pipelineLayout() const { return _pipelineLayout; }

private:
	AX_VkPipelineLayout			_pipelineLayout;
	Array<UPtr<Pipeline>, 4>	_pipelineTable;

	template<class FUNC>
	void _visitModules(FUNC func) {
		func(_vsModule, ShaderStageFlags::Vertex);
		func(_csModule, ShaderStageFlags::Pixel);
		func(_gsModule, ShaderStageFlags::Geometry);
		func(_csModule, ShaderStageFlags::Compute);
	}

	AX_VkShaderModule	_vsModule;
	AX_VkShaderModule	_psModule;
	AX_VkShaderModule	_gsModule;
	AX_VkShaderModule	_csModule;
};

class Shader_VK : public Shader_Backend {
	AX_RTTI_INFO(Shader_VK, Shader_Backend)
public:
	Shader_VK(const CreateDesc& desc) : Base(desc) {}

	virtual UPtr<ShaderPass_Backend> onNewPass(const ShaderPass_Backend_CreateDesc& desc) override {
		return UPtr_new<ShaderPass_VK>(AX_ALLOC_REQ, desc);
	}
};

} // namespace
#endif // AX_RENDERER_VK