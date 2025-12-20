module;

export module AxRender:Material_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :Material_Backend;
export import :Shader_Vk;


export namespace ax /*::AxRender*/ {

class RenderRequest_Vk;

class MaterialParamSpace_Vk : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_Vk, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_Vk(const CreateDesc& desc);

	VkDescriptorSet getUpdatedDescriptorSet(RenderRequest_Vk* req);
	VkDescriptorSet getLastDescriptorSet();

	const ShaderParamSpace_Vk* shaderParamSpace() const { return rttiCastCheck<ShaderParamSpace_Vk>(_shaderParamSpace.ptr()); }

private:
	VkDescriptorSet	_currentDescriptorSet() { return _descriptorSets[_currentDescriptorSetsIndex]; }
	void _nextDescriptorSet  (RenderRequest_Vk* req, const ShaderParamSpace_Vk* shaderParamSpace);
	void _updateDescriptorSet(RenderRequest_Vk* req, const ShaderParamSpace_Vk* shaderParamSpace);

	Array<VkDescriptorBufferInfo, 16> _updateUniformBufferInfos;
	Array<VkDescriptorBufferInfo, 16> _updateStorageBufferInfos;
	Array<VkDescriptorImageInfo,  16> _updateSamplerInfos;
	Array<VkDescriptorImageInfo,  16> _updateTextureInfos;

	Array<VkWriteDescriptorSet,   64> _updateWriteDescriptorSets;

	static constexpr Int kMaxRenderRequestCount = AxRenderConfig::kMaxRenderRequestCount;
	Array<VkDescriptorSet, kMaxRenderRequestCount>	_descriptorSets;


	AX_VkDescriptorPool		_descriptorPool;

	RenderSeqId	_lastUpdateRenderSeqId = 0;
	Int			_currentDescriptorSetsIndex = 0;
}; 



class MaterialPass_Vk : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Vk, MaterialPass_Backend)
public:
	MaterialPass_Vk(const CreateDesc& desc);

	const ShaderPass_Vk* shaderPass_vk() const { return rttiCastCheck<ShaderPass_Vk>(shaderPass()); }

	virtual bool onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) override;
};

class Material_Vk : public Material_Backend {
	AX_RTTI_INFO(Material_Vk, Material_Backend)
public:
	Material_Vk(const CreateDesc& desc) : Base(desc) {}

private:
	using Util = AX_VkUtil;

	Shader_Vk* shader_vk() { return static_cast<Shader_Vk*>(_shader_backend.ptr()); }

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_Backend_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Vk>(AX_ALLOC_REQ, desc);
	}
};

} // namespace
#endif // AX_RENDERER_VK