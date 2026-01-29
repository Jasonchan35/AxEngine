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
	GpuBufferPool_Vk(const CreateDesc& desc);

	struct Page_Vk : public Page {
		AX_VkDeviceMemory	_devMem;
	};
	
	virtual void onAllocateBlock(GpuBuffer* buf) override { _pagePool.onAllocateBlock(buf, _pageSize); }
	virtual void onFreeBlock(GpuBuffer* buf) override { _pagePool.onFreeBlock(buf, _pageSize); }
	
	virtual void onGpuUpdatePages(RenderRequest_Backend* req_) override;

	PagePool_<Page_Vk>	_pagePool;
	AX_VkBuffer			_vkBuf;
};

class GpuBuffer_Vk : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Vk, GpuBuffer_Backend)
public:
	GpuBuffer_Vk(const CreateDesc& desc);

	static Int s_getMinAlignement(GpuBufferType type);

	virtual MutByteSpan onMapMemory(IntRange range) override	{ return _getVkBuf().mapMemory(range); }
	virtual void		onUnmapMemory() override				{ return _getVkBuf().unmapMemory(); }

	virtual void		onFlush(IntRange range) override;
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) override;

	const VkBuffer&	vkBufHandle() const { return _vkBufHandle; }
	
	VkDescriptorBufferInfo _getUpdatedDescriptorInfo(RenderRequest_Vk* req) const {
		VkDescriptorBufferInfo info = {};
		info.buffer = _vkBufHandle;
		info.offset = _bufferOffset;
		info.range  = ax_safe_cast_from(_size);
		return info;
	}

private:
	
	AX_VkBuffer& _getVkBuf() { return _pool ? rttiCastCheck<GpuBufferPool_Vk>(_pool)->_vkBuf : _vkBufWithoutPool; }
	
	AX_VkBuffer	_vkBufWithoutPool;
	VkBuffer	_vkBufHandle = VK_NULL_HANDLE;
};

class StructuredGpuBuffer_Vk : public StructuredGpuBuffer_Backend {
	AX_RTTI_INFO(StructuredGpuBuffer_Vk, StructuredGpuBuffer_Backend)
public:
	StructuredGpuBuffer_Vk(const CreateDesc& desc) : Base(desc) {}
};



} // namespace

#endif // AX_RENDERER_VK