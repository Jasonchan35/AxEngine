module;

export module AxRender:Material_Vk;

#if AX_RENDER_VK
export import :AX_Vulkan;
export import :Material_Backend;
export import :Shader_Vk;
export import :Texture_Vk;

export namespace ax /*::AxRender*/ {

class RenderRequest_Vk;
class MaterialPass_Vk;

class MaterialParamSpace_Vk : public MaterialParamSpace_Backend {
	AX_RTTI_INFO(MaterialParamSpace_Vk, MaterialParamSpace_Backend)
public:
	MaterialParamSpace_Vk(const CreateDesc& desc) : Base(desc) {}

	struct PerFrameData : public NonCopyable {
		VkDescriptorSet _descSet = VK_NULL_HANDLE;
	};

	const ShaderParamSpace_Vk* shaderParamSpace_vk() const { return rttiCastCheck<ShaderParamSpace_Vk>(_shaderParamSpace.ptr()); }

	void onUpdatePerFrameData(Int currentIndex, RenderRequest_Backend* req, PerFrameData& data);
	RenderPerFrameDataSet_<This, PerFrameData>	_perFrameDataSet {this};
}; 

class MaterialPass_Vk : public MaterialPass_Backend {
	AX_RTTI_INFO(MaterialPass_Vk, MaterialPass_Backend)
public:
	using ConstBufferParam      = MaterialParamSpace_Backend::ConstBufferParam;
	using SamplerParam          = MaterialParamSpace_Backend::SamplerParam;
	using TextureParam          = MaterialParamSpace_Backend::TextureParam;
	using StructuredBufferParam = MaterialParamSpace_Backend::StructuredBufferParam;
	
	MaterialPass_Vk(const CreateDesc& desc);

	const ShaderPass_Vk* shaderPass_vk() const { return rttiCastCheck<ShaderPass_Vk>(shaderPass()); }

	virtual bool onBindMaterial(RenderRequest* req, AxVertexShaderDraw& draw, AxVertexShaderDraw_RootConst* rootConst) override;
	virtual bool onBindMaterial(RenderRequest* req, AxMeshShaderDraw  & draw, AxMeshShaderDraw_RootConst  * rootConst) override;

	bool _onBindMaterial(RenderRequest_Vk* req, ByteSpan rootConstData);
	
	const MaterialParamSpace_Vk* getParamSpace_vk(BindSpace bs) const {
		return rttiCastCheck<MaterialParamSpace_Vk>(getParamSpace(bs));
	}

	MaterialParamSpace_Vk* getOwnParamSpace_vk(BindSpace bs) {
		return rttiCastCheck<MaterialParamSpace_Vk>(getOwnParamSpace(bs));
	}
};

class Material_Vk : public Material_Backend {
	AX_RTTI_INFO(Material_Vk, Material_Backend)
public:
	Material_Vk(const CreateDesc& desc) : Base(desc) {}

private:
	using Util = AX_VkUtil;

	Shader_Vk* shader_vk() { return static_cast<Shader_Vk*>(_shader_backend.ptr()); }

	virtual UPtr<MaterialPass_Backend> onNewPass(const MaterialPass_CreateDesc& desc) override {
		return UPtr_new<MaterialPass_Vk>(AX_NEW, desc);
	}
};

} // namespace
#endif // AX_RENDER_VK