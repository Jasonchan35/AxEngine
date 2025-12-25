module;

export module AxRender:RenderRequest_Vk;

#if AX_RENDERER_VK
export import :CommandBuffer_Vk;
export import :RenderContext_Vk;
export import :RenderRequest_Bindless_Vk;
export import :RenderSystem_Vk;
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

	void _updatedBindlessResources();
	
	RenderSystem_Vk*	renderSystem_vk() { return rttiCastCheck<RenderSystem_Vk>(_renderSystem); }

	AX_RenderRequest_Backend_FunctionInterfaces(override)

	CommandBuffer_Vk	_uploadCmdBuf_vk; // submit earlier than graphCmdBuf
	AX_VkSemaphore		_uploadCmdSem_vk;

	CommandBuffer_Vk	_graphCmdBuf_vk;
	AX_VkSemaphore		_graphCmdSem_vk;
	
	AX_VkSemaphore		_imageAcquiredSemaphore_vk;
	AX_VkFence			_completedFence_vk;

	Array<VkWriteDescriptorSet>		_writeDescriptorSets;
	LinearAllocator					_writeDescLinearAllocator;

	VkDescriptorBufferInfo*		_getWriteDescBufferInfo() { return _writeDescLinearAllocator.newObject<VkDescriptorBufferInfo>(); }
	VkDescriptorImageInfo*		_getWriteDescImageInfo()  { return _writeDescLinearAllocator.newObject<VkDescriptorImageInfo>();   }

	AX_VkDescriptorPool			_descriptorPool;
};

} // namespace
#endif // AX_RENDERER_VK