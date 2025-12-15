module;

export module AxRender:RenderPass_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :RenderPass_Backend;

export namespace ax /*::AxRender*/ {

class RenderPass_Vk : public RenderPass_Backend {
	AX_RTTI_INFO(RenderPass_Vk, RenderPass_Backend)
public:	
	RenderPass_Vk(const CreateDesc& desc);

	AX_VkRenderPass		_renderPass_vk;
	AX_VkFramebuffer	_framebuffer_vk;
};

} // namespace

#endif // AX_RENDERER_VK