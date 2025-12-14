module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
export import :Dx12Util;
export import :Renderer_Backend;
export import :RenderRequest_Backend;

namespace ax {

class RenderRequest_Dx12 : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Dx12, RenderRequest_Backend)
public:
	
	RenderRequest_Dx12(const CreateDesc& desc);

	ComPtr<ID3D12CommandAllocator>	_cmdAllocator;
	ComPtr<ID3D12CommandAllocator>	_computeCmdAllocator;

	ComPtr<ID3D12GraphicsCommandList> _cmdList;
	ComPtr<ID3D12GraphicsCommandList> _computeCmdList;

	ComPtr<ID3D12Fence>	_computeFence;
};

} // namespace

#endif //AX_RENDERER_DX12