module AxRender;

#if AX_RENDERER_VK

import :RenderCommandList_Vk;
import :RenderContext_Vk;
import :GpuBuffer_Vk;
import :StockObjects;
import :RenderPass_Vk;

namespace ax /*::AxRender*/ {

void RenderCommandList_Vk::create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queue) {
	_pool.create(dev, queue);
	_cmdBuf.create(_pool);
}

void RenderCommandList_Vk::commandBegin() {
	_cmdBuf.beginCommand();
}

void RenderCommandList_Vk::commandEnd() {
	_cmdBuf.endCommand();
}

} // namespace
#endif // AX_RENDERER_VK