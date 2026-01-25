module;

export module AxRender:Dx12Resource;
import :Dx12Util;

#if AX_RENDERER_DX12

import :RenderSystem_Backend;

namespace ax {

enum class Dx12ResourceType {
	None,
	ColorBuffer,
};

class Dx12ResourceBase : public NonCopyable {
	using This = Dx12ResourceBase;
public:
	void destroy();

	MutByteSpan	_mapMemory(IntRange range);
	void		_unmapMemory();
	
	using ScopedMapMemory = ScopedMemFuncProxy0<MutByteSpan, This, &This::_unmapMemory>;
	ScopedMapMemory 	mapMemory(IntRange range) { return ScopedMapMemory(_mapMemory(range), this);  }

	void uploadToGpu(Int offset, ByteSpan data);

	ID3D12Resource*	d3dResource() { return _d3dResource; }
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() { return _d3dResource ? _d3dResource->GetGPUVirtualAddress() : 0; }

	Dx12_ID3DResource** ptrForInit() { return _d3dResource.ptrForInit(); }

	bool isValid() const { return _d3dResource.ptr() != nullptr; }

	D3D12_RESOURCE_STATES resourceState() const { return _resourceState; }

	D3D12_RESOURCE_STATES resourceBarrier_Debug(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newResourceState, const SrcLoc& srcLoc = SrcLoc::s_current());
	D3D12_RESOURCE_STATES resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newResourceState);

	operator ID3D12Resource*() { return _d3dResource; }

	const D3D12_RESOURCE_DESC1&	resourceDesc() const { return _resourceDesc; }

	Int bufferSize() const { return _bufferSize; }

#if AX_RENDER_DEBUG_NAME
	void setDebugName(StrView debugName) { _debugName.setUtf(debugName); _d3dResource->SetName(_debugName.c_str()); }
#endif
	
protected:
	Dx12ResourceBase();
	void _create(const D3D12_CLEAR_VALUE* clearValue = nullptr);
	void _reset();

	ComPtr<Dx12_ID3DResource>   _d3dResource;
	ComPtr<D3D12MA::Allocation> _resourceAllocation;
	Int                         _bufferSize    = 0;
	D3D12MA::ALLOCATION_DESC    _allocDesc     = {};
	D3D12_RESOURCE_DESC1        _resourceDesc  = {};
	D3D12_RESOURCE_STATES       _resourceState = D3D12_RESOURCE_STATE_COMMON;

#if AX_RENDER_DEBUG_NAME
	StringW _debugName;
#endif
};

class Dx12Resource_ColorBuffer : public Dx12ResourceBase {
public:
	void createFromSwapChain(Dx12_IDXGISwapChain* swapChain, UINT backBufIndex);
	void create(Vec2i size, ColorType colorType, const D3D12_CLEAR_VALUE& clearValue);
};

class Dx12Resource_DepthBuffer : public Dx12ResourceBase {
public:
	void create(Vec2i size, RenderDepthType depthType, const D3D12_CLEAR_VALUE& clearValue);
};

class Dx12Resource_GpuBuffer : public Dx12ResourceBase {
public:
	void create(GpuBufferType type, Int bufferSize);
	void create(GpuBufferType type, ByteSpan data) {
		create(type, data.sizeInBytes());
		uploadToGpu(0, data);
	}
};

class Dx12Resource_Texture : public Dx12ResourceBase {
};

class Dx12Resource_Texture2D : public Dx12Resource_Texture {
public:
	void create(Vec2i size, Int mipmapCount, ColorType colorType);
};

class Dx12Fence : public NonCopyable {
public:
	void create(ID3D12Device* dev, u64 initialValue);
	u64 getCompletedValue() { return _fence->GetCompletedValue(); }
	
	operator ID3D12Fence* () { return _fence; }
	ComPtr<ID3D12Fence>	_fence;
};

class Dx12CpuEvent : public NonCopyable {
public:
	~Dx12CpuEvent() { destroy(); }
	
	void create() {
		_h = ::CreateEvent(nullptr, false, false, nullptr);
		if (_h == nullptr) throw Error_Undefined("Dx12Win32Event - CreateEvent");
	}

	void destroy() {
		if (_h != nullptr) { CloseHandle(_h); }
	}

	void signalOnFenceCompletion(Dx12Fence& fence, u64 value) {
		AX_ASSERT(_h != nullptr);
		auto hr = fence._fence->SetEventOnCompletion(value, _h);
		Dx12Util::throwIfError(hr);
	}

	void signal() {
		::SetEvent(_h);
	}

	bool wait(const Opt<Milliseconds>& timeout) {
		AX_ASSERT(_h != nullptr);
		DWORD dw = timeout ? ax_safe_cast_to<DWORD>(timeout->value) : INFINITE;
		DWORD ret = ::WaitForSingleObject(_h, dw);
		if (ret == WAIT_OBJECT_0) return true;
		if (ret == WAIT_TIMEOUT) return false;
		throw Error_Undefined("Dx12Win32Event - wait");
	}	
	
private:

