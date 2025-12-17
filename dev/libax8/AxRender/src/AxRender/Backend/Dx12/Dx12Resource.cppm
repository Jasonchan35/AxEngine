module;

export module AxRender:Dx12Resource;
import :Dx12Util;

#if AX_RENDERER_DX12

import :Renderer_Backend;

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

	Int dataSize() const { return _dataSize; }
	Int alignedDataSize() const { return _alignedDataSize; }

	ID3D12Resource** ptrForInit() { return _d3dResource.ptrForInit(); }

	bool isValid() const { return _d3dResource.ptr() != nullptr; }

	D3D12_RESOURCE_STATES resourceState() const { return _resourceState; }

	D3D12_RESOURCE_STATES resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state);

	operator ID3D12Resource*() { return _d3dResource; }

	D3D12_RESOURCE_DESC&	desc() { return _desc; }
	
protected:
	Dx12ResourceBase();
	void _create(const D3D12_CLEAR_VALUE* clearValue = nullptr);
	void _reset();

	ComPtr<ID3D12Resource> _d3dResource;
	Int _dataSize = 0;
	Int _alignedDataSize = 0;

	D3D12_RESOURCE_DESC		_desc  = {};
	D3D12_HEAP_PROPERTIES	_heapProps = {};
	D3D12_RESOURCE_STATES	_resourceState;
};

class Dx12Resource_ColorBuffer : public Dx12ResourceBase {
public:
	void createFromSwapChain(AX_DX12_IDXGISwapChain* swapChain, UINT backBufIndex);
	void create(Vec2i size, ColorType colorType);
};

class Dx12Resource_DepthBuffer : public Dx12ResourceBase {
public:
	void create(Vec2i size, RenderDepthType depthType);
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
		_h = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
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

	bool wait() { return _wait(INFINITE); }
	bool wait(const Milliseconds& timeout) { return _wait(Dx12Util::castDWORD(timeout.value)); }
	
private:
	
	bool _wait(DWORD dwMilliseconds) {
		AX_ASSERT(_h != nullptr);
		DWORD ret = ::WaitForSingleObject(_h, dwMilliseconds);
		if (ret == WAIT_TIMEOUT) return false;
		if (ret == WAIT_OBJECT_0) return true;
		throw Error_Undefined("Dx12Win32Event - wait");
	}
	
	
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

	AX_DX12_IDXGISwapChain* ptr()			{ return _swapChain; }
	operator AX_DX12_IDXGISwapChain*()		{ return _swapChain; }
	
private:
	ComPtr<AX_DX12_IDXGISwapChain>	_swapChain;
};

} // namespace

#endif //AX_RENDERER_DX12