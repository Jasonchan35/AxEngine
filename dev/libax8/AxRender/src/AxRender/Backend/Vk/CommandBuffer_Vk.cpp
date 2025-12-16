module AxRender;

#if AX_RENDERER_VK

import :CommandBuffer_Vk;
import :RenderContext_Vk;
import :GpuBuffer_Vk;
import :StockObjects;
import :RenderPass_Vk;

namespace ax /*::AxRender*/ {

void CommandBuffer_Vk::create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queue) {
	_pool.create(dev, queue);
	_cmdBuf.create(_pool);
}

void CommandBuffer_Vk::commandBegin() {
	_cmdBuf.beginCommand();
}

void CommandBuffer_Vk::commandEnd() {
	_cmdBuf.endCommand();
}

} // namespace
#endif // AX_RENDERER_VK