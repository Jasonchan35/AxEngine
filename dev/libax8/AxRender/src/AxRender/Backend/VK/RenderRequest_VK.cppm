module;

export module AxRender:RenderRequest_VK;

#if AX_RENDERER_VK
export import :CommandBuffer_VK;
export import :RenderContext_VK;
export import :RenderRequest_Bindless_VK;
export import :Renderer_VK;

export namespace ax::AxRender {


class RenderRequest_VK : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_VK, RenderRequest_Backend)

public:
	RenderRequest_VK(const CreateDesc& desc);

	RenderContext_VK*	renderContext()		{ return rttiCastCheck<RenderContext_VK>(_renderContext); }
	RenderPass_VK*		currentRenderPass()	{ return rttiCastCheck<RenderPass_VK   >(_currentRenderPass); }
	CommandBuffer_VK&	uploadCmdBuf_vk()	{ return _uploadCmdBuf_vk; }
	CommandBuffer_VK&	graphCmdBuf_vk()	{ return _graphCmdBuf_vk; }

	virtual void onWaitCompleted() override;
	virtual void onFrameBegin() override;
	virtual void onFrameEnd() override;

	Renderer_VK*	renderer() { return rttiCastCheck<Renderer_VK>(_renderer); }

#if AX_RENDER_BINDLESS
	RenderRequest_Bindless_VK _bindless;
#endif

	CommandBuffer_VK	_uploadCmdBuf_vk; // submit early than graphCmdBuf

	CommandBuffer_VK	_graphCmdBuf_vk;
	AX_VkSemaphore		_graphSemaphore_vk;
	AX_VkSemaphore		_imageAcquiredSemaphore_vk;

	AX_VkFence			_completedFence_vk;
};

} // namespace
#endif // AX_RENDERER_VK