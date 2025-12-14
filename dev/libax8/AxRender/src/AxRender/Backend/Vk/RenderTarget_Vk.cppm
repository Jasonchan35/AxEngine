module;

export module AxRender:RenderTarget_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :RenderTarget_Backend;

export namespace ax /*::AxRender*/ {

class RenderTargetColorBuffer_Vk : public RenderTargetColorBuffer_Backend {
	AX_RTTI_INFO(RenderTargetColorBuffer_Vk, RenderTargetColorBuffer_Backend)
public:
	RenderTargetColorBuffer_Vk(const CreateDesc& desc);

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

class RenderTargetDepthBuffer_Vk : public RenderTargetDepthBuffer_Backend {
	AX_RTTI_INFO(RenderTargetDepthBuffer_Vk, RenderTargetDepthBuffer_Backend)
public:
	RenderTargetDepthBuffer_Vk(const CreateDesc& desc);

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


} // namespace

#endif // AX_RENDERER_VK