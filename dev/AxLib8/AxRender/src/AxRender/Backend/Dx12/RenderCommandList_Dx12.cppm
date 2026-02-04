module;

export module AxRender:RenderCommandList_Dx12;

#if AX_RENDER_DX12

export import :Dx12Util;
export import :RenderCommandList_Backend;
export import :RenderSystem_Backend;
export import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {
class RenderCommandList_Dx12 : public RenderCommandList_Backend {
	AX_RTTI_INFO(RenderCommandList_Dx12, RenderCommandList_Backend)
public:
	void create(ID3D12Device* dev, RenderCommandListType type, StrView name);

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

	AX_ID3D12GraphicsCommandList* operator->() { return _cmdList_dx12.ptr(); }
	operator AX_ID3D12GraphicsCommandList*() { return _cmdList_dx12.ptr(); }

	ComPtr<AX_ID3D12CommandAllocator>		_cmdAllocator_dx12;
	ComPtr<AX_ID3D12GraphicsCommandList>	_cmdList_dx12;
};

} // namespace

#endif // #if AX_RENDER_DX12