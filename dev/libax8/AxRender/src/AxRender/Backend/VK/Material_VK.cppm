module;

#if AX_RENDERER_VK

export module AxRender:Material_VK;
export import :AX_Vulkan;
export import :Material_Backend;
export import :Shader_VK;


export namespace ax::AxRender {

class RenderRequest_VK;

class MaterialParamSpace_VK : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_VK, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_VK(const CreateDesc& desc);

	VkDescriptorSet getUpdatedDescriptorSet(RenderRequest_VK* req);
	VkDescriptorSet getLastDescriptorSet();

	const ShaderParamSpace_VK* shaderParamSpace() const { return rttiCastCheck<ShaderParamSpace_VK>(_shaderParamSpace.ptr()); }

private:
	VkDescriptorSet	_currentDescriptorSet() { return _descriptorSets[_currentDescriptorSetsIndex]; }
	void _nextDescriptorSet  (RenderRequest_VK* req, const ShaderParamSpace_VK* shaderParamSpace);
	void _updateDescriptorSet(RenderRequest_VK* req, const ShaderParamSpace_VK* shaderParamSpace);

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



class MaterialPass_VK : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_VK, MaterialPass_Backend)
public:
	MaterialPass_VK(const CreateDesc& desc);

	virtual bool onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) override;
};

class Material_VK : public Material_Backend {
	AX_RTTI_INFO(Material_VK, Material_Backend)
public:
	Material_VK(const CreateDesc& desc) : Base(desc) {}

private:
	using Util = AX_VkUtil;

	Shader_VK* shader() { return static_cast<Shader_VK*>(_shader.ptr()); }

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_Backend_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_VK>(AX_ALLOC_REQ, desc);
	}
};

} // namespace
#endif // AX_RENDERER_VK