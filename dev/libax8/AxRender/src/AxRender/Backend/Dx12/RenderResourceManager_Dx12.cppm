module;

export module AxRender:RenderResourceManager_Dx12;
#if AX_RENDERER_DX12

export import :Dx12DescripterHeap;
export import :RenderResourceManager_Backend;
export import :RenderSystem_Backend;

namespace ax {

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE();
	
	RenderResourceManager_Dx12(const CreateDesc& desc);

#if AX_RENDER_BINDLESS
	Dx12DescripterHeap		bindlessHeap_CBV_SRV_UAV;
	Dx12DescripterHeap		bindlessHeap_Sampler;
#endif	
	
protected:
};

RenderResourceManager_Dx12::RenderResourceManager_Dx12(const CreateDesc& desc): Base(desc) {

#if AX_RENDER_BINDLESS
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1000;
	heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	bindlessHeap_CBV_SRV_UAV.create(dev, heapDesc);
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	bindlessHeap_Sampler.create(dev, heapDesc);
#endif

}

} // namespace
#endif // #if AX_RENDERER_DX12