module;

export module AxRender:RenderObjectManager_Vk;
#if AX_RENDER_VK

export import :RenderObjectManager_Backend;
export import :RenderSystem_Backend;
export import :AX_Vulkan;

namespace ax {

class RenderObjectManager_Vk : public RenderObjectManager_Backend {
	AX_RTTI_INFO(RenderObjectManager_Vk, RenderObjectManager_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE()
	
	using BindSpace = ShaderParamBindSpace;

	RenderObjectManager_Vk(const CreateDesc& desc) : Base(desc) {}

	virtual void onPostCreate() override;

#if AX_RENDER_BINDLESS
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Sampler  >>& list) override;
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Texture2D>>& list) override;

	VkDescriptorSet		_bindlessDescriptorSet = nullptr;
	AX_VkDescriptorPool	_bindlessDescriptorPool;
#endif

};

}

#endif // #if AX_RENDER_VK