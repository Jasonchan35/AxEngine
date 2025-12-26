module;

export module AxRender:Dx12DescriptorHeap;
import :Dx12Resource;
import :RenderSystem_Dx12;

#if AX_RENDERER_DX12

import :RenderSystem_Backend;

namespace ax {

struct Dx12DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};

	// operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return cpu; }
	// operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return gpu; }

	Dx12DescriptorHandle operator+(Int offset) const {
		Dx12DescriptorHandle h;
		h.cpu.ptr = cpu.ptr ? cpu.ptr + offset : 0;
		h.gpu.ptr = gpu.ptr ? gpu.ptr + offset : 0;
		return h;
	}
};

struct Dx12Descriptor_ColorBuffer	{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_DepthBuffer	{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_Sampler		{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_ConstBuffer	{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_UAV			{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_RawUAV		{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_Texture		{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_Texture2D		{ Dx12DescriptorHandle handle; };


struct Dx12DescriptorHeap : public NonCopyable {
	ID3D12DescriptorHeap* d3dHeap() { return _d3dHeap; }
	
	void create(Dx12_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc);
	void destroy();
	
	Dx12DescriptorHandle currentHandle() { return getHandle(_used); }
	Dx12DescriptorHandle getHandle(Int index) { return _startHandle + _stride * index; }

	Int size  () const { return _size; }
	Int used  () const { return _used; }
	Int stride() const { return _stride; }
	Int remain() const { return _size - _used; }

	void adjustUsed(Int v) { _used += v; }
	void reset() { _used = 0;}
private:
	ComPtr<ID3D12DescriptorHeap> _d3dHeap;
	Dx12DescriptorHandle         _startHandle;

	Int _size   = 0;
	Int _used   = 0;
	Int _stride = 0;
};

class Dx12DescriptorHeapPool : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;

	void destroy();
	void reset();

	ID3D12DescriptorHeap* d3dHeap() { return _heap.d3dHeap(); }
	
protected:
	friend class Dx12DescriptorAllocator;
	void _onCreateAllocator(Dx12DescriptorAllocator & allocator, Int size);
	void _create(Dx12_ID3D12Device* dev, Int numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	
	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	Dx12_ID3D12Device*				_dev = nullptr;
	Dx12DescriptorHeap				_heap;
};

class Dx12DescriptorAllocator : public NonCopyable {
public:
	void reset() { _used = 0; }

	Int size() const { return _size; }
	Int used() const { return _used; }
	Int remain() const { return _size - _used; }
	ID3D12DescriptorHeap* d3dHeap() const				{ return _d3dHeap; }
	Dx12DescriptorHandle  startHandle() const			{ return _startHandle; }
	Dx12DescriptorHandle  currentHandle() const			{ return _startHandle + _stride * _used; }
	Dx12DescriptorHandle  getHandle(Int index) const	{
		if (index < 0 || index >= _used) throw Error_IndexOutOfRange();
		return _startHandle + _stride * index;
	}

	void adjustUsedToSize() { _used = _size; }
	
protected:
	void _create(Dx12DescriptorHeapPool& heapPool, Int size) { return heapPool._onCreateAllocator(*this, size); }
	
	Int _addHandle() {
		if (_used >= _size) throw Error_Undefined();
		return _used++;
	}

	template<class HANDLE>
	HANDLE _getHandle(Int index) {
		HANDLE h;
		h.handle = getHandle(index);
		return h; 
	}

protected:
	friend class Dx12DescriptorHeapPool;
		
	Dx12DescriptorHandle  _startHandle;
	Int                   _size    = 0;
	Int                   _used    = 0;
	Int                   _stride  = 0;
	ID3D12DescriptorHeap* _d3dHeap = nullptr;
	Dx12_ID3D12Device*    _dev     = nullptr;
};

class Dx12DescriptorHeapPool_ColorBuffer : public Dx12DescriptorHeapPool {
public:
	void create(Dx12_ID3D12Device* dev, Int numDescriptors) {
		_create(dev, numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}
};

struct Dx12DescriptorAllocator_ColorBuffer : public Dx12DescriptorAllocator {
	void create(Dx12DescriptorHeapPool_ColorBuffer& heapPool, Int size) { return _create(heapPool, size); }
	
	Dx12Descriptor_ColorBuffer setRenderTargetView(Int index, Dx12Resource_ColorBuffer& res) {
		auto h = _getHandle<Dx12Descriptor_ColorBuffer>(index);
		_dev->CreateRenderTargetView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_ColorBuffer addRenderTargetView(Dx12Resource_ColorBuffer& res) {
		return setRenderTargetView(_addHandle(), res);
	}
};

class Dx12DescriptorHeapPool_DepthBuffer : public Dx12DescriptorHeapPool {
public:
	void create(Dx12_ID3D12Device* dev, Int numDescriptors) {
		_create(dev, numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}
};

struct Dx12DescriptorAllocator_DepthBuffer : public Dx12DescriptorAllocator {
	void create(Dx12DescriptorHeapPool_DepthBuffer& heapPool, Int size) { return _create(heapPool, size); }
	
