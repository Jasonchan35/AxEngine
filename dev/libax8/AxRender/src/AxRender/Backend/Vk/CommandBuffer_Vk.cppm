module;

export module AxRender:CommandBuffer_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :CommandBuffer_Backend;
export import :Renderer_Backend;
export import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {

class CommandBuffer_Vk : public CommandBuffer_Backend {
	AX_RTTI_INFO(CommandBuffer_Vk, CommandBuffer_Backend)
public:
	void create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queue);

	AX_VkCommandBuffer* operator->() { return &_cmdBuf; }
	const VkCommandBuffer& handle() { return _cmdBuf.handle(); }

	operator AX_VkCommandBuffer&()	{ return _cmdBuf; }
	operator VkCommandBuffer()		{ return _cmdBuf.handle(); }

	void commandBegin();
	void commandEnd();

#if AX_RENDER_DEBUG_NAME
	void setDebugName(const String& name) {
		_pool.setDebugName(Fmt("{}-pool",		name));
		_cmdBuf.setDebugName(Fmt("{}-cmdBuf",	name));
	}
#endif // AX_RENDER_DEBUG_NAME

	void resetAndReleaseResource() {
		_cmdBuf.resetAndReleaseResource();
	}
	
protected:
	AX_VkCommandPool	_pool;
	AX_VkCommandBuffer	_cmdBuf;
};

}

#endif // AX_RENDERER_VK