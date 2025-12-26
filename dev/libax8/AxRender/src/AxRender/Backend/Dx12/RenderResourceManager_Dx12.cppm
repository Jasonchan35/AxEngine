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

	Dx12DescriptorHeapChunk_CBV_SRV_UAV	descChunk_Texture2D;
	Dx12DescriptorHeapChunk_Sampler		descChunk_Sampler;
};

} // namespace
#endif // #if AX_RENDERER_DX12