	Dx12Descriptor_DepthBuffer setDepthStencilView(Int index, Dx12Resource_DepthBuffer& res) {
		//	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		auto h = _getHandle<Dx12Descriptor_DepthBuffer>(index);
		_dev->CreateDepthStencilView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_DepthBuffer addDepthStencilView(Dx12Resource_DepthBuffer& res) {
		return setDepthStencilView(_addHandle(), res);
	}
};

class Dx12DescriptorHeapPool_Sampler : public Dx12DescriptorHeapPool {
public:
	void create(Dx12_ID3D12Device* dev, Int numDescriptors) {
		_create(dev, numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}
};

struct Dx12DescriptorAllocator_Sampler : public Dx12DescriptorAllocator {
	void create(Dx12DescriptorHeapPool_Sampler& heapPool, Int size) { return _create(heapPool, size); }
	
	Dx12Descriptor_Sampler setSampler(Int index, SamplerFilter filter, SamplerWrapUVW wrap) {
		D3D12_SAMPLER_DESC desc;
		desc.Filter           = Dx12Util::getDxSamplerFilter(filter);
		desc.AddressU         = Dx12Util::getDxSamplerWrap(wrap.u);
		desc.AddressV         = Dx12Util::getDxSamplerWrap(wrap.v);
		desc.AddressW         = Dx12Util::getDxSamplerWrap(wrap.w);
		desc.MipLODBias       = 0;
		desc.MaxAnisotropy    = 0;
		desc.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
		desc.BorderColor[0]   = 0;
		desc.BorderColor[1]   = 0;
		desc.BorderColor[2]   = 0;
		desc.BorderColor[3]   = 0;
		desc.MinLOD           = 0.0f;
		desc.MaxLOD           = D3D12_FLOAT32_MAX;

		auto h = _getHandle<Dx12Descriptor_Sampler>(index);
		_dev->CreateSampler(&desc, h.handle.cpu);
		return h;
	}

	Dx12Descriptor_Sampler addSampler(SamplerFilter filter, SamplerWrapUVW wrap) {
		return setSampler(_addHandle(), filter, wrap);
	}
};

class Dx12DescriptorHeapPool_CBV_SRV_UAV : public Dx12DescriptorHeapPool {
public:
	void create(Dx12_ID3D12Device* dev, Int numDescriptorsPerChunk) {
		_create(dev, numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}
};

struct Dx12DescriptorAllocator_CBV_SRV_UAV : public Dx12DescriptorAllocator {
	void create(Dx12DescriptorHeapPool_CBV_SRV_UAV& heapPool, Int size) { return _create(heapPool, size); }
	
	Dx12Descriptor_ConstBuffer setCBV(Int index, const Dx12Resource_GpuBuffer& res) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = ax_const_cast(res).gpuAddress();
		desc.SizeInBytes    = Dx12Util::castUINT(res.bufferSize());
		auto h = _getHandle<Dx12Descriptor_ConstBuffer>(index);
		_dev->CreateConstantBufferView(&desc, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_ConstBuffer addCBV(const Dx12Resource_GpuBuffer& res) {
		return setCBV(_addHandle(), res);
	}

	Dx12Descriptor_UAV setUAV(Int index, const Dx12Resource_GpuBuffer& buf) {
		auto h = _getHandle<Dx12Descriptor_UAV>(index);
		_dev->CreateUnorderedAccessView(ax_const_cast(buf).d3dResource(),
																nullptr,
																nullptr,
																h.handle.cpu);
		return h;
	}
	Dx12Descriptor_UAV addUAV(Int index, const Dx12Resource_GpuBuffer& buf) {
		return setUAV(_addHandle(), buf);
	}
	
	Dx12Descriptor_RawUAV setTypelessUAV(Int index, const Dx12Resource_GpuBuffer& buf) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = Dx12Util::castUINT(buf.dataSize() / 4);
		desc.Buffer.StructureByteStride = 0;
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		auto h = _getHandle<Dx12Descriptor_RawUAV>(index);
		_dev->CreateUnorderedAccessView(ax_const_cast(buf).d3dResource(),
																nullptr,
																&desc,
																h.handle.cpu);
		return h;
	}
	Dx12Descriptor_RawUAV addTypelessUAV(const Dx12Resource_GpuBuffer& buf) {
		return setTypelessUAV(_addHandle(), buf);
	}

	Dx12Descriptor_Texture2D setTexture(Int index, const Dx12Resource_Texture2D& res) {
		//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		auto h = _getHandle<Dx12Descriptor_Texture2D>(index);
		_dev->CreateShaderResourceView(ax_const_cast(res).d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
	
	Dx12Descriptor_Texture2D setTexture(Int index, Dx12DescriptorHandle srcHandle) {
		auto dst = _getHandle<Dx12Descriptor_Texture2D>(index);
		_dev->CopyDescriptorsSimple(1, dst.handle.cpu, srcHandle.cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return dst;
	}

	Dx12Descriptor_Texture2D addTexture(Dx12DescriptorHandle srcHandle) {
		return setTexture(_addHandle(), srcHandle);
	}
};


} // namespace

#endif