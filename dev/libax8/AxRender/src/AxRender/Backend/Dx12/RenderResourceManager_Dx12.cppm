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

	virtual void onUpdateDescriptors(Array<SPtr<Sampler_Backend  >>& list) {}
	virtual void onUpdateDescriptors(Array<SPtr<Texture2D_Backend>>& list) {}
	
	Dx12DescriptorHeapPool_ColorBuffer	descHeap_ColorBuffer;
	Dx12DescriptorHeapPool_DepthBuffer	descHeap_DepthBuffer;
	Dx12DescriptorHeapPool_CBV_SRV_UAV	descHeap_CBV_SRV_UAV;
	Dx12DescriptorHeapPool_Sampler		descHeap_Sampler;
};

RenderResourceManager_Dx12::RenderResourceManager_Dx12(const CreateDesc& desc): Base(desc) {
	auto& info = RenderSystem_Dx12::s_instance()->info();
	auto* dev = RenderSystem_Dx12::s_d3dDevice();

	descHeap_ColorBuffer.create(dev, info.renderPass.maxColorBufferCount * info.renderPass.maxCount * info.renderRequest.count);
	descHeap_DepthBuffer.create(dev, info.renderPass.maxDepthBufferCount * info.renderPass.maxCount * info.renderRequest.count);

	Int reqCount = info.renderRequest.count;
	Int renderRequest_CBV_SRV_UAV_Count = info.renderRequest.maxConstBufferCount + info.renderRequest.maxTextureCount;
	Int resource_CBV_SRV_UAV_Count = info.resource.maxTextureCount;
	
	descHeap_CBV_SRV_UAV.create(dev, resource_CBV_SRV_UAV_Count    + renderRequest_CBV_SRV_UAV_Count    * reqCount);
	    descHeap_Sampler.create(dev, info.resource.maxSamplerCount + info.renderRequest.maxSamplerCount * reqCount);
/*
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 256 * 1024;
	heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	descriptorHeap_CBV_SRV_UAV.create(dev, heapDesc);
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	heapDesc.NumDescriptors = 1000;
	descriptorHeap_Sampler.create(dev, heapDesc);
	*/
}

} // namespace
#endif // #if AX_RENDERER_DX12