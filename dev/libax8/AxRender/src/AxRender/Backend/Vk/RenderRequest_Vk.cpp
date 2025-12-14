module AxRender;

#if AX_RENDERER_VK

import :RenderRequest_Vk;
import :Renderer_Vk;
import :RenderContext_Vk;
import :Material_Vk;

namespace ax /*::AxRender*/ {

RenderRequest_Vk::RenderRequest_Vk(const CreateDesc& desc)
: Base(desc)
{
	auto* renderer = Renderer_Vk::s_instance();
	auto& dev = renderer->device();

	_completedFence_vk.create(dev, true);
	_graphSemaphore_vk.create(dev);
	_imageAcquiredSemaphore_vk.create(dev);

	_uploadCmdBuf_vk.create(dev, dev.graphQueueFamilyIndex());
	_graphCmdBuf_vk.create(dev, dev.graphQueueFamilyIndex());

	Base::_graphCmdBuf = &_graphCmdBuf_vk;

#if AX_DEBUG_NAME
	auto debugIndex = desc.index;
	        _completedFence_vk.setDebugName(Fmt("RenderReq_{}-completedFence",			debugIndex));
	        _graphSemaphore_vk.setDebugName(Fmt("RenderReq_{}-graphSemaphore",			debugIndex));
	_imageAcquiredSemaphore_vk.setDebugName(Fmt("RenderReq_{}-imageAcquiredSemaphore",	debugIndex));
	          _uploadCmdBuf_vk.setDebugName(Fmt("RenderReq_{}-uploadCmdBuf",			debugIndex));
	           _graphCmdBuf_vk.setDebugName(Fmt("RenderReq_{}-graphCmdBuf",				debugIndex));
#endif
}

void RenderRequest_Vk::onWaitCompleted() {
	_completedFence_vk.wait();
	_uploadCmdBuf_vk.resetAndReleaseResource();
	_graphCmdBuf_vk.resetAndReleaseResource();
}

void RenderRequest_Vk::onFrameBegin() {
	Base::onFrameBegin();
	_uploadCmdBuf_vk.onCommandBegin();
	_graphCmdBuf_vk.onCommandBegin();
}

void RenderRequest_Vk::onFrameEnd() {
#if AX_RENDER_BINDLESS
	_bindless.update(this);
#endif

	_graphCmdBuf_vk.onCommandEnd();
	_uploadCmdBuf_vk.onCommandEnd();
	Base::onFrameEnd();
}


} // namespace
#endif // AX_RENDERER_VK