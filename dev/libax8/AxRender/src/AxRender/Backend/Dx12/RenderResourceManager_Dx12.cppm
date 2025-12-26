module;

export module AxRender:RenderResourceManager_Dx12;
#if AX_RENDERER_DX12

export import :Dx12DescriptorHeap;
export import :RenderResourceManager_Backend;
export import :RenderSystem_Backend;

namespace ax {

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE();
	
	RenderResourceManager_Dx12(const CreateDesc& desc);

	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Sampler_Backend  >>& list) override;
	virtual void onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Texture2D_Backend>>& list) override;
	
	Dx12DescriptorHeapPool_ColorBuffer	descHeapPool_ColorBuffer;
	Dx12DescriptorHeapPool_DepthBuffer	descHeapPool_DepthBuffer;
	Dx12DescriptorHeapPool_CBV_SRV_UAV	descHeapPool_CBV_SRV_UAV;
	Dx12DescriptorHeapPool_Sampler		descHeapPool_Sampler;

	Dx12DescriptorAllocator_CBV_SRV_UAV	descAlloc_Texture2D;
	Dx12DescriptorAllocator_Sampler		descAlloc_Sampler;
};

RenderResourceManager_Dx12::RenderResourceManager_Dx12(const CreateDesc& desc): Base(desc) {
	auto& info = RenderSystem_Dx12::s_instance()->info();
	auto* dev = RenderSystem_Dx12::s_d3dDevice();

	descHeapPool_ColorBuffer.create(dev, info.renderPass.maxColorBufferCount * info.renderPass.maxCount * info.renderRequest.count);
	descHeapPool_DepthBuffer.create(dev, info.renderPass.maxDepthBufferCount * info.renderPass.maxCount * info.renderRequest.count);

	Int reqCount = info.renderRequest.count;
	Int renderRequest_CBV_SRV_UAV_Count = info.renderRequest.maxConstBufferCount + info.renderRequest.maxTextureCount;
	Int resource_CBV_SRV_UAV_Count = info.resource.maxTextureCount;
	
	descHeapPool_CBV_SRV_UAV.create(dev, resource_CBV_SRV_UAV_Count    + renderRequest_CBV_SRV_UAV_Count    * reqCount);
	    descHeapPool_Sampler.create(dev, info.resource.maxSamplerCount + info.renderRequest.maxSamplerCount * reqCount);

	descAlloc_Texture2D.create(descHeapPool_CBV_SRV_UAV, info.resource.maxTextureCount);
	  descAlloc_Sampler.create(descHeapPool_Sampler    , info.resource.maxSamplerCount);

	descAlloc_Texture2D.adjustUsedToSize();
	  descAlloc_Sampler.adjustUsedToSize();

}

} // namespace
#endif // #if AX_RENDERER_DX12