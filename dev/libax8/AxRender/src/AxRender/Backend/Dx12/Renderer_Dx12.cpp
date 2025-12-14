module AxRender;

#if AX_RENDERER_DX12

import :Renderer_Dx12;
import :GpuBuffer_Dx12;
import :Material_Dx12;
import :RenderContext_Dx12;
import :RenderPass_Dx12;
import :RenderRequest_Dx12;
import :RenderTarget_Dx12;
import :Shader_Dx12;
import :Texture_Dx12;

namespace ax {

AX_Renderer_FunctionBodies(Dx12);

Renderer_Dx12::Renderer_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	HRESULT hr;
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(_dxgiDebug.ptrForInit()));
	Dx12Util::throwIfError(hr);

	hr = D3D12GetDebugInterface(IID_PPV_ARGS(_d3dDebug.ptrForInit()));
	Dx12Util::throwIfError(hr);

	// Enable the debug layer (requires the Graphics Tools "optional feature" from Windows Settings > System > Optional features).
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	_d3dDebug->EnableDebugLayer();
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#endif
	
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
	Dx12Util::throwIfError(hr);

	_getHardwareAdapter();

	hr = D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(_d3dDevice.ptrForInit())); 
	Dx12Util::throwIfError(hr);

	static const D3D_FEATURE_LEVEL s_featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		ARRAYSIZE(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_12_0
	};
	hr = _d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	Dx12Util::throwIfError(hr);

	_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;

	{
		D3D12_FEATURE_DATA_SHADER_MODEL s = {};
		auto reqShaderModel = D3D_SHADER_MODEL_6_0;
		s.HighestShaderModel = reqShaderModel;
		hr = _d3dDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &s, sizeof(s));
		Dx12Util::throwIfError(hr);

		if (s.HighestShaderModel < reqShaderModel) {
			AX_LOG("DX12 supported shader model {?:X} < {?:X} is required for DXIL shader", static_cast<int>(s.HighestShaderModel), static_cast<int>(reqShaderModel));
			//			throw Error_Undefined();
		}

		if (s.HighestShaderModel >= D3D_SHADER_MODEL_5_1) {
			_adapterInfo.computeShader = true;
		}
	}

	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS  s = {};
		hr = _d3dDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &s, sizeof(s));
		Dx12Util::throwIfError(hr);
		_adapterInfo.shaderFloat64 = s.DoublePrecisionFloatShaderOps;
	}

#if AX_RENDERER_DX12_DXR
	auto rt = axSPtr_new(axDX12RayTracing());
	if (rt->create()) {
		_rayTracing = rt;
	}
#endif

}

void Renderer_Dx12::onGetMemoryInfo(MemoryInfo& info) {
	DXGI_QUERY_VIDEO_MEMORY_INFO  i;
	auto hr = _dxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &i);
	Dx12Util::throwIfError(hr);

	info.used = i.CurrentUsage;
	info.budget = i.Budget;
}

void Renderer_Dx12::_getHardwareAdapter() {
	for (UINT i = 0; ; ++i) {
		ComPtr<IDXGIAdapter1> adapter;
		auto hr =  _dxgiFactory->EnumAdapters1(i, adapter.ptrForInit());
		if (DXGI_ERROR_NOT_FOUND == hr)
			break;

		hr = adapter->QueryInterface(IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
		Dx12Util::throwIfError(hr);

		DXGI_ADAPTER_DESC1 desc;
		_dxgiAdapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		hr = D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(AX_DX12_ID3D12Device), nullptr);
		if (!Dx12Util::checkError(hr))
			continue;

		_adapterInfo.name.setUtf(StrView_c_str(desc.Description));
		_adapterInfo.memorySize = desc.DedicatedVideoMemory;

		SIZE_T megaByte = 1024 * 1024;

		AX_LOG(	"DX12 Adpter = {}\n"
				"	Revision = {}\n"
				"	VendorId = {}\n"
				"	DeviceId = {}\n"
				"	 Video  Memory = {}M\n"
				"	 System Memory = {}M\n"
				"	 Shared Memory = {}M\n",
					desc.Description, 
					desc.Revision,
					desc.VendorId,
					desc.DeviceId,
					desc.DedicatedVideoMemory  / megaByte,
					desc.DedicatedSystemMemory / megaByte,
					desc.SharedSystemMemory    / megaByte);

		return;
	}

	throw Error_Undefined();
}

} // namespace

#endif //AX_RENDERER_DX12