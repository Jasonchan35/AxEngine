module AxRender;

#if AX_RENDERER_VK

import :RenderPass_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :GpuBuffer_Vk;
import :RenderRequest_Vk;

namespace ax /*::AxRender*/ {


RenderPassColorBuffer_Vk::RenderPassColorBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

	if (desc.fromBackBuffer) {
		auto* renderContext_vk = rttiCast<RenderContext_Vk>(desc.fromBackBuffer.renderContext);
		if (!renderContext_vk) throw Error_Undefined();

		auto* backBuffer = renderContext_vk->_getBackBuffer(desc.fromBackBuffer.index);
		if (!backBuffer) throw Error_Undefined();

		auto& image = backBuffer->_vkImage;
		if (image == VK_NULL_HANDLE) throw Error_Undefined();
		
		_image.createFromBackBuffer(dev, image, desc.frameSize, desc.colorType);
	} else {

		_image.createImage2D(dev, desc.frameSize, desc.colorType, 1);
		_mem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

#if AX_RENDER_DEBUG_NAME
		_image.setDebugName(Fmt("{}-image",  desc.name));
		_mem.setDebugName(Fmt("{}-devMem", desc.name));
#endif
	}

	_view.create(_image);

#if AX_RENDER_DEBUG_NAME
	_view.setDebugName(Fmt("{}-view", desc.name));
#endif
}

RenderPassDepthBuffer_Vk::RenderPassDepthBuffer_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

	_image.createDepthStencil(dev, desc.frameSize, desc.depthType);
	_mem.createForImage(_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_view.create(_image);

#if AX_RENDER_DEBUG_NAME
	_image.setDebugName(Fmt("{}-image", desc.name));
	_mem.setDebugName(Fmt("{}-mem",   desc.name));
	_view.setDebugName(Fmt("{}-view",  desc.name));
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
	if (desc.fromBackBuffer) {
		renderContext_vk	= rttiCastCheck<RenderContext_Vk>(desc.fromBackBuffer.renderContext);
		backBuffer_vk		= renderContext_vk->_getBackBuffer(desc.fromBackBuffer.index);
	}

//---- color buffers ----
	const Int colorBufferCount = desc.colorAttachmentDescs.size();
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& colorAttach = _colorAttachments.emplaceBack();
		colorAttach.desc = desc.colorAttachmentDescs[i];
		
		if (desc.fromBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			colorAttach.buffer = backBuffer_vk->_colorBuf_vk;

		} else {
			RenderPassColorBuffer_CreateDesc colorBuf_createDesc;
			colorBuf_createDesc.name      = FmtName("{}-color", desc.name);
			colorBuf_createDesc.colorType = colorAttach.desc.colorType;

			colorAttach.buffer = RenderPassColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);
		}

		auto* newColorBuf_vk = rttiCastCheck<RenderPassColorBuffer_Vk>(colorAttach.buffer.ptr());
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
			newAttachDesc_vk.loadOp			= AX_VkUtil::getVkLoadOp(colorAttach.desc.loadOp);
			newAttachDesc_vk.storeOp		= VK_ATTACHMENT_STORE_OP_STORE;
			newAttachDesc_vk.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			newAttachDesc_vk.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			newAttachDesc_vk.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			if (colorAttach.desc.loadOp == RenderBufferLoadOp::Clear) {
				newAttachDesc_vk.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			} else {
				newAttachDesc_vk.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
			}
		}
	}

//--- depth buffer
	_depthAttachment.desc = desc.depthAttachmentDesc;
	bool hasDepth = _depthAttachment.isEnabled();
	if (hasDepth) {
		if (desc.fromBackBuffer) {
			_depthAttachment.buffer = renderContext_vk->_depthBuf_vk;
			
		} else {
			RenderPassDepthBuffer_CreateDesc depthBufDesc;
			depthBufDesc.name = Fmt("{}-depth", desc.name);
			depthBufDesc.frameSize = desc.frameSize;
			depthBufDesc.depthType = desc.depthAttachmentDesc.depthType;

			_depthAttachment.buffer = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
		}

		auto* depthBuffer_vk = rttiCastCheck<RenderPassDepthBuffer_Vk>(_depthAttachment.buffer.ptr()); 

		imageViews.emplaceBack(depthBuffer_vk->_view.handle());
		auto depthFormat = AX_VkUtil::getVkDepthType(depthBuffer_vk->depthType());

		depthAttachRef_vk.attachment = AX_VkUtil::castUInt32(attachDescList_vk.size());
		depthAttachRef_vk.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		{
			auto& dst			= attachDescList_vk.emplaceBack();
			dst.format			= depthFormat;
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= AX_VkUtil::getVkLoadOp(desc.depthAttachmentDesc.loadOp);
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if (desc.depthAttachmentDesc.loadOp == RenderBufferLoadOp::Clear) {
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

#if AX_RENDER_DEBUG_NAME
	_renderPass_vk.setDebugName(Fmt("{}-renderPass", desc.name));
	_framebuffer_vk.setDebugName(Fmt("{}-framebuffer", desc.name));
#endif

}

} // namespace
#endif // AX_RENDERER_VK