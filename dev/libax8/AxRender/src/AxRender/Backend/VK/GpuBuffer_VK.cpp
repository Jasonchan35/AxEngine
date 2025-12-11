module;

module AxRender;

#if AX_RENDERER_VK
import :GpuBuffer_VK;
import :Renderer_VK;
import :RenderContext_VK;
import :RenderRequest_VK;

namespace ax::AxRender {

GpuBuffer_VK::GpuBuffer_VK(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_VK::s_instance()->device();

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
		case GpuBufferType::Uniform: {
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

#if AX_DEBUG_NAME
	_vkBuf.setDebugName(desc.name);
	_vkDevMem.setDebugName(Fmt("{}-devMem", desc.name));
#endif
}

void GpuBuffer_VK::onFlush(IntRange range) {
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

void GpuBuffer_VK::onCopyFromGpuBuffer(RenderRequest* req_, GpuBuffer* src_, IntRange srcRange, Int dstOffset) {
	if (!_vkBuf) { AX_ASSERT(false); return; }

	auto* src = rttiCastCheck<GpuBuffer_VK>(src_);
	auto* req = rttiCastCheck<RenderRequest_VK>(req_);

	if (!src   ) { AX_ASSERT(false); return; }
	if (!req   ) { AX_ASSERT(false); return; }

	auto srcOffset  = srcRange.begin();
	auto sizeToCopy = srcRange.size();
	auto dstRange   = IntRange::s_beginSize(dstOffset, sizeToCopy);

	if (!bufferRange().contains(dstRange))
		throw Error_Undefined();

	VkBufferCopy region = {};
	region.dstOffset = AX_VkUtil::castUInt32(dstOffset);
	region.srcOffset = AX_VkUtil::castUInt32(srcOffset);
	region.size      = AX_VkUtil::castUInt32(sizeToCopy);
	
	vkCmdCopyBuffer(req->_uploadCmdBuf_vk, *src, _vkBuf, 1, &region);
}


RenderColorBuffer_VK::RenderColorBuffer_VK(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_VK::s_instance()->device();
	auto frameSize = AX_VkUtil::castVkExtent2D(desc.size);
	auto format = AX_VkUtil::getVkColorType(desc.colorType);

	if (desc.backBufferRef) {
		auto* ctx = rttiCast<RenderContext_VK>(desc.backBufferRef.renderContext);
		if (!ctx) throw Error_Undefined();
		
		auto image = ctx->_getBackBufferImage(desc.backBufferRef.index);
		if (image == VK_NULL_HANDLE) throw Error_Undefined();
		
		_image.createFromBackBuffer(dev, image, frameSize, format);
	} else {

		_image.createImage2D(dev, frameSize, format, 1,
								VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
								VK_IMAGE_LAYOUT_UNDEFINED);

		_mem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

#if AX_DEBUG_NAME
		_image.setDebugName(Fmt("{}-image",  desc.name));
		_mem.setDebugName(Fmt("{}-devMem", desc.name));
#endif
	}

	_view.create(_image);

#if AX_DEBUG_NAME
	_view.setDebugName(Fmt("{}-view", desc.name));
#endif
}

RenderDepthBuffer_VK::RenderDepthBuffer_VK(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_VK::s_instance()->device();

	auto frameSize = AX_VkUtil::castVkExtent2D(desc.frameSize);
	auto format    = AX_VkUtil::getVkDepthType(desc.depthType);
	_image.createDepthStencil(dev, frameSize, format);
	_mem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_view.create(_image);

#if AX_DEBUG_NAME
	_image.setDebugName(Fmt("{}-image", desc.name));
	_mem.setDebugName(Fmt("{}-mem",   desc.name));
	_view.setDebugName(Fmt("{}-view",  desc.name));
#endif
}


} // namespace
#endif // AX_RENDERER_VK
