module;

module AxRender;

#if AX_RENDERER_VK
import :GpuBuffer_Vk;
import :RenderSystem_Vk;
import :RenderContext_Vk;
import :RenderRequest_Vk;

namespace ax /*::AxRender*/ {

GpuBuffer_Vk::GpuBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto* sys = RenderSystem_Vk::s_instance();

	VkBufferUsageFlags usage        = 0;
	VmaMemoryUsage     vmaUsage     = VMA_MEMORY_USAGE_AUTO;
	AX_VkSparseBuffer* sparseBuffer = nullptr;

	switch (desc.bufferType) {
		case GpuBufferType::Vertex: {
			usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			// memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			sparseBuffer = sys->sparseVertexBuffer();
		}break;
		case GpuBufferType::Index: {
			usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			// memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			sparseBuffer = sys->sparseIndexBuffer();
		}break;
		case GpuBufferType::Const: {
			usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			// memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		}break;
		case GpuBufferType::Structured: {
			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//			memProps = VK_MEMORY_PROPERTY_PROTECTED_BIT; // no CPU access
//			memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		}break;

		// https://gpuopen.com/learn/vulkan-device-memory/
		case GpuBufferType::StagingToGpu: {
			usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			vmaUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		}break;
		case GpuBufferType::StagingToCpu: {
			usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			// memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			vmaUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
		}break;

		default: throw Error_Undefined();
	}

	_vkBuf.create(sys->device(), AX_VkUtil::castVkDeviceSize(desc.bufferSize), 0, usage, vmaUsage, sparseBuffer);
#if AX_RENDER_DEBUG_NAME
	if (!sparseBuffer) {
		_vkBuf.setDebugName(desc.name);
	}
#endif
}

void GpuBuffer_Vk::onFlush(IntRange range) {
	switch (bufferType()) {
		case GpuBufferType::StagingToGpu: {
			_vkBuf.flushMappedMemoryRanges(range);
		} break;

		case GpuBufferType::StagingToCpu: {
			_vkBuf.invalidateMappedMemoryRanges(range);
		} break;

		default: AX_ASSERT(false);
	}
}

void GpuBuffer_Vk::onCopyFromGpuBuffer(RenderRequest* req_, GpuBuffer* src_, IntRange srcRange, Int dstOffset) {
	if (!_vkBuf) { AX_ASSERT(false); return; }
	auto* src = rttiCastCheck<GpuBuffer_Vk>(src_);
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);

	if (!src   ) { AX_ASSERT(false); return; }
	if (!req   ) { AX_ASSERT(false); return; }

	auto* rc = req->renderContext_vk();
	_vkBuf.bindSparse(rc->graphQueue_vk().handle(), VK_NULL_HANDLE);

	auto srcOffset  = srcRange.start();
	auto sizeToCopy = srcRange.size();
	auto dstRange   = Range_StartAndSize(dstOffset, sizeToCopy);

	if (!bufferRange().contains(dstRange))
		throw Error_Undefined();

	VkBufferCopy region = {};
	region.dstOffset = AX_VkUtil::castUInt32(dstOffset + _vkBuf.sparseOffset());
	region.srcOffset = AX_VkUtil::castUInt32(srcOffset);
	region.size      = AX_VkUtil::castUInt32(sizeToCopy);
	
	vkCmdCopyBuffer(req->_uploadCmdList_vk, *src, _vkBuf, 1, &region);
}

} // namespace
#endif // AX_RENDERER_VK
