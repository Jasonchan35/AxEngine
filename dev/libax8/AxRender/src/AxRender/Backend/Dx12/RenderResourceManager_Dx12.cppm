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
	
	Dx12DescriptorHeap		descriptorHeap_CBV_SRV_UAV;
	Dx12DescriptorHeap		descriptorHeap_Sampler;
};

RenderResourceManager_Dx12::RenderResourceManager_Dx12(const CreateDesc& desc): Base(desc) {

	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 256 * 1024;
	heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	descriptorHeap_CBV_SRV_UAV.create(dev, heapDesc);
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	heapDesc.NumDescriptors = 1000;
	descriptorHeap_Sampler.create(dev, heapDesc);
}

} // namespace
#endif // #if AX_RENDERER_DX12