	HANDLE _h = nullptr; // // Event use nullptr, not INVALID_HANDLE_VALUE(-1)
};

class Dx12CommandQueue : public NonCopyable {
public:
	void create(ID3D12Device* dev);
	
	void signal(Dx12Fence& fence, u64 value) {
		auto hr = _queue->Signal(fence, value);
		Dx12Util::throwIfError(hr);
	}

	void signal(Dx12Fence& fence, Dx12CpuEvent& cpuEvent, u64 value) {
		signal(fence, value);
		cpuEvent.signalOnFenceCompletion(fence, value);
	}
	
	void gpuWait(Dx12Fence& fence, u64 value) {
		auto hr = _queue->Wait(fence, value);
		Dx12Util::throwIfError(hr);
	}

	void execCmdList(ID3D12CommandList* cmdList) {
		_queue->ExecuteCommandLists(1, &cmdList);
	}
	
	void execCmdList(Span<ID3D12CommandList*> cmdListSpan) {
		_queue->ExecuteCommandLists(Dx12Util::castUINT(cmdListSpan.size()), cmdListSpan.data());
	}

//	ID3D12CommandQueue* operator->() { return _queue; }
	operator ID3D12CommandQueue* ()  { return _queue; }
private:
	ComPtr<ID3D12CommandQueue>	_queue;
};

class Dx12SwapChain : public NonCopyable {
public:
	void create(Dx12CommandQueue& cmdQueue, HWND hwnd, DXGI_SWAP_CHAIN_DESC1& desc);
	void getDesc(DXGI_SWAP_CHAIN_DESC1* desc) {
		auto hr = _swapChain->GetDesc1(desc);
		Dx12Util::throwIfError(hr);
	}

	void resizeBuffers(Int bufferCount, Vec2i frameSize, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
		auto hr = _swapChain->ResizeBuffers(Dx12Util::castUINT(bufferCount),
		                                    Dx12Util::castUINT(frameSize.x),
		                                    Dx12Util::castUINT(frameSize.y),
		                                    NewFormat,
		                                    SwapChainFlags);
		Dx12Util::throwIfError(hr);
	}

	Int getCurrentBackBufferIndex() { return ax_safe_cast_from(_swapChain->GetCurrentBackBufferIndex()); }

	void present(UINT SyncInterval, UINT Flags);

	Dx12_IDXGISwapChain* ptr()			{ return _swapChain; }
	operator Dx12_IDXGISwapChain*()		{ return _swapChain; }
	
private:
	ComPtr<Dx12_IDXGISwapChain>	_swapChain;
};

struct Dx12DescriptorTable : public NonCopyable {
	using BindPoint = ShaderParamBindPoint;
	using BindSpace = ShaderParamBindSpace;

	Array<D3D12_DESCRIPTOR_RANGE, 8>	descriptorRanges;

	Int size() const { return descriptorRanges.size(); }

	Int addDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE type, BindPoint bindPoint, Int bindCount, BindSpace bindSpace) {
		_totalBindCount += bindCount;
		Int descriptprIndex = descriptorRanges.size();
		auto& dst                             = descriptorRanges.emplaceBack();
		dst.RangeType                         = type;
		dst.NumDescriptors                    = Dx12Util::castUINT(bindCount);
		dst.BaseShaderRegister                = Dx12Util::castUINT(ax_enum_int(bindPoint));
		dst.RegisterSpace                     = Dx12Util::castUINT(ax_enum_int(bindSpace));
		dst.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		return descriptprIndex;
	}

	Int totalBindCount() const { return _totalBindCount; }
private:	
	Int _totalBindCount = 0; 
};

#define AX_Dx12RootParamType_ENUM_LIST(E) \
	E(None                 ,) \
	E(RootCBV              ,) \
	E(RootSRV              ,) \
	E(RootUAV              ,) \
	E(RootUInt32           ,) \
	E(RootStaticSampler    ,) \
	E(DescTable_CBV_SRV_UAV,) \
	E(DescTable_Sampler    ,) \
	E(_COUNT,) \
//-----
AX_ENUM_CLASS(AX_Dx12RootParamType_ENUM_LIST, Dx12RootParamType, u8);
constexpr auto Dx12RootParamType_COUNT = ax_enum_int(Dx12RootParamType::_COUNT);

struct Dx12RootParameterList {
	using BindPoint = ShaderParamBindPoint;
	using BindSpace = ShaderParamBindSpace;

	UINT addRootDescriptorTable(D3D12_SHADER_VISIBILITY shaderVisibility, const Dx12DescriptorTable& table) {
		if (table.descriptorRanges.size() <= 0) throw Error_Undefined();
		UINT rootParamIndex = ax_safe_cast_from(parameters.size());
		auto& dst = parameters.emplaceBack();
		dst.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		dst.ShaderVisibility                    = shaderVisibility;
		dst.DescriptorTable.NumDescriptorRanges = Dx12Util::castUINT(table.descriptorRanges.size());
		dst.DescriptorTable.pDescriptorRanges   = table.descriptorRanges.data();
		return rootParamIndex;
	}

	UINT addRoot32BitConst(D3D12_SHADER_VISIBILITY shaderVisibility, BindPoint bindPoint, BindSpace	bindSpace, Int sizeInBytes) {
		UINT rootParamIndex = ax_safe_cast_from(parameters.size());
		auto& dst = parameters.emplaceBack();
		dst.ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		dst.ShaderVisibility         = shaderVisibility;
		dst.Constants.ShaderRegister = Dx12Util::castUINT(ax_enum_int(bindPoint));
		dst.Constants.RegisterSpace  = Dx12Util::castUINT(ax_enum_int(bindSpace));
		dst.Constants.Num32BitValues = ax_safe_cast_from(sizeInBytes / 4); // 32bit = 4 bytes
		return rootParamIndex;
	}

	UINT addRootCBV(D3D12_SHADER_VISIBILITY shaderVisibility, BindPoint bindPoint, BindSpace bindSpace) {
		UINT rootParamIndex = ax_safe_cast_from(parameters.size());
		auto& dst = parameters.emplaceBack();
		dst.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
		dst.ShaderVisibility          = shaderVisibility;
		dst.Descriptor.ShaderRegister = Dx12Util::castUINT(ax_enum_int(bindPoint));
		dst.Descriptor.RegisterSpace  = Dx12Util::castUINT(ax_enum_int(bindSpace));	
		return rootParamIndex;
	}

	UINT addRootSRV(D3D12_SHADER_VISIBILITY shaderVisibility, BindPoint bindPoint, BindSpace bindSpace) {
		UINT rootParamIndex = ax_safe_cast_from(parameters.size());
		auto& dst = parameters.emplaceBack();
		dst.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
		dst.ShaderVisibility          = shaderVisibility;
		dst.Descriptor.ShaderRegister = Dx12Util::castUINT(ax_enum_int(bindPoint));
		dst.Descriptor.RegisterSpace  = Dx12Util::castUINT(ax_enum_int(bindSpace));
		return rootParamIndex;
	}

	UINT addRootUAV(D3D12_SHADER_VISIBILITY shaderVisibility, BindPoint bindPoint, BindSpace bindSpace) {
		UINT rootParamIndex = ax_safe_cast_from(parameters.size());
		auto& dst = parameters.emplaceBack();
		dst.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
		dst.ShaderVisibility          = shaderVisibility;
		dst.Descriptor.ShaderRegister = Dx12Util::castUINT(ax_enum_int(bindPoint));
		dst.Descriptor.RegisterSpace  = Dx12Util::castUINT(ax_enum_int(bindSpace));	
		return rootParamIndex;
	}

	UINT addRootStaticSampler(D3D12_SHADER_VISIBILITY shaderVisibility,
		                      BindPoint               bindPoint,
		                      BindSpace               bindSpace,
		                      SamplerFilter           filter,
		                      SamplerWrapUVW          wrap
	) {
		UINT rootParamIndex = ax_safe_cast_from(parameters.size());
		auto& dst            = staticSamplers.emplaceBack();
		dst.ShaderVisibility = shaderVisibility;
		dst.Filter           = Dx12Util::getDxSamplerFilter(filter);
		dst.AddressU         = Dx12Util::getDxSamplerWrap(wrap.u);
		dst.AddressV         = Dx12Util::getDxSamplerWrap(wrap.v);
		dst.AddressW         = Dx12Util::getDxSamplerWrap(wrap.w);
		dst.MipLODBias       = 0;
		dst.MaxAnisotropy    = 0;
		dst.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
		dst.BorderColor      = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		dst.MinLOD           = 0.0f;
		dst.MaxLOD           = D3D12_FLOAT32_MAX;
		dst.ShaderRegister   = Dx12Util::castUINT(ax_enum_int(bindPoint));
		dst.RegisterSpace    = Dx12Util::castUINT(ax_enum_int(bindSpace));
		return rootParamIndex;
	}

	void createRootSignature(ComPtr<ID3D12RootSignature> &outRootSignature);

	Array<D3D12_ROOT_PARAMETER>		 parameters;
	Array<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
private:
};

struct Dx12_IndirectDrawArgument {
	// D3D12_GPU_VIRTUAL_ADDRESS    argumentDataSrv;
	D3D12_DRAW_INDEXED_ARGUMENTS	drawIndexed;
	AxDrawCallRootConst				rootConst;
};


} // namespace

#endif //AX_RENDERER_DX12