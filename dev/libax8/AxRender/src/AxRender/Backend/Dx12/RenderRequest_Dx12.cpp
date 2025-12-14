module AxRender;
import :RenderRequest_Dx12;
import :Renderer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderRequest_Dx12::RenderRequest_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	auto* renderer = Renderer_Dx12::s_instance();
	auto* dev = renderer->d3dDevice();
	
	{
		auto hr = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ptrForInit()));
		Dx12Util::throwIfError(hr);

		hr = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(_cmdList.ptrForInit()));
		Dx12Util::throwIfError(hr);

		hr = _cmdList->Close();
		Dx12Util::throwIfError(hr);
	}

	{ // compute
		auto hr = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(_computeCmdAllocator.ptrForInit()));
		Dx12Util::throwIfError(hr);

		hr = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, _computeCmdAllocator, nullptr, IID_PPV_ARGS(_computeCmdList.ptrForInit()));
		Dx12Util::throwIfError(hr);

		hr = _computeCmdList->Close();
		Dx12Util::throwIfError(hr);
	}

	{
		auto hr = Renderer_Dx12::s_d3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_computeFence.ptrForInit()));
		Dx12Util::throwIfError(hr);
	}
}

} // namespace

#endif // #if AX_RENDERER_DX12