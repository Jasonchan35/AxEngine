module AxRender;

#if AX_RENDERER_VK

import :RenderRequest_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :Material_Vk;
import :GpuBuffer_Vk;

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

#if AX_DEBUG_NAME
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
	if (!_completedFence_vk.wait(AxRenderConfig::kMaxRenderWaitTime)) {
		throw Error_Undefined("Render - timeout");
	}
	_uploadCmdBuf_vk.resetAndReleaseResource();
	_graphCmdBuf_vk.resetAndReleaseResource();
}

void RenderRequest_Vk::onFrameBegin() {
	_uploadCmdBuf_vk.commandBegin();
	_graphCmdBuf_vk.commandBegin();
}

void RenderRequest_Vk::onFrameEnd() {
#if AX_RENDER_BINDLESS
	_bindless.update(this);
#endif
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
		AX_VkUtil::setFloat4(dst.float32, src.desc.clearColorValue);
	}
	
	if (auto& desc = pass->depthAttachment().desc) {
		auto& dst         = clearValues.emplaceBack().depthStencil;
		dst.depth         = desc.clearDepthValue;
		dst.stencil       = desc.clearStencilValue;
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

	_graphCmdBuf_vk->debugLabelBegin(pass->name(), Color4f(0, 0, 0.25f,1));
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

void RenderRequest_Vk::onDrawCall(Cmd_DrawCall& cmd) {
	u32	vertexCount   = AX_VkUtil::castUInt32(cmd.vertexCount);
	u32 vertexStart   = AX_VkUtil::castUInt32(cmd.vertexStart);

	u32 indexStart    = AX_VkUtil::castUInt32(cmd.indexStart);
	u32 indexCount    = AX_VkUtil::castUInt32(cmd.indexCount);

	u32 instanceStart = AX_VkUtil::castUInt32(cmd.instanceStart);
	u32 instanceCount = AX_VkUtil::castUInt32(cmd.instanceCount);

	if (auto* vb = rttiCastCheck<GpuBuffer_Vk>(cmd.vertexBuffer)) {
		// draw indirect doesn't support byte offset
		// VkDeviceSize vertexBufferByteOffset = AX_VkUtil::castUInt32(cmd.vertexBufferByteOffset);
		u32 firstBinding = ax_enum_int(ShaderResourceBindPoint::VertexBuffer);
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(_graphCmdBuf_vk, firstBinding, 1, &vb->vkBufHandle(), &offset);
	}

	if (cmd.indexType == IndexType::None) {
		vkCmdDraw(_graphCmdBuf_vk, vertexCount, instanceCount, vertexStart, instanceStart);

	} else if (auto* ib = rttiCastCheck<GpuBuffer_Vk>(cmd.indexBuffer)) {
		vkCmdBindIndexBuffer(_graphCmdBuf_vk, ib->vkBufHandle(), 0, AX_VkUtil::getVkIndexType(cmd.indexType));
		vkCmdDrawIndexed(_graphCmdBuf_vk, indexCount, instanceCount, indexStart, ax_safe_cast_from(vertexStart), instanceStart);

	} else {
		AX_ASSERT(false);
	}
}


} // namespace
#endif // AX_RENDERER_VK