module AxRender;

#if AX_RENDER_DX12

import :RenderCommandList_Dx12;
import :RenderPass_Dx12;

namespace ax /*::AxRender*/ {

void RenderCommandList_Dx12::create(ID3D12Device* dev, RenderCommandListType type, StrView name) {

	D3D12_COMMAND_LIST_TYPE type_dx12 = Dx12Util::getDxCommandListType(type);
	
	auto hr = dev->CreateCommandAllocator(type_dx12, IID_PPV_ARGS(_cmdAllocator_dx12.ptrForInit()));
	Dx12Util::throwIfError(hr);
	
	hr = dev->CreateCommandList(0, type_dx12, _cmdAllocator_dx12, nullptr, IID_PPV_ARGS(_cmdList_dx12.ptrForInit()));
	Dx12Util::throwIfError(hr);

	hr = _cmdList_dx12->Close();
	Dx12Util::throwIfError(hr);

#if AX_RENDER_DEBUG_NAME
	_cmdAllocator_dx12->SetName(Fmt(L"{}-cmdAlloc", name).c_str());
	_cmdList_dx12->SetName(Fmt(L"{}-cmdList", name).c_str());
#endif
	
}

} // namespace

#endif // #if AX_RENDER_DX12
 