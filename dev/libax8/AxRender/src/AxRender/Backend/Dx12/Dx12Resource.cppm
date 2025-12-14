module;

export module AxRender:Dx12Resource;
import :Dx12Util;

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {

template<class DATA, class OWNER, void (OWNER::*)()>
class ScopeDataProxy0 : public NonCopyable {
public:
	AX_NODISCARD ScopeDataProxy0(OWNER* owner, DATA && data) : _owner(owner), _data(AX_FORWARD(data)) {}

private:
	OWNER*	_owner = nullptr;
	DATA	_data;
};



class Dx12ResourceBase : public NonCopyable {
	using This = Dx12ResourceBase;
public:
	void destroy();

	MutByteSpan	_mapMemory(IntRange range);
	void		_unmapMemory();
	
	using ScopeMapMemory = ScopeDataProxy0<MutByteSpan, This, &This::_unmapMemory>;
	ScopeMapMemory 	scopeMapMemory(IntRange range) { return ScopeMapMemory(this, _mapMemory(range));  }
	
	void uploadToGpu(Int offset, ByteSpan data);

	ID3D12Resource*	d3dResource() { return _d3dResource; }
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() { return _d3dResource ? _d3dResource->GetGPUVirtualAddress() : 0; }

	Int dataSize() const { return _dataSize; }
	Int alignedDataSize() const { return _alignedDataSize; }

	ID3D12Resource** ptrForInit() { return _d3dResource.ptrForInit(); }

	bool isValid() const { return _d3dResource.ptr() != nullptr; }

	D3D12_RESOURCE_STATES	resourceState() const { return _resourceState; }

	void resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state);

protected:
	Dx12ResourceBase();
	void _create(const D3D12_CLEAR_VALUE* clearValue = nullptr);

	ComPtr<ID3D12Resource> _d3dResource;
	Int _dataSize = 0;
	Int _alignedDataSize = 0;

	D3D12_RESOURCE_DESC		_desc  = {};
	D3D12_HEAP_PROPERTIES	_heapProps = {};
	D3D12_RESOURCE_STATES	_resourceState;
};

class Dx12Resource_RenderTarget : public Dx12ResourceBase {
public:
	void createFromSwapChain(AX_DX12_IDXGISwapChain* swapChain, UINT i);
};

class Dx12Resource_DepthStencilBuffer : public Dx12ResourceBase {
public:
	void create(Vec2i size);
};

class Dx12Resource_Buffer : public Dx12ResourceBase {
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

} // namespace

#endif //AX_RENDERER_DX12