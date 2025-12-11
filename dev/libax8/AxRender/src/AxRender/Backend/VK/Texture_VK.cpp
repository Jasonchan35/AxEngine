module AxRender;

#if AX_RENDERER_VK

import :Texture_VK;
import :RenderContext_VK;
import :RenderRequest_VK;
import :ImageIO;
import :GpuBuffer_VK;

namespace ax::AxRender {

void Sampler_VK::onCreate(const CreateDesc& desc) {
	Base::onCreate(desc);

	auto& ss = desc.samplerState;

	VkSamplerCreateInfo info = {};
	info.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.pNext						= nullptr;
	info.magFilter					= AX_VkUtil::getVkSamplerFilter(ss.filter);
	info.minFilter					= AX_VkUtil::getVkSamplerFilter(ss.filter);
	info.mipmapMode					= AX_VkUtil::getVkSamplerMipmapMode(ss.filter);
	info.addressModeU				= AX_VkUtil::getVkSamplerWrap(ss.wrapU);
	info.addressModeV				= AX_VkUtil::getVkSamplerWrap(ss.wrapV);
	info.addressModeW				= AX_VkUtil::getVkSamplerWrap(ss.wrapW);
	info.mipLodBias					= 0.0f;
	info.anisotropyEnable			= VK_FALSE;
	info.maxAnisotropy				= 1;
	info.compareOp					= VK_COMPARE_OP_NEVER;
	info.minLod						= ss.minLOD;
	info.maxLod						= ss.maxLOD;
	info.borderColor				= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	info.unnormalizedCoordinates	= VK_FALSE;

	auto& dev = Renderer_VK::s_instance()->device();
	_sampler.create(dev, info);
}

void Texture2D_VK::onImageIO_ReadHandler(ImageIO_ReadHandler& handler) {
	auto& info = handler.desc.info;
	_info = info;

	auto& dev = Renderer_VK::s_instance()->device();
	_image.createImage2D(	dev,
							AX_VkUtil::castVkExtent2D(info.size.xy()),
							AX_VkUtil::getVkColorType(info.colorType),
							AX_VkUtil::castUInt32(    info.mipLevels),
							VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
							VK_IMAGE_LAYOUT_PREINITIALIZED);

	_devMem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_view.create(_image);

	auto dataSize = handler.desc.dataSize;

	_uploadBuffer = GpuBuffer_Backend::s_new(AX_ALLOC_REQ, "Texture2D_VK-upload", GpuBufferType::StagingToGpu, dataSize);

	auto map = _uploadBuffer->mapMemory(IntRange(dataSize));
	handler.readPixelsTo(map.data());
}

void Texture2D_VK::_bindImage(RenderRequest_VK* req, VkDescriptorImageInfo& outInfo) {

	if (auto* uploadBuf = rttiCast<GpuBuffer_VK>(_uploadBuffer.ptr())) {
		auto& cmdBuf = req->uploadCmdBuf_vk();

		u32 width  = AX_VkUtil::castUInt32(_info.size.x);
		u32 height = AX_VkUtil::castUInt32(_info.size.y);

		VkBufferImageCopy region = {};
		region.bufferOffset						= 0;
		region.bufferRowLength					= width;
		region.bufferImageHeight				= height;
		region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel		= 0;
		region.imageSubresource.baseArrayLayer	= 0;
		region.imageSubresource.layerCount		= 1;
		region.imageOffset.x					= 0;
		region.imageOffset.y					= 0;
		region.imageOffset.z					= 0;
		region.imageExtent.width				= width;
		region.imageExtent.height				= height;
		region.imageExtent.depth				= 1;

		_image.setLayout(cmdBuf, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT);

		vkCmdCopyBufferToImage(cmdBuf, uploadBuf->vkBufHandle(), _image, _image.layout(), 1, &region);

		_image.setLayout(	cmdBuf,
							VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
							VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

		req->resourcesToKeep.add(uploadBuf);

		_uploadBuffer = nullptr;
	}

//----
	req->resourcesToKeep.add(this);

	outInfo.imageView	= _view;
	outInfo.imageLayout	= _image.layout();
	outInfo.sampler		= VK_NULL_HANDLE;
}

} // namespace
#endif // AX_RENDERER_VK