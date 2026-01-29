module;

module AxRender;

#if AX_RENDERER_VK
import :GpuBuffer_Vk;
import :RenderSystem_Vk;
import :RenderContext_Vk;
import :RenderRequest_Vk;
import :RenderObjectManager_Vk;

namespace ax /*::AxRender*/ {

GpuBufferPool_Vk::GpuBufferPool_Vk(const CreateDesc& desc): Base(desc) {
	auto& dev = RenderSystem_Vk::s_instance()->device();
	_vkBuf.create(dev, desc.bufferType, desc.maxSize, true);
	_vkBuf.setDebugName(desc.name);
	_alignment = desc.alignment ? desc.alignment : s_getMinAlignement(desc.bufferType);
	_pagePool.create(desc);
}

Int GpuBufferPool_Vk::s_getMinAlignement(GpuBufferType type) {
	auto& limits = RenderSystem_Vk::s_instance()->device().physicalDevice()->props().limits;

	switch (type) {
		case GpuBufferType::Const		: return limits.minUniformBufferOffsetAlignment;
		case GpuBufferType::Vertex		: return 64;;
		case GpuBufferType::Index		: return 16;
		case GpuBufferType::StagingToGpu: return limits.minStorageBufferOffsetAlignment;
		case GpuBufferType::StagingToCpu: return limits.minStorageBufferOffsetAlignment;
		case GpuBufferType::Structured	: return limits.minStorageBufferOffsetAlignment;
		default: return 16;
	}
}

void GpuBufferPool_Vk::onGpuUpdatePages(RenderRequest_Backend* req_) {
	if (_pagePool._pendingCommitPages.size() <= 0) return;
	
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	auto& cmdQueue = req->renderContext_vk()->graphQueue_vk();

	Array<VkSparseMemoryBind, 8> sparseMemoryBindList;
	
	for (auto& pageIndex : _pagePool._pendingCommitPages) {
		auto& page = _pagePool._pages[pageIndex];
		Int resourceOffset = ax_safe_cast_from(pageIndex * _pageSize);
		
		page._devMem.createForVirtualMemPage(_vkBuf, _pageSize);
		
		auto& bind = sparseMemoryBindList.emplaceBack();
		bind.resourceOffset = resourceOffset;
		bind.size           = _pageSize;
		bind.memory         = page._devMem;
		bind.memoryOffset   = 0;
		bind.flags          = 0;
	}
	_pagePool._pendingCommitPages.clear();
	
	VkSparseBufferMemoryBindInfo bufferBindInfo = {};
	bufferBindInfo.buffer    = _vkBuf;
	bufferBindInfo.bindCount = ax_safe_cast_from(sparseMemoryBindList.size());
	bufferBindInfo.pBinds    = sparseMemoryBindList.data();
	
	VkBindSparseInfo bindInfo = {};
	bindInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
	bindInfo.bufferBindCount = 1;
	bindInfo.pBufferBinds = &bufferBindInfo;

	VkFence fence = VK_NULL_HANDLE;
	auto err = vkQueueBindSparse(cmdQueue.handle(), 1, &bindInfo, fence);
	AX_VkUtil::throwIfError(err);
	
}

GpuBuffer_Vk::GpuBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	if (_pool) {
		auto* pool = rttiCastCheck<GpuBufferPool_Vk>(_pool);
		_vkBufHandle = pool->_vkBuf.handle();
 
	} else {
		auto& dev = RenderSystem_Vk::s_instance()->device();
		_vkBufWithoutPool.create(dev, desc.bufferType, desc.bufferSize, false);
		_vkBufWithoutPool.setDebugName(desc.name);
		_vkBufHandle = _vkBufWithoutPool.handle();
	}
}

void GpuBuffer_Vk::onFlush(IntRange range) {
	switch (_type) {
		case GpuBufferType::StagingToGpu: {
			_getVkBuf().flushMappedMemoryRanges(range);
		} break;

		case GpuBufferType::StagingToCpu: {
			_getVkBuf().invalidateMappedMemoryRanges(range);
		} break;

		default: AX_ASSERT(false);
	}
}

void GpuBuffer_Vk::onCopyFromGpuBuffer(RenderRequest* req_, GpuBuffer* src_, IntRange srcRange, Int dstOffset) {
	if (!_vkBufHandle) { AX_ASSERT(false); return; }
	auto* src = rttiCastCheck<GpuBuffer_Vk>(src_);
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);

	if (!src   ) { AX_ASSERT(false); return; }
	if (!req   ) { AX_ASSERT(false); return; }

	auto srcOffset  = srcRange.start();
	auto sizeToCopy = srcRange.size();
	auto dstRange   = Range_StartAndSize(dstOffset, sizeToCopy);

	if (!IntRange(_size).contains(dstRange))
		throw Error_Undefined();

	VkBufferCopy region = {};
	region.dstOffset = AX_VkUtil::castUInt32(dstOffset + _bufferOffset);
	region.srcOffset = AX_VkUtil::castUInt32(srcOffset + src->bufferOffset());
	region.size      = AX_VkUtil::castUInt32(sizeToCopy);
	
	vkCmdCopyBuffer(req->_uploadCmdList_vk, src->vkBufHandle(), _vkBufHandle, 1, &region);
}

} // namespace
#endif // AX_RENDERER_VK
