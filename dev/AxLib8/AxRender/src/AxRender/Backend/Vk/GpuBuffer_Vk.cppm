module;

export module AxRender:GpuBuffer_Vk;

#if AX_RENDERER_VK
export import :AX_Vulkan;
export import :GpuBuffer_Backend;
export import :RenderRequest_Vk;

export namespace ax /*::AxRender*/ {

class GpuBufferPool_Vk : public GpuBufferPool_Backend {
	AX_RTTI_INFO(GpuBufferPool_Vk, GpuBufferPool_Backend)
public:
	GpuBufferPool_Vk(const CreateDesc& desc) : Base(desc) {}
};

class GpuBuffer_Vk : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Vk, GpuBuffer_Backend)
public:
	GpuBuffer_Vk(const CreateDesc& desc);

	virtual MutByteSpan onMapMemory(IntRange range) override	{ return _vkBuf.mapMemory(range); }
	virtual void		onUnmapMemory() override				{ return _vkBuf.unmapMemory(); }

	virtual void		onFlush(IntRange range) override;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) override;

	const VkBuffer&	vkBufHandle() const { return _vkBuf.handle(); }

	operator AX_VkBuffer&()			{ return _vkBuf; }
	operator    VkBuffer()			{ return _vkBuf; }

	VkDeviceSize sparseOffset() const { return _vkBuf.sparseOffset(); }
	
	VkDescriptorBufferInfo _getUpdatedDescriptorInfo(RenderRequest_Vk* req) const {
		VkDescriptorBufferInfo info = {};
		info.buffer = _vkBuf.handle();
		info.offset = sparseOffset();
		info.range  = ax_safe_cast_from(_size);
		return info;
	}

private:

	AX_VkBuffer	_vkBuf;
};

class GpuStructuredBuffer_Vk : public GpuStructuredBuffer_Backend {
	AX_RTTI_INFO(GpuStructuredBuffer_Vk, GpuStructuredBuffer_Backend)
public:
	GpuStructuredBuffer_Vk(const CreateDesc& desc) : Base(desc) {}
};



} // namespace

#endif // AX_RENDERER_VK