module;

#if AX_RENDERER_VK

export module AxRender:RenderPass_VK;
export import :AX_Vulkan;
export import :RenderPass_Backend;

export namespace ax::AxRender {

class RenderPass_VK : public RenderPass_Backend {
	AX_RTTI_INFO(RenderPass_VK, RenderPass_Backend)
public:	
	RenderPass_VK(const CreateDesc& desc);

	AX_VkRenderPass		_renderPass;
	AX_VkFramebuffer	_framebuffer;
};

} // namespace

#endif // AX_RENDERER_VK