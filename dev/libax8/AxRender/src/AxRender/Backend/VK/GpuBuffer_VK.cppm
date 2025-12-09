module;

export module AxRender:GpuBuffer_VK;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :GpuBuffer_Backend;

export namespace ax::AxRender {

class GpuBuffer_VK : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_VK, GpuBuffer_Backend)
public:
	GpuBuffer_VK(const CreateDesc& desc);

	virtual MutByteSpan	onMapMemory(IntRange range) override	{ return _vkDevMem.mapMemory(range); }
	virtual void		onUnmapMemory() override				{ return _vkDevMem.unmapMemory(); }

	virtual void		onFlush(IntRange range) override;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) override;

	AX_VkBuffer& vkBuf()			{ return _vkBuf; }
	const VkBuffer&  vkBufHandle()	{ return _vkBuf; }

	AX_VkDeviceMemory& vkDevMem()		{ return _vkDevMem; }
	   VkDeviceMemory  vkDevMemHandle() { return _vkDevMem; }

	operator AX_VkBuffer&()			{ return _vkBuf; }
	operator    VkBuffer()			{ return _vkBuf; }

	operator AX_VkDeviceMemory&()	{ return _vkDevMem; }
	operator    VkDeviceMemory()	{ return _vkDevMem; }

private:
	AX_VkBuffer			_vkBuf;
	AX_VkDeviceMemory	_vkDevMem;
};


class RenderColorBuffer_VK : public RenderColorBuffer_Backend {
	AX_RTTI_INFO(RenderColorBuffer_VK, RenderColorBuffer_Backend)
public:
	RenderColorBuffer_VK(const CreateDesc& desc);

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

class RenderDepthBuffer_VK : public RenderDepthBuffer_Backend {
	AX_RTTI_INFO(RenderDepthBuffer_VK, RenderDepthBuffer_Backend)
public:
	RenderDepthBuffer_VK(const CreateDesc& desc);

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