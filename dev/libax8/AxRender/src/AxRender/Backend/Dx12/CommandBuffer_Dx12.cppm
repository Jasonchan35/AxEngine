module;

export module AxRender:CommandBuffer_Dx12;

#if AX_RENDERER_DX12

export import :Dx12Util;
export import :CommandBuffer_Backend;
export import :Renderer_Backend;
export import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {
class CommandBuffer_Dx12 : public CommandBuffer_Backend {
	AX_RTTI_INFO(CommandBuffer_Dx12, CommandBuffer_Backend)
public:
	void create(ID3D12Device* dev, CommandBufferType type, StrView name);

	void commandBegin() {
		auto hr = _cmdAllocator_dx12->Reset();
		Dx12Util::throwIfError(hr);
	
		ID3D12PipelineState* pipelineState = nullptr;
		hr = _cmdList_dx12->Reset(_cmdAllocator_dx12, pipelineState);
		Dx12Util::throwIfError(hr);
	}
	
	void commandEnd() {
		auto hr = _cmdList_dx12->Close();
		Dx12Util::throwIfError(hr);	
	}

	ID3D12GraphicsCommandList* operator->() { return _cmdList_dx12.ptr(); }
	operator ID3D12GraphicsCommandList*() { return _cmdList_dx12.ptr(); }

	ComPtr<ID3D12CommandAllocator>		_cmdAllocator_dx12;
	ComPtr<ID3D12GraphicsCommandList>	_cmdList_dx12;
};

} // namespace

#endif // #if AX_RENDERER_DX12