module;

module AxRender;

#if AX_RENDERER_VK
import :RenderTarget_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :RenderRequest_Vk;

namespace ax /*::AxRender*/ {

RenderTargetColorBuffer_Vk::RenderTargetColorBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();
	auto frameSize = AX_VkUtil::castVkExtent2D(desc.size);
	auto format = AX_VkUtil::getVkColorType(desc.colorType);

	if (desc.backBufferRef) {
		auto* ctx = rttiCast<RenderContext_Vk>(desc.backBufferRef.renderContext);
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

RenderTargetDepthBuffer_Vk::RenderTargetDepthBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

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
