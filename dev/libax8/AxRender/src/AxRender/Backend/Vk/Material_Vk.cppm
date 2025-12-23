module;

export module AxRender:Material_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :Material_Backend;
export import :Shader_Vk;


export namespace ax /*::AxRender*/ {

class RenderRequest_Vk;
class MaterialPass_Vk;

class MaterialParamSpace_Vk : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_Vk, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_Vk(const CreateDesc& desc) : Base(desc) {}

	struct PerFrameData : public NonCopyable {
		VkDescriptorSet _descSet;
	};

	const PerFrameData& getUpdatedFrameData(RenderRequest_Vk* req) const {
		return ax_const_cast(this)->_getUpdatedFrameData(req);
	}
	
	const ShaderParamSpace_Vk* shaderParamSpace_vk() const { return rttiCastCheck<ShaderParamSpace_Vk>(_shaderParamSpace.ptr()); }

private:
	PerFrameData& _currentFrameData() { return _perFrameData[_currentFrameDataIndex]; }
	void		  _updateFrameData(RenderRequest_Vk* req, PerFrameData& frameData);
	PerFrameData& _getUpdatedFrameData(RenderRequest_Vk* req);
	
	Array<PerFrameData, AxRenderConfig::kMaxRenderRequestCount>	_perFrameData;
	RenderSeqId	_lastUpdateRenderSeqId = 0;
	Int			_currentFrameDataIndex = 0;
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

	struct PerFrameData : public NonCopyable {
		Array<VkDescriptorSet, BindSpace_COUNT>  _allDescSets;

		void update(MaterialPass_Vk* pass, RenderRequest_Vk* req);
	};

	PerFrameData& getUpdatedFrameData(RenderRequest_Vk* req);

	const MaterialParamSpace_Vk* getParamSpace_vk(BindSpace bs) const {
		return rttiCastCheck<MaterialParamSpace_Vk>(getParamSpace(bs));
	}

	MaterialParamSpace_Vk* getOwnParamSpace_vk(BindSpace bs) {
		return rttiCastCheck<MaterialParamSpace_Vk>(getOwnParamSpace(bs));
	}

	AX_VkDescriptorPool& descPool() {
		if (!_descPool) _createDescPool();
		return _descPool;
	}

private:
	void _createDescPool();
	
	AX_VkDescriptorPool _descPool;
	PerFrameData	_perFrameData;

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

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Vk>(AX_ALLOC_REQ, desc);
	}
};

} // namespace
#endif // AX_RENDERER_VK