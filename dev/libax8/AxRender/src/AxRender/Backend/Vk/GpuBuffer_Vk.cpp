module;

module AxRender;

#if AX_RENDERER_VK
import :GpuBuffer_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :RenderRequest_Vk;

namespace ax /*::AxRender*/ {

GpuBuffer_Vk::GpuBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

	VkBufferUsageFlags usage = 0;
	VkMemoryPropertyFlags memProps = 0;

	switch (desc.bufferType) {
		case GpuBufferType::Vertex: {
			usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
		}break;
		case GpuBufferType::Index: {
			usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
		}break;
		case GpuBufferType::Const: {
			usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
		}break;
		case GpuBufferType::Storage: {
			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//			memProps = VK_MEMORY_PROPERTY_PROTECTED_BIT; // no CPU access
			memProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Gpu only
		}break;

		// https://gpuopen.com/learn/vulkan-device-memory/
		case GpuBufferType::StagingToGpu: {
			usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}break;
		case GpuBufferType::StagingToCpu: {
			usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		}break;

		default: throw Error_Undefined();
	}

	_vkBuf.create(dev, AX_VkUtil::castVkDeviceSize(desc.bufferSize), usage);
	_vkDevMem.createForBuffer(_vkBuf, memProps);

#if AX_RENDER_DEBUG_NAME
	_vkBuf.setDebugName(desc.name);
	_vkDevMem.setDebugName(Fmt("{}-devMem", desc.name));
#endif
}

void GpuBuffer_Vk::onFlush(IntRange range) {
	switch (bufferType()) {
		case GpuBufferType::StagingToGpu: {
			_vkDevMem.flushMappedMemoryRanges(range);
		} break;

		case GpuBufferType::StagingToCpu: {
			_vkDevMem.InvalidateMappedMemoryRanges(range);
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

	auto srcOffset  = srcRange.start();
	auto sizeToCopy = srcRange.size();
	auto dstRange   = Range_StartAndSize(dstOffset, sizeToCopy);

	if (!bufferRange().contains(dstRange))
		throw Error_Undefined();

	VkBufferCopy region = {};
	region.dstOffset = AX_VkUtil::castUInt32(dstOffset);
	region.srcOffset = AX_VkUtil::castUInt32(srcOffset);
	region.size      = AX_VkUtil::castUInt32(sizeToCopy);
	
	vkCmdCopyBuffer(req->_uploadCmdBuf_vk, *src, _vkBuf, 1, &region);
}

} // namespace
#endif // AX_RENDERER_VK
