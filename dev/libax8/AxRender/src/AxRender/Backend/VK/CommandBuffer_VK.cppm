module;

export module AxRender:CommandBuffer_VK;
export import :AX_Vulkan;
export import :CommandBuffer;
export import :Renderer_Backend;
export import :RenderRequest_Backend;

#if AX_RENDERER_VK

namespace ax::AxRender {

class CommandBuffer_VK : public CommandBuffer {
	AX_RTTI_INFO(CommandBuffer_VK, CommandBuffer)
public:
	CommandBuffer_VK& create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queue);

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

	AX_VkCommandPool	_pool;
	AX_VkCommandBuffer	_cmdBuf;
};

}

#endif // AX_RENDERER_VK