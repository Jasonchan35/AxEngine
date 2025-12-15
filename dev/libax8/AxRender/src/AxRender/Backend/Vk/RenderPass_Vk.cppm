module;

export module AxRender:RenderPass_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :RenderPass_Backend;

export namespace ax /*::AxRender*/ {


class RenderPassColorBuffer_Vk : public RenderPassColorBuffer_Backend {
	AX_RTTI_INFO(RenderPassColorBuffer_Vk, RenderPassColorBuffer_Backend)
public:
	RenderPassColorBuffer_Vk(const CreateDesc& createDesc);

#if AX_DEBUG_NAME
	virtual void onSetDebugName(const String& name) override {
		_image.setDebugName(Fmt("{}-image", name));
		_mem.setDebugName(Fmt("{}-mem",   name));
		_view.setDebugName(Fmt("{}-view",  name));
	}
#endif

	AX_VkImage			_image;
	AX_VkDeviceMemory	_mem;
	AX_VkImageView		_view;
};

class RenderPassDepthBuffer_Vk : public RenderPassDepthBuffer_Backend {
	AX_RTTI_INFO(RenderPassDepthBuffer_Vk, RenderPassDepthBuffer_Backend)
public:
	RenderPassDepthBuffer_Vk(const CreateDesc& createDesc);

#if AX_DEBUG_NAME
	virtual void onSetDebugName(const String& name) override {
		_image.setDebugName(Fmt("{}-image", name));
		_mem.setDebugName(Fmt("{}-mem",   name));
		_view.setDebugName(Fmt("{}-view",  name));
	}
#endif

	AX_VkImage			_image;
	AX_VkDeviceMemory	_mem;
	AX_VkImageView		_view;
};


class RenderPass_Vk : public RenderPass_Backend {
	AX_RTTI_INFO(RenderPass_Vk, RenderPass_Backend)
public:	
	RenderPass_Vk(const CreateDesc& desc);

	AX_VkRenderPass		_renderPass_vk;
	AX_VkFramebuffer	_framebuffer_vk;
};

} // namespace

#endif // AX_RENDERER_VK