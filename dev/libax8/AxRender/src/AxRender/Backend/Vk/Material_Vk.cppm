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
	MaterialParamSpace_Vk(const CreateDesc& desc) : Base(desc) {}

	const ShaderParamSpace_Vk* shaderParamSpace() const { return rttiCastCheck<ShaderParamSpace_Vk>(_shaderParamSpace.ptr()); }
}; 

class MaterialPass_Vk : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Vk, MaterialPass_Backend)
public:
	using ConstBufferParam   = MaterialParamSpace_Backend::ConstBufferParam;
	using SamplerParam       = MaterialParamSpace_Backend::SamplerParam;
	using TextureParam       = MaterialParamSpace_Backend::TextureParam;
	using StorageBufferParam = MaterialParamSpace_Backend::StorageBufferParam;
	
	MaterialPass_Vk(const CreateDesc& desc);

	const ShaderPass_Vk* shaderPass_vk() const { return rttiCastCheck<ShaderPass_Vk>(shaderPass()); }

	virtual bool onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) override;

	static constexpr Int kMaxRenderRequestCount = AxRenderConfig::kMaxRenderRequestCount;

	struct PerFrameData : public NonCopyable {
		FixedArray<VkDescriptorSet, BindSpace_COUNT> descSets;
		AX_VkDescriptorPool pool;

		void create(MaterialPass_Vk* pass, RenderRequest_Vk* req);
		void update(MaterialPass_Vk* pass, RenderRequest_Vk* req);
	};

	PerFrameData& getUpdatedFrameData(RenderRequest_Vk* req);
private:
	PerFrameData& _currentFrameData() { return _perFrameData[_currentFrameDataIndex]; }
	void _nextFrameData(RenderRequest_Vk* req);

	Array<PerFrameData, kMaxRenderRequestCount>	_perFrameData;

	RenderSeqId	_lastUpdateRenderSeqId = 0;
	Int			_currentFrameDataIndex = 0;
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