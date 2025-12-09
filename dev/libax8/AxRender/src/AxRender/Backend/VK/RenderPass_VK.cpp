module AxRender;

#if AX_RENDERER_VK

import :RenderPass_VK;
import :Renderer_VK;
import :RenderContext_VK;
import :GpuBuffer_VK;
import :RenderRequest_VK;

namespace ax::AxRender {

RenderPass_VK::RenderPass_VK(const CreateDesc& desc) 
: Base(desc)
{
	auto& dev = Renderer_VK::s_instance()->device();

	const Int colorBufferCount = desc.colorBuffers.size();
	Array<VkImageView, 16>				frameBufferAttachments;

	Array<VkAttachmentDescription, 16>	renderPassAttachmentDescription;
	Array<VkAttachmentReference,16>		renderPassColorReference;
	VkAttachmentReference				renderPassDepthReference = {};

//---- color buffers
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& colorBuf = _colorBuffers.emplaceBack();
		auto& srcColorDesc = desc.colorBuffers[i];

		_colorBuffers[i].desc = srcColorDesc;

		if (desc.isBackBuffer) {
			colorBuf.colorBuf = desc.backBufferRenderContext->backColorBuffer(desc.backBufferIndex);
		} else {
			RenderColorBuffer_CreateDesc colorBufDesc;
			colorBufDesc.name = Fmt("{}-color", desc.name);
			colorBufDesc.colorType	= srcColorDesc.colorType;
			colorBufDesc.size		= desc.frameSize;

			colorBuf.colorBuf = RenderColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBufDesc);
		}

		auto* color_tex_vk = rttiCastCheck<RenderColorBuffer_VK>(colorBuf.colorBuf.ptr());
		AX_ASSERT(color_tex_vk);

		auto viewHandle = color_tex_vk->_view.handle();
		AX_ASSERT(viewHandle);
		frameBufferAttachments.emplaceBack(viewHandle);

		auto& colorReference = renderPassColorReference.emplaceBack();

		colorReference.attachment = AX_VkUtil::castUInt32(renderPassAttachmentDescription.size());
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		{
			auto& dst = renderPassAttachmentDescription.emplaceBack();
			dst.format			= AX_VkUtil::getVkColorType(color_tex_vk->colorType());
			dst.flags			= 0;
			dst.samples			= VK_SAMPLE_COUNT_1_BIT;
			dst.loadOp			= AX_VkUtil::getVkLoadOp(srcColorDesc.loadOp);
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			if (srcColorDesc.loadOp == BufferLoadOp::Clear) {
				dst.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			} else {
				dst.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR ;
			}
		}
	}

//--- depth buffer
	_depthBuffer.desc = desc.depthBuffer;
	bool hasDepth = _depthBuffer.desc.isEnabled();

	if (desc.isBackBuffer) {
		_depthBuffer.depthBuf = desc.backBufferRenderContext->backDepthBuffer();
	} else {
		RenderDepthBuffer_CreateDesc depthBufDesc;
		depthBufDesc.name = Fmt("{}-depth", desc.name);
		depthBufDesc.depthType = desc.depthBuffer.depthType;
		depthBufDesc.frameSize = desc.frameSize;

		_depthBuffer.depthBuf = RenderDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
	}

	auto* depth_tex_vk = rttiCastCheck<RenderDepthBuffer_VK>(_depthBuffer.depthBuf.ptr());
	if (depth_tex_vk) {
		hasDepth = true;

		frameBufferAttachments.emplaceBack(depth_tex_vk->_view.handle());
		auto depthFormat = AX_VkUtil::getVkDepthType(depth_tex_vk->depthType());

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

			if (desc.depthBuffer.loadOp == BufferLoadOp::Clear) {
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

	_renderPass.create(dev, renderPassCreateInfo);
	_framebuffer.create(dev, _renderPass, frameBufferAttachments, AX_VkUtil::castVkExtent2D(desc.frameSize));

#if AX_DEBUG_NAME
	_renderPass.setDebugName(Fmt("{}-renderPass", desc.name));
	_framebuffer.setDebugName(Fmt("{}-framebuffer", desc.name));
#endif

}

} // namespace
#endif // AX_RENDERER_VK