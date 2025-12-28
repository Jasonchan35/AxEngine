module;

export module AxRender:RenderRequest_Vk;

#if AX_RENDERER_VK
export import :RenderCommandList_Vk;
export import :RenderContext_Vk;
export import :RenderSystem_Vk;
export import :RenderPass_Vk;

export namespace ax /*::AxRender*/ {

class RenderRequest_Vk : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Vk, RenderRequest_Backend)
public:
	using BindPoint = ShaderParamBindPoint;
	
	RenderRequest_Vk(const CreateDesc& desc);

	RenderContext_Vk*	renderContext_vk()		{ return rttiCastCheck<RenderContext_Vk>(_renderContext); }
	RenderPass_Vk*		currentRenderPass_vk()	{ return rttiCastCheck<RenderPass_Vk   >(_currentRenderPass); }
	RenderCommandList_Vk&	uploadCmdList_vk()		{ return _uploadCmdList_vk; }
	RenderCommandList_Vk&	graphCmdList_vk()		{ return _graphCmdList_vk; }

	void _updatedBindlessResources();
	
	RenderSystem_Vk*	renderSystem_vk() { return rttiCastCheck<RenderSystem_Vk>(_renderSystem); }

	AX_RenderRequest_Backend_FunctionInterfaces(override)

	AX_VkDevice*		_device_vk = nullptr;

	RenderCommandList_Vk	_uploadCmdList_vk; // submit earlier than graphCmdList
	AX_VkSemaphore			_uploadCmdSem_vk;

	RenderCommandList_Vk	_graphCmdList_vk;
	AX_VkSemaphore			_graphCmdSem_vk;
	
	AX_VkSemaphore			_imageAcquiredSemaphore_vk;
	AX_VkFence				_completedFence_vk;

	AX_VkDescriptorPool				_descriptorPool;
	AX_VkDescriptor_UpdateHelper	_writeDescSetHelper;

#if AX_RENDER_BINDLESS
	VkDescriptorSet		_bindlessDescriptorSet = nullptr;
#endif
	
};

} // namespace
#endif // AX_RENDERER_VK