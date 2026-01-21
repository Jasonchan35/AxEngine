module AxRender;

#if AX_RENDERER_VK

import :RenderRequest_Vk;
import :RenderSystem_Vk;
import :RenderObjectManager_Vk;
import :RenderContext_Vk;
import :Material_Vk;
import :GpuBuffer_Vk;
import :Texture_Vk;

namespace ax /*::AxRender*/ {

RenderRequest_Vk::RenderRequest_Vk(const CreateDesc& desc)
: Base(desc)
{
	auto* renderSystem = RenderSystem_Vk::s_instance();
	auto& dev = renderSystem->device();
	
	 _device_vk = &renderSystem->device();

	_completedFence_vk.create(dev, true);
	_imageAcquiredSemaphore_vk.create(dev);

	_uploadCmdList_vk.create(dev, dev.graphQueueFamilyIndex());
	_uploadCmdSem_vk.create(dev);
	
	_graphCmdList_vk.create(dev, dev.graphQueueFamilyIndex());
	_graphCmdSem_vk.create(dev);

#if AX_RENDER_DEBUG_NAME
	auto debugIndex = desc.index;
	        _completedFence_vk.setDebugName(Fmt("RenderReq_{}-completedFence",			debugIndex));
	_imageAcquiredSemaphore_vk.setDebugName(Fmt("RenderReq_{}-imageAcquiredSemaphore",	debugIndex));
	          _uploadCmdList_vk.setDebugName(Fmt("RenderReq_{}-uploadCmdList",			debugIndex));
	          _uploadCmdSem_vk.setDebugName(Fmt("RenderReq_{}-uploadCmdSem",			debugIndex));
	           _graphCmdList_vk.setDebugName(Fmt("RenderReq_{}-graphCmdList",				debugIndex));
	           _graphCmdSem_vk.setDebugName(Fmt("RenderReq_{}-graphCmdSem",				debugIndex));
#endif

	{
		AX_VkDescriptorPool_CreateDesc poolDesc;
		Int n = 1000;
		poolDesc.maxDestSetCount = n;
		poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER       , n * 4);
		poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE , n * 4);
		poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, n * 2);
		poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, n * 1);
#if AX_RENDER_BINDLESS		
		poolDesc.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
#endif		
		_descriptorPool.create(dev, poolDesc);
	}
}

void RenderRequest_Vk::onWaitCompleted() {
	if (!_completedFence_vk.wait(AxRenderConfig::kMaxRenderWaitTime())) {
		throw Error_Undefined("Render - timeout");
	}
	_uploadCmdList_vk.resetAndReleaseResource();
	_graphCmdList_vk.resetAndReleaseResource();
}

void RenderRequest_Vk::_updatedBindlessResources() {
#if 0 // AX_RENDER_BINDLESS
	auto* commonPass = rttiCastCheck<MaterialPass_Vk>(_commonMaterialPass);
	if (!commonPass) return;

	auto* bindlessParamSpace = commonPass->getOwnParamSpace_vk(BindSpace::Bindless);
	if (!bindlessParamSpace) throw Error_Undefined();

	auto curDescSet = bindlessParamSpace->_perFrameDataSet.getUpdated(this)._descSet;
	
	auto* shaderParamSpace = bindlessParamSpace->shaderParamSpace_vk();
	if (!shaderParamSpace) return;
	
	auto writeDescHelper = _writeDescSetHelper.scopeStart();

	{
		auto* samplerParam   = shaderParamSpace->findSamplerParam(AX_NAMEID("AxBindless_SamplerState"));
		for (auto& sampler_ : updatedBindlessResources.samplers) {
			if (!sampler_) continue;
			auto* sampler      = rttiCast<Sampler_Vk>(sampler_.ptr());
			u32   arrayElement = sampler->objectSlot.slotId();
			writeDescHelper.addSamplerInfo(samplerParam->bindPoint(), curDescSet, arrayElement, sampler->vkHandle());
		}
	}
	
	{
		auto* texture2DParam = shaderParamSpace->findTextureParam(AX_NAMEID("AxBindless_Texture2D"));
		for (auto& tex_ : updatedBindlessResources.texture2Ds) {
			auto* tex = rttiCastCheck<Texture2D_Vk>(tex_.ptr());
			if (!tex) continue;
			u32   arrayElement = tex->objectSlot.slotId();
			auto info = tex->_bindImage(this);
			writeDescHelper.addImageInfo(texture2DParam->bindPoint(), curDescSet, arrayElement, info.imageView, info.imageLayout);
		}
	}

	writeDescHelper.updateToDevice(_device_vk->handle());
#endif
}

void RenderRequest_Vk::onFrameBegin() {
#if AX_RENDER_BINDLESS
	auto* resMgr = rttiCastCheck<RenderObjectManager_Vk>(_objectManager);
	_bindlessDescriptorSet = resMgr->_bindlessDescriptorSet;
#endif	
	
	_descriptorPool.reset();
	_uploadCmdList_vk.commandBegin();
	_graphCmdList_vk.commandBegin();
}

void RenderRequest_Vk::onFrameEnd() {
	_updatedBindlessResources();
	_graphCmdList_vk.commandEnd();
	_uploadCmdList_vk.commandEnd();
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

	_graphCmdList_vk->debugLabelBegin(pass->name().toString(), Color4f(0, 0, 0.25f,1));
	vkCmdBeginRenderPass(_graphCmdList_vk, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderRequest_Vk::onRenderPassEnd(RenderPass* pass_) {
	vkCmdEndRenderPass(_graphCmdList_vk);
	_graphCmdList_vk->debugLabelEnd();
}

void RenderRequest_Vk::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	VkViewport tmp;
#if 1
	// flip Y - in vulkan Y is downward, while DX and OpenGL is up
	// VK_KHR_Maintenance1 allow negative height
	tmp.x        = rect.x;
	tmp.y        = rect.h - rect.y;
	tmp.width    = rect.w;
	tmp.height   = -rect.h;
#else
	tmp.x        = rect.x;
	tmp.y        = rect.y;
	tmp.width    = rect.w;
	tmp.height   = rect.h;
#endif	

	tmp.minDepth = minDepth;
	tmp.maxDepth = maxDepth;
	vkCmdSetViewport(_graphCmdList_vk, 0, 1, &tmp);
}

void RenderRequest_Vk::onSetScissorRect(const Rect2f& rect) {
	VkRect2D rc = AX_VkUtil::castVkRect2D(rect);
	vkCmdSetScissor(_graphCmdList_vk, 0, 1, &rc);
}

void RenderRequest_Vk::onDrawCall(AxDrawCallDesc& drawcall) {
	if (auto* vb = rttiCastCheck<GpuBuffer_Vk>(drawcall.vertexBuffer)) {
		constexpr u32 firstBinding = ax_enum_int(ShaderParamBindPoint::BindVertexBuffer);
		constexpr u32 bindingCount = 1 ;
		VkDeviceSize offset = vb->sparseOffset();
		vkCmdBindVertexBuffers(_graphCmdList_vk, firstBinding, bindingCount, &vb->vkBufHandle(), &offset);
	}

	if (drawcall.indexType == VertexIndexType::None) {
		vkCmdDraw(_graphCmdList_vk,
		          ax_safe_cast_from(drawcall.vertexCount),
		          ax_safe_cast_from(drawcall.instanceCount),
		          ax_safe_cast_from(drawcall.vertexStart),
		          ax_safe_cast_from(drawcall.instanceStart));

	} else {
		auto* ib = rttiCastCheck<GpuBuffer_Vk>(drawcall.indexBuffer);
		if (!ib) throw Error_Undefined();
		VkDeviceSize offset = ib->sparseOffset();
		vkCmdBindIndexBuffer(_graphCmdList_vk, ib->vkBufHandle(), offset, AX_VkUtil::getVkIndexType(drawcall.indexType));
		vkCmdDrawIndexed(_graphCmdList_vk,
		                 ax_safe_cast_from(drawcall.indexCount),
		                 ax_safe_cast_from(drawcall.instanceCount),
		                 ax_safe_cast_from(drawcall.indexStart),
		                 ax_safe_cast_from(drawcall.vertexStart),
		                 ax_safe_cast_from(drawcall.instanceStart));
	}
}


} // namespace
#endif // AX_RENDERER_VK