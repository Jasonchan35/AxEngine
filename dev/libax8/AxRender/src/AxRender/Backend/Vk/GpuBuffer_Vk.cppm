module;

export module AxRender:GpuBuffer_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :GpuBuffer_Backend;

export namespace ax /*::AxRender*/ {

class GpuBuffer_Vk : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Vk, GpuBuffer_Backend)
public:
	GpuBuffer_Vk(const CreateDesc& desc);

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

} // namespace

#endif // AX_RENDERER_VK