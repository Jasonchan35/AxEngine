module;

export module AxRender:RenderResourceManager_Vk;
#if AX_RENDERER_VK

export import :RenderResourceManager_Backend;
export import :RenderSystem_Backend;
export import :AX_Vulkan;

namespace ax {

class RenderResourceManager_Vk : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Vk, RenderResourceManager_Backend)
public:
	using BindSpace = ShaderParamBindSpace;

	RenderResourceManager_Vk(const CreateDesc& desc) : Base(desc) {}

	virtual void onPostCreate() override;
	
#if AX_RENDER_BINDLESS
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Sampler_Backend  >>& list) override;
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Texture2D_Backend>>& list) override;

	VkDescriptorSet		_bindlessDescriptorSet = nullptr;
	AX_VkDescriptorPool	_bindlessDescriptorPool;
#endif

};

}

#endif // #if AX_RENDERER_VK