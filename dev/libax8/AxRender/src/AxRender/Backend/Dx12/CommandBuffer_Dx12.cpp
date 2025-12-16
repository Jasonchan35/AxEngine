module AxRender;

#if AX_RENDERER_DX12

import :CommandBuffer_Dx12;
import :RenderPass_Dx12;

namespace ax /*::AxRender*/ {

void CommandBuffer_Dx12::create(ID3D12Device* dev, CommandBufferType type) {

	D3D12_COMMAND_LIST_TYPE type_dx12 = Dx12Util::getDxCommandBufferType(type);
	
	auto hr = dev->CreateCommandAllocator(type_dx12, IID_PPV_ARGS(_cmdAllocator_dx12.ptrForInit()));
	Dx12Util::throwIfError(hr);

	hr = dev->CreateCommandList(0, type_dx12, _cmdAllocator_dx12, nullptr, IID_PPV_ARGS(_cmdList_dx12.ptrForInit()));
	Dx12Util::throwIfError(hr);

	hr = _cmdList_dx12->Close();
	Dx12Util::throwIfError(hr);	
}

} // namespace

#endif // #if AX_RENDERER_DX12
 