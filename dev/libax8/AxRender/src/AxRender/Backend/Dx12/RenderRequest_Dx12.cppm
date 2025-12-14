module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
import :Dx12Util;
import :Renderer_Backend;
import :RenderRequest_Backend;

namespace ax {

class RenderRequest_Dx12 : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Dx12, RenderRequest_Backend)
public:
	

	RenderRequest_Dx12() {
		auto* dev = Dx12Util::d3dDevice();
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
			auto hr = Dx12Util::d3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_computeFence.ptrForInit()));
			Dx12Util::throwIfError(hr);
		}
	}

	virtual void onGpuEnded() override {
		_textureUploadRequests2d.clear();
	}

	ComPtr<ID3D12CommandAllocator>	_cmdAllocator;
	ComPtr<ID3D12CommandAllocator>	_computeCmdAllocator;

	ComPtr<ID3D12GraphicsCommandList> _cmdList;
	ComPtr<ID3D12GraphicsCommandList> _computeCmdList;

	ComPtr<ID3D12Fence>	_computeFence;

	axDX12TextureManager::GpuUploadRequests2D	_textureUploadRequests2d;
};

} // namespace

#endif //AX_RENDERER_DX12