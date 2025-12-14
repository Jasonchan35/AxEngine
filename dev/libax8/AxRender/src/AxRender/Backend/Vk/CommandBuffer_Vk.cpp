module AxRender;

#if AX_RENDERER_VK

import :CommandBuffer_Vk;
import :RenderContext_Vk;
import :GpuBuffer_Vk;
import :StockObjects;
import :RenderPass_Vk;

namespace ax /*::AxRender*/ {

CommandBuffer_Vk& CommandBuffer_Vk::create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queue) {
	_pool.create(dev, queue);
	_cmdBuf.create(_pool);
	return *this;
}

void CommandBuffer_Vk::onRenderPassBegin(RenderPass* pass_) {
	auto* pass  = rttiCast<RenderPass_Vk >(pass_);
	AX_ASSERT(pass);

	VkExtent2D extent = AX_VkUtil::castVkExtent2D(pass->frameSize());

	Int colorBufCount = pass->colorBufferCount();

	Array<VkClearValue, 32>	clearValues;
	for (Int i = 0; i < colorBufCount; i++ ) {
		auto& dst = clearValues.emplaceBack().color;
		AX_VkUtil::setFloat4(dst.float32, pass->clearColor(i));
	}

	{
		auto& dst = clearValues.emplaceBack().depthStencil;
		dst.depth   = pass->clearDepth();
		dst.stencil = pass->clearStencil();
	}

	VkRenderPassBeginInfo info = {};
	info.sType					= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext					= nullptr;
	info.renderPass				= pass->_renderPass;
	info.framebuffer			= pass->_framebuffer;
	info.renderArea.offset.x	= 0;
	info.renderArea.offset.y	= 0;
	info.renderArea.extent		= extent;
	info.clearValueCount		= AX_VkUtil::castUInt32(clearValues.size());
	info.pClearValues			= clearValues.data();

	_cmdBuf.debugLabelBegin(pass->name(), Color4f(0, 0, 0.25f,1));
	vkCmdBeginRenderPass(_cmdBuf, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer_Vk::onRenderPassEnd() {
	vkCmdEndRenderPass(_cmdBuf);
	_cmdBuf.debugLabelEnd();
}

void CommandBuffer_Vk::onCommandBegin() {
	_cmdBuf.beginCommand();
}

void CommandBuffer_Vk::onCommandEnd() {
	_cmdBuf.endCommand();
}

void CommandBuffer_Vk::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	VkViewport tmp;
	tmp.x = rect.x;
	tmp.y = rect.y;
	tmp.width = rect.w;
	tmp.height = rect.h;
	tmp.minDepth = minDepth;
	tmp.maxDepth = maxDepth;
	vkCmdSetViewport(_cmdBuf, 0, 1, &tmp);
}

void CommandBuffer_Vk::onSetScissorRect(const Rect2f& rect) {
	VkRect2D rc = AX_VkUtil::castVkRect2D(rect);
	vkCmdSetScissor(_cmdBuf, 0, 1, &rc);
}

void CommandBuffer_Vk::onDrawCall(Cmd_DrawCall& cmd) {
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
		vkCmdBindVertexBuffers(_cmdBuf, firstBinding, 1, &vb->vkBufHandle(), &offset);
	}

	if (cmd.indexType == IndexType::None) {
		vkCmdDraw(_cmdBuf, vertexCount, instanceCount, vertexStart, instanceStart);

	} else if (auto* ib = rttiCastCheck<GpuBuffer_Vk>(cmd.indexBuffer)) {
		vkCmdBindIndexBuffer(_cmdBuf, ib->vkBufHandle(), 0, AX_VkUtil::getVkIndexType(cmd.indexType));
		vkCmdDrawIndexed(_cmdBuf, indexCount, instanceCount, indexStart, SafeCast(vertexStart), instanceStart);

	} else {
		AX_ASSERT(false);
	}
}

} // namespace
#endif // AX_RENDERER_VK