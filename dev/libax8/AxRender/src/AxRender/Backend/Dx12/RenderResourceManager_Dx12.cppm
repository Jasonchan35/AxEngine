module;

export module AxRender:RenderResourceManager_Dx12;
#if AX_RENDERER_DX12

export import :Dx12DescripterHeap;
export import :RenderResourceManager_Backend;
export import :Renderer_Backend;

namespace ax {

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	RenderResourceManager_Dx12(const CreateDesc& desc) : Base(desc) {}


	Dx12DescripterHeap_CBV_SRV_UAV		_CBV_SRV_UAV_DescHeap;
	Dx12DescripterHeap_Sampler			_samplerDescHeap;
	
protected:
};

}

#endif // #if AX_RENDERER_DX12