module AxRender;

#if AX_RENDERER_VK

import :RenderPass_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :GpuBuffer_Vk;
import :RenderRequest_Vk;
import :RenderTarget_Vk;

namespace ax /*::AxRender*/ {

RenderPass_Vk::RenderPass_Vk(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_Vk::s_instance()->device();

	Array<VkImageView, 16>				frameBufferAttachments;

	Array<VkAttachmentDescription, 16>	renderPassAttachmentDescription;
	Array<VkAttachmentReference,16>		renderPassColorReference;
	VkAttachmentReference				renderPassDepthReference = {};

	RenderContext_Vk*                renderContext_vk = nullptr;
	RenderContext_Vk::BackBuffer_Vk* backBuffer_vk    = nullptr;
	if (desc.isBackBuffer) {
		renderContext_vk	= rttiCastCheck<RenderContext_Vk>(desc.renderContext);
		backBuffer_vk		= renderContext_vk->_getBackBuffer(desc.backBufferIndex);
	}

//---- color buffers ----
	const Int colorBufferCount = desc.colorBuffers.size();
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& colorBuf = _colorBuffers.emplaceBack();
		auto& srcColorDesc = desc.colorBuffers[i];

		_colorBuffers[i].desc = srcColorDesc;

		if (desc.isBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			colorBuf.colorBuf = backBuffer_vk->_colorBuf_vk;

		} else {
			RenderTargetColorBuffer_CreateDesc colorBufDesc;
			colorBufDesc.name = Fmt("{}-color", desc.name);
			colorBufDesc.colorType	= srcColorDesc.colorType;
			colorBufDesc.size		= desc.frameSize;

			colorBuf.colorBuf = RenderTargetColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBufDesc);
		}

		auto* colorBuf_vk = rttiCastCheck<RenderTargetColorBuffer_Vk>(colorBuf.colorBuf.ptr());
		AX_ASSERT(colorBuf_vk);

		auto viewHandle = colorBuf_vk->_view.handle();
		AX_ASSERT(viewHandle);
		frameBufferAttachments.emplaceBack(viewHandle);

		auto& colorReference = renderPassColorReference.emplaceBack();

		colorReference.attachment = AX_VkUtil::castUInt32(renderPassAttachmentDescription.size());
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		{
			auto& dst = renderPassAttachmentDescription.emplaceBack();
			dst.format			= AX_VkUtil::getVkColorType(colorBuf_vk->colorType());
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= AX_VkUtil::getVkLoadOp(srcColorDesc.loadOp);
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			if (srcColorDesc.loadOp == RenderBufferLoadOp::Clear) {
				dst.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			} else {
				dst.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
			}
		}
	}

//--- depth buffer
	_depthBuffer.desc = desc.depthBuffer;
	bool hasDepth = _depthBuffer.desc.isEnabled();
	if (hasDepth) {
		if (desc.isBackBuffer) {
			_depthBuffer.depthBuf = renderContext_vk->_depthBuf_vk;
			
		} else {
			RenderTargetDepthBuffer_CreateDesc depthBufDesc;
			depthBufDesc.name = Fmt("{}-depth", desc.name);
			depthBufDesc.depthType = desc.depthBuffer.depthType;
			depthBufDesc.frameSize = desc.frameSize;

			_depthBuffer.depthBuf = RenderTargetDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
		}

		auto* depthBuffer_vk = rttiCastCheck<RenderTargetDepthBuffer_Vk>(_depthBuffer.depthBuf.ptr()); 

		frameBufferAttachments.emplaceBack(depthBuffer_vk->_view.handle());
		auto depthFormat = AX_VkUtil::getVkDepthType(depthBuffer_vk->depthType());

		renderPassDepthReference.attachment = AX_VkUtil::castUInt32(renderPassAttachmentDescription.size());
		renderPassDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		{
			auto& dst			= renderPassAttachmentDescription.emplaceBack();
			dst.format			= depthFormat;
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= AX_VkUtil::getVkLoadOp(desc.depthBuffer.loadOp);
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if (desc.depthBuffer.loadOp == RenderBufferLoadOp::Clear) {
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
	subpassDesc.pColorAttachments			= renderPassColorReference.data();
	subpassDesc.pResolveAttachments			= nullptr;
	subpassDesc.pDepthStencilAttachment		= hasDepth ? &renderPassDepthReference : nullptr;
	subpassDesc.preserveAttachmentCount		= 0;
	subpassDesc.pPreserveAttachments		= nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext				= nullptr;
	renderPassCreateInfo.flags				= 0;
	renderPassCreateInfo.attachmentCount	= AX_VkUtil::castUInt32(renderPassAttachmentDescription.size());
	renderPassCreateInfo.pAttachments		= renderPassAttachmentDescription.data();
	renderPassCreateInfo.subpassCount		= 1;
	renderPassCreateInfo.pSubpasses			= &subpassDesc;
	renderPassCreateInfo.dependencyCount	= 0;
	renderPassCreateInfo.pDependencies		= nullptr;

	_renderPass_vk.create(dev, renderPassCreateInfo);
	_framebuffer_vk.create(dev, _renderPass_vk, frameBufferAttachments, AX_VkUtil::castVkExtent2D(desc.frameSize));

#if AX_DEBUG_NAME
	_renderPass_vk.setDebugName(Fmt("{}-renderPass", desc.name));
	_framebuffer_vk.setDebugName(Fmt("{}-framebuffer", desc.name));
#endif

}

} // namespace
#endif // AX_RENDERER_VK