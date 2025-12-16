module AxRender;

#if AX_RENDERER_VK

import :RenderPass_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :GpuBuffer_Vk;
import :RenderRequest_Vk;

namespace ax /*::AxRender*/ {


RenderPassColorBuffer_Vk::RenderPassColorBuffer_Vk(const CreateDesc& createDesc) 
: Base(createDesc)
{
	auto& dev       = Renderer_Vk::s_instance()->device();
	auto  frameSize = AX_VkUtil::castVkExtent2D(createDesc.frameSize);
	auto  format    = AX_VkUtil::getVkColorType(createDesc.attachment.colorType);

	if (createDesc.backBufferRef) {
		auto* renderContext_vk = rttiCast<RenderContext_Vk>(createDesc.backBufferRef.renderContext);
		if (!renderContext_vk) throw Error_Undefined();

		auto* backBuffer = renderContext_vk->_getBackBuffer(createDesc.backBufferRef.index);
		if (!backBuffer) throw Error_Undefined();

		auto& image = backBuffer->_vkImage;
		if (image == VK_NULL_HANDLE) throw Error_Undefined();
		
		_image.createFromBackBuffer(dev, image, frameSize, format);
	} else {

		_image.createImage2D(dev, frameSize, format, 1,
								VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
								VK_IMAGE_LAYOUT_UNDEFINED);

		_mem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

#if AX_DEBUG_NAME
		_image.setDebugName(Fmt("{}-image",  createDesc.name));
		_mem.setDebugName(Fmt("{}-devMem", createDesc.name));
#endif
	}

	_view.create(_image);

#if AX_DEBUG_NAME
	_view.setDebugName(Fmt("{}-view", createDesc.name));
#endif
}

RenderPassDepthBuffer_Vk::RenderPassDepthBuffer_Vk(const CreateDesc& createDesc) 
: Base(createDesc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

	auto frameSize = AX_VkUtil::castVkExtent2D(createDesc.frameSize);
	auto format    = AX_VkUtil::getVkDepthType(createDesc.attachment.depthType);
	_image.createDepthStencil(dev, frameSize, format);
	_mem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_view.create(_image);

#if AX_DEBUG_NAME
	_image.setDebugName(Fmt("{}-image", createDesc.name));
	_mem.setDebugName(Fmt("{}-mem",   createDesc.name));
	_view.setDebugName(Fmt("{}-view",  createDesc.name));
#endif
}


RenderPass_Vk::RenderPass_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

	Array<VkImageView, 16>				imageViews;

	Array<VkAttachmentDescription, 16>	attachDescList_vk;
	Array<VkAttachmentReference,16>		colorAttachRefList_vk;
	VkAttachmentReference				depthAttachRef_vk = {};

	RenderContext_Vk*					renderContext_vk = nullptr;
	RenderContext_Vk::BackBuffer_Vk*	backBuffer_vk    = nullptr;
	if (desc.isBackBuffer) {
		renderContext_vk	= rttiCastCheck<RenderContext_Vk>(desc.renderContext);
		backBuffer_vk		= renderContext_vk->_getBackBuffer(desc.backBufferIndex);
	}

//---- color buffers ----
	const Int colorBufferCount = desc.colorBufferAttachments.size();
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& newColorBuffer = _colorBuffers.emplaceBack();
		newColorBuffer.attachment = desc.colorBufferAttachments[i];
		
		if (desc.isBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			newColorBuffer.buffer = backBuffer_vk->_colorBuf_vk;

		} else {
			RenderPassColorBuffer_CreateDesc colorBuf_createDesc;
			colorBuf_createDesc.name = Fmt("{}-color", desc.name);
			colorBuf_createDesc.attachment = newColorBuffer.attachment;

			newColorBuffer.buffer = RenderPassColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);
		}

		auto* newColorBuf_vk = rttiCastCheck<RenderPassColorBuffer_Vk>(newColorBuffer.buffer.ptr());
		AX_ASSERT(newColorBuf_vk);

		auto viewHandle = newColorBuf_vk->_view.handle();
		AX_ASSERT(viewHandle);
		imageViews.emplaceBack(viewHandle);

		auto& newColorAttachRef_vk = colorAttachRefList_vk.emplaceBack();

		newColorAttachRef_vk.attachment = AX_VkUtil::castUInt32(attachDescList_vk.size());
		newColorAttachRef_vk.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		{
			auto& newAttachDesc_vk = attachDescList_vk.emplaceBack();
			newAttachDesc_vk.format			= AX_VkUtil::getVkColorType(newColorBuf_vk->colorType());
			newAttachDesc_vk.flags			= 0;
			newAttachDesc_vk.samples		= VK_SAMPLE_COUNT_1_BIT;
			newAttachDesc_vk.loadOp			= AX_VkUtil::getVkLoadOp(newColorBuffer.attachment.loadOp);
			newAttachDesc_vk.storeOp		= VK_ATTACHMENT_STORE_OP_STORE;
			newAttachDesc_vk.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			newAttachDesc_vk.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			newAttachDesc_vk.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			if (newColorBuffer.attachment.loadOp == RenderBufferLoadOp::Clear) {
				newAttachDesc_vk.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			} else {
				newAttachDesc_vk.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
			}
		}
	}

//--- depth buffer
	_depthBuffer.attachment = desc.depthBufferAttachment;
	bool hasDepth = _depthBuffer.attachment.isEnabled();
	if (hasDepth) {
		if (desc.isBackBuffer) {
			_depthBuffer.buffer = renderContext_vk->_depthBuf_vk;
			
		} else {
			RenderPassDepthBuffer_CreateDesc depthBufDesc;
			depthBufDesc.name = Fmt("{}-depth", desc.name);
			depthBufDesc.frameSize = desc.frameSize;
			depthBufDesc.attachment = desc.depthBufferAttachment;

			_depthBuffer.buffer = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
		}

		auto* depthBuffer_vk = rttiCastCheck<RenderPassDepthBuffer_Vk>(_depthBuffer.buffer.ptr()); 

		imageViews.emplaceBack(depthBuffer_vk->_view.handle());
		auto depthFormat = AX_VkUtil::getVkDepthType(depthBuffer_vk->depthType());

		depthAttachRef_vk.attachment = AX_VkUtil::castUInt32(attachDescList_vk.size());
		depthAttachRef_vk.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		{
			auto& dst			= attachDescList_vk.emplaceBack();
			dst.format			= depthFormat;
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= AX_VkUtil::getVkLoadOp(desc.depthBufferAttachment.loadOp);
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if (desc.depthBufferAttachment.loadOp == RenderBufferLoadOp::Clear) {
				dst.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			} else {
				dst.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
		}
	}
		
	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.flags = 0;
	subpassDesc.inputAttachmentCount		= 0;
	subpassDesc.pInputAttachments			= nullptr;
	subpassDesc.colorAttachmentCount		= 1;
	subpassDesc.pColorAttachments			= colorAttachRefList_vk.data();
	subpassDesc.pResolveAttachments			= nullptr;
	subpassDesc.pDepthStencilAttachment		= hasDepth ? &depthAttachRef_vk : nullptr;
	subpassDesc.preserveAttachmentCount		= 0;
	subpassDesc.pPreserveAttachments		= nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext				= nullptr;
	renderPassCreateInfo.flags				= 0;
	renderPassCreateInfo.attachmentCount	= AX_VkUtil::castUInt32(attachDescList_vk.size());
	renderPassCreateInfo.pAttachments		= attachDescList_vk.data();
	renderPassCreateInfo.subpassCount		= 1;
	renderPassCreateInfo.pSubpasses			= &subpassDesc;
	renderPassCreateInfo.dependencyCount	= 0;
	renderPassCreateInfo.pDependencies		= nullptr;

	_renderPass_vk.create(dev, renderPassCreateInfo);
	_framebuffer_vk.create(dev, _renderPass_vk, imageViews, AX_VkUtil::castVkExtent2D(desc.frameSize));

#if AX_DEBUG_NAME
	_renderPass_vk.setDebugName(Fmt("{}-renderPass", desc.name));
	_framebuffer_vk.setDebugName(Fmt("{}-framebuffer", desc.name));
#endif

}

} // namespace
#endif // AX_RENDERER_VK