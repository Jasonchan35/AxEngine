module AxRender;

#if AX_RENDERER_VK

import :RenderRequest_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :Material_Vk;
import :GpuBuffer_Vk;
import :Texture_Vk;

namespace ax /*::AxRender*/ {

RenderRequest_Vk::RenderRequest_Vk(const CreateDesc& desc)
: Base(desc)
{
	auto* renderer = Renderer_Vk::s_instance();
	auto& dev = renderer->device();

	_completedFence_vk.create(dev, true);
	_imageAcquiredSemaphore_vk.create(dev);

	_uploadCmdBuf_vk.create(dev, dev.graphQueueFamilyIndex());
	_uploadCmdSem_vk.create(dev);
	
	_graphCmdBuf_vk.create(dev, dev.graphQueueFamilyIndex());
	_graphCmdSem_vk.create(dev);

#if AX_RENDER_DEBUG_NAME
	auto debugIndex = desc.index;
	        _completedFence_vk.setDebugName(Fmt("RenderReq_{}-completedFence",			debugIndex));
	_imageAcquiredSemaphore_vk.setDebugName(Fmt("RenderReq_{}-imageAcquiredSemaphore",	debugIndex));
	          _uploadCmdBuf_vk.setDebugName(Fmt("RenderReq_{}-uploadCmdBuf",			debugIndex));
	          _uploadCmdSem_vk.setDebugName(Fmt("RenderReq_{}-uploadCmdSem",			debugIndex));
	           _graphCmdBuf_vk.setDebugName(Fmt("RenderReq_{}-graphCmdBuf",				debugIndex));
	           _graphCmdSem_vk.setDebugName(Fmt("RenderReq_{}-graphCmdSem",				debugIndex));
#endif
}

void RenderRequest_Vk::onWaitCompleted() {
	if (!_completedFence_vk.wait(AxRenderConfig::kMaxRenderWaitTime())) {
		throw Error_Undefined("Render - timeout");
	}
	_uploadCmdBuf_vk.resetAndReleaseResource();
	_graphCmdBuf_vk.resetAndReleaseResource();

	_writeDescriptorSets.clear();
	_writeDescriptor_BufferInfos.clear();
	_writeDescriptor_ImageInfos.clear();
}

void RenderRequest_Vk::_updatedBindlessResources() {
#if AX_RENDER_BINDLESS
	auto* renderer =  renderer_vk();
	auto* commonMaterial = renderer->commonMaterial();
	if (!commonMaterial) return;

	auto* mtlSpace = commonMaterial->getPassParamSpace_<MaterialParamSpace_Vk>(0, BindSpace::Global);
	if (!mtlSpace) return;

	auto* shdSpace = mtlSpace->shaderParamSpace();
	if (!shdSpace) return; 

	auto currentDescriptorSet = mtlSpace->getUpdatedDescriptorSet(this);

	using ParamBase = ShaderParamSpace_Backend::ParamBase;
	
	auto addWriteSet = [&](auto* param, VkDescriptorType _descriptorType, ResourceSlotId slotId) -> VkWriteDescriptorSet& {
		auto& w = _writeDescriptorSet.emplaceBack();
		w.sType			  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		w.pNext			  = nullptr;
		w.descriptorType  = _descriptorType;
		w.dstSet		  = currentDescriptorSet;
		w.dstBinding	  = AX_VkUtil::castUInt32(ax_enum_int(param->bindPoint()));
		w.dstArrayElement = ax_enum_int(slotId);
		w.descriptorCount = 1;
		return w;
	};

	auto* samplerParam   = shdSpace->findSamplerParam(AX_NAMEID("AxBindless_SamplerState"));
	for (auto& sampler_ : updatedBindlessResources.samplers) {
		if (!sampler_) continue;
		auto* sampler         = rttiCast<Sampler_Vk>(sampler_.ptr());
		auto& desc            = addWriteSet(samplerParam, VK_DESCRIPTOR_TYPE_SAMPLER, sampler->resourceHandle.slotId());
		auto& imageInfo       = _writeDescriptor_imageInfos.emplaceBack();
		imageInfo.imageView   = VK_NULL_HANDLE;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sampler     = sampler->vkHandle();
		desc.pImageInfo       = &imageInfo;
	}
	auto* texture2DParam = shdSpace->findTextureParam(AX_NAMEID("AxBindless_Texture2D"));
	for (auto& tex_ : updatedBindlessResources.texture2Ds) {
		if (!tex_) continue;
		auto* tex       = rttiCastCheck<Texture2D_Vk>(tex_.ptr());
		auto& desc      = addWriteSet(texture2DParam, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, tex->resourceHandle.slotId());
		auto& imageInfo = _writeDescriptor_imageInfos.emplaceBack();
		tex->_bindImage(this, imageInfo);
		desc.pImageInfo = &imageInfo;
	}
	
	AX_vkUpdateDescriptorSets(renderer->device(), _writeDescriptorSet, {});
#endif
}

void RenderRequest_Vk::onFrameBegin() {
	_uploadCmdBuf_vk.commandBegin();
	_graphCmdBuf_vk.commandBegin();
}

void RenderRequest_Vk::onFrameEnd() {
	_updatedBindlessResources();
	_graphCmdBuf_vk.commandEnd();
	_uploadCmdBuf_vk.commandEnd();
}

void RenderRequest_Vk::onRenderPassBegin(RenderPass* pass_) {
	auto* pass  = rttiCast<RenderPass_Vk >(pass_);
	AX_ASSERT(pass);

	VkExtent2D extent = AX_VkUtil::castVkExtent2D(pass->frameSize());

	Array<VkClearValue, 32>	clearValues;
	for (auto& src : pass->colorAttachments()) {
		auto& dst = clearValues.emplaceBack().color;
		AX_VkUtil::setFloat4(dst.float32, src.desc.clearColor);
	}
	
	if (auto& desc = pass->depthAttachment().desc) {
		auto& dst         = clearValues.emplaceBack().depthStencil;
		dst.depth         = desc.clearDepth;
		dst.stencil       = desc.clearStencil;
	}

	VkRenderPassBeginInfo info = {};
	info.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext					= nullptr;
	info.renderPass				= pass->_renderPass_vk;
	info.framebuffer			= pass->_framebuffer_vk;
	info.renderArea.offset.x	= 0;
	info.renderArea.offset.y	= 0;
	info.renderArea.extent		= extent;
	info.clearValueCount		= AX_VkUtil::castUInt32(clearValues.size());
	info.pClearValues			= clearValues.data();

	_graphCmdBuf_vk->debugLabelBegin(pass->name().toString(), Color4f(0, 0, 0.25f,1));
	vkCmdBeginRenderPass(_graphCmdBuf_vk, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderRequest_Vk::onRenderPassEnd(RenderPass* pass_) {
	vkCmdEndRenderPass(_graphCmdBuf_vk);
	_graphCmdBuf_vk->debugLabelEnd();
}

void RenderRequest_Vk::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	VkViewport tmp;
	tmp.x        = rect.x;
	tmp.y        = rect.y;
	tmp.width    = rect.w;
	tmp.height   = rect.h;
	tmp.minDepth = minDepth;
	tmp.maxDepth = maxDepth;
	vkCmdSetViewport(_graphCmdBuf_vk, 0, 1, &tmp);
}

void RenderRequest_Vk::onSetScissorRect(const Rect2f& rect) {
	VkRect2D rc = AX_VkUtil::castVkRect2D(rect);
	vkCmdSetScissor(_graphCmdBuf_vk, 0, 1, &rc);
}

void RenderRequest_Vk::onDrawCall(Cmd_DrawCall& drawcall) {

	if (auto* vb = rttiCastCheck<GpuBuffer_Vk>(drawcall.vertexBuffer)) {
		constexpr u32 firstBinding = ax_enum_int(ShaderParamBindPoint::VertexBuffer);
		constexpr u32 bindingCount = 1 ;
		// auto vertexLayout = drawcall.vertexLayout;
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(_graphCmdBuf_vk, firstBinding, bindingCount, &vb->vkBufHandle(), &offset);
	}

	if (drawcall.indexType == IndexType::None) {
		vkCmdDraw(_graphCmdBuf_vk,
		          ax_safe_cast_from(drawcall.vertexCount),
		          ax_safe_cast_from(drawcall.instanceCount),
		          ax_safe_cast_from(drawcall.vertexStart),
		          ax_safe_cast_from(drawcall.instanceStart));

	} else {
		auto* ib = rttiCastCheck<GpuBuffer_Vk>(drawcall.indexBuffer);
		if (!ib) throw Error_Undefined();

		vkCmdBindIndexBuffer(_graphCmdBuf_vk, ib->vkBufHandle(), 0, AX_VkUtil::getVkIndexType(drawcall.indexType));
		vkCmdDrawIndexed(_graphCmdBuf_vk,
		                 ax_safe_cast_from(drawcall.indexCount),
		                 ax_safe_cast_from(drawcall.instanceCount),
		                 ax_safe_cast_from(drawcall.indexStart),
		                 ax_safe_cast_from(drawcall.vertexStart),
		                 ax_safe_cast_from(drawcall.instanceStart));
	}
}


} // namespace
#endif // AX_RENDERER_VK