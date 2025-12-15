module;

export module AxRender:RenderRequest_Vk;

#if AX_RENDERER_VK
export import :CommandBuffer_Vk;
export import :RenderContext_Vk;
export import :RenderRequest_Bindless_Vk;
export import :Renderer_Vk;
export import :RenderPass_Vk;

export namespace ax /*::AxRender*/ {

class RenderRequest_Vk : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Vk, RenderRequest_Backend)

public:
	RenderRequest_Vk(const CreateDesc& desc);

	RenderContext_Vk*	renderContext_vk()		{ return rttiCastCheck<RenderContext_Vk>(_renderContext); }
	RenderPass_Vk*		currentRenderPass_vk()	{ return rttiCastCheck<RenderPass_Vk   >(_currentRenderPass); }
	CommandBuffer_Vk&	uploadCmdBuf_vk()		{ return _uploadCmdBuf_vk; }
	CommandBuffer_Vk&	graphCmdBuf_vk()		{ return _graphCmdBuf_vk; }

	virtual void onWaitCompleted() override;
	virtual void onFrameBegin() override;
	virtual void onFrameEnd() override;
	
	Renderer_Vk*	renderer_vk() { return rttiCastCheck<Renderer_Vk>(_renderer); }

#if AX_RENDER_BINDLESS
	RenderRequest_Bindless_Vk _bindless;
#endif

	CommandBuffer_Vk	_uploadCmdBuf_vk; // submit earlier than graphCmdBuf

	CommandBuffer_Vk	_graphCmdBuf_vk;
	AX_VkSemaphore		_graphSemaphore_vk;
	AX_VkSemaphore		_imageAcquiredSemaphore_vk;

	AX_VkFence			_completedFence_vk;
};

} // namespace
#endif // AX_RENDERER_VK