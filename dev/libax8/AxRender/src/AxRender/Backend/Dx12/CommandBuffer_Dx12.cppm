module;

export module AxRender:CommandBuffer_Dx12;

#if AX_RENDERER_DX12

export import :Dx12Util;
export import :CommandBuffer;
export import :Renderer_Backend;
export import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {
class CommandBuffer_Dx12 : public CommandBuffer {
	AX_RTTI_INFO(CommandBuffer_Dx12, CommandBuffer)
public:
	void create(ID3D12Device* dev, CommandBufferType type);

	AX_RENDER_CommandBuffer_FunctionInterfaces(override)

	ComPtr<ID3D12CommandAllocator>		_cmdAllocator_dx12;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList_dx12;
};

} // namespace

#endif // #if AX_RENDERER_DX12