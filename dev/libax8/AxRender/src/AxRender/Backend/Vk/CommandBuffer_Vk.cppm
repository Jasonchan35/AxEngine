module;

export module AxRender:CommandBuffer_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :CommandBuffer;
export import :Renderer_Backend;
export import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {

class CommandBuffer_Vk : public CommandBuffer {
	AX_RTTI_INFO(CommandBuffer_Vk, CommandBuffer)
public:
	CommandBuffer_Vk& create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queue);

	AX_RENDER_CommandBuffer_FunctionInterfaces(override)

	AX_VkCommandBuffer* operator->() { return &_cmdBuf; }
	const VkCommandBuffer& handle() { return _cmdBuf.handle(); }

	operator AX_VkCommandBuffer&()	{ return _cmdBuf; }
	operator VkCommandBuffer()		{ return _cmdBuf.handle(); }

#if AX_DEBUG_NAME
	void setDebugName(const String& name) {
		_pool.setDebugName(Fmt("{}-pool",		name));
		_cmdBuf.setDebugName(Fmt("{}-cmdBuf",	name));
	}
#endif // AX_DEBUG_NAME

	void resetAndReleaseResource() {
		_cmdBuf.resetAndReleaseResource();
	}
	
protected:
	AX_VkCommandPool	_pool;
	AX_VkCommandBuffer	_cmdBuf;
};

}

#endif // AX_RENDERER_VK