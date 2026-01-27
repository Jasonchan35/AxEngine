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

	template<class CH>
	void onFormat(Format_<CH> & fmt) const {
		fmt << Fmt("Descriptor(cpu={:8x} gpu={:8x})", cpu.ptr, gpu.ptr);
	}
};

struct Dx12Descriptor_ColorBuffer	{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_DepthBuffer	{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_Sampler		{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_ConstBuffer	{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_SRV			{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_UAV			{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_RawUAV		{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_Texture		{ Dx12DescriptorHandle handle; };
struct Dx12Descriptor_Texture2D		{ Dx12DescriptorHandle handle; };


struct Dx12DescriptorHeap : public NonCopyable {
	ID3D12DescriptorHeap* d3dHeap() { return _d3dHeap; }
	
	void create(InNameId name, AX_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc);
	void destroy();
	
	Dx12DescriptorHandle currentHandle() { return getHandle(_used); }
	Dx12DescriptorHandle getHandle(Int index) { return _startHandle + _stride * index; }
	Dx12DescriptorHandle addHandle(Int count) {
		if (count > remain()) throw Error_Undefined();
		auto h = _startHandle + _stride * _used;
		_used += count;
		return h;
	}

	NameId name() const { return _name; }
	Int    size() const { return _size; }
	Int    used() const { return _used; }
	Int    stride() const { return _stride; }
	Int    remain() const { return _size - _used; }

//	void adjustUsed(Int v) { _used += v; }
	void reset() { _used = 0;}

	const D3D12_DESCRIPTOR_HEAP_DESC& desc() const { return _desc; }
	bool isShaderVisible() const { return _desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; }

private:
	ComPtr<ID3D12DescriptorHeap> _d3dHeap;
	Dx12DescriptorHandle         _startHandle;
	NameId                       _name;
	D3D12_DESCRIPTOR_HEAP_DESC   _desc   = {};
	Int                          _size   = 0;
	Int                          _used   = 0;
	Int                          _stride = 0;
};

class Dx12DescriptorHeapPool : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;

	void destroy();
	void reset();

	ID3D12DescriptorHeap* d3dHeap() { return _heap.d3dHeap(); }

	const D3D12_DESCRIPTOR_HEAP_DESC& desc() const	{ return _heap.desc(); }

	bool isShaderVisible() const { return _heap.isShaderVisible(); }

	AX_ID3D12Device* d3dDevice() { return _dev; }

	Dx12DescriptorHandle currentHandle() { return _heap.currentHandle(); }

	AX_INLINE NameId name() const 		{ return _heap.name(); }
	AX_INLINE Int    size() const 		{ return _heap.size(); }
	AX_INLINE Int    used() const 		{ return _heap.used(); }
	AX_INLINE Int    stride() const 	{ return _heap.stride(); }
	AX_INLINE Int    remain() const 	{ return _heap.remain(); }
	
	
protected:
	friend class Dx12DescriptorHeapChunk;
	void _onCreateChunk(Dx12DescriptorHeapChunk & outHeapChunk, InNameId name, Int size, bool fullyUsed);
	void _create(InNameId name, AX_ID3D12Device* dev, Int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	
	AX_ID3D12Device*				_dev = nullptr;
	Dx12DescriptorHeap				_heap;
};

class Dx12DescriptorHeapChunk : public NonCopyable {
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

	NameId name() const { return _name; }

protected:
	void _create(InNameId name, Dx12DescriptorHeapPool& heapPool, Int size, bool fullyUsed) {
		return heapPool._onCreateChunk(*this, name, size, fullyUsed);
	}
	
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
	NameId _name;
	Int                   _size    = 0;
	Int                   _used    = 0;
	Int                   _stride  = 0;
	ID3D12DescriptorHeap* _d3dHeap = nullptr;
	AX_ID3D12Device*    _dev     = nullptr;
};

class Dx12DescriptorHeapPool_ColorBuffer : public Dx12DescriptorHeapPool {
public:
	void create(InNameId name, AX_ID3D12Device* dev, Int numDescriptors) {
		_create(name, dev, numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}
};

struct Dx12DescriptorHeapChunk_ColorBuffer : public Dx12DescriptorHeapChunk {
	using HeapPool = Dx12DescriptorHeapPool_ColorBuffer;

	void create(InNameId name, HeapPool& heapPool, Int size, bool fullyUsed) {
		return _create(name, heapPool, size, fullyUsed);
	}
	
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
	void create(InNameId name, AX_ID3D12Device* dev, Int numDescriptors) {
		_create(name, dev, numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}
};

struct Dx12DescriptorHeapChunk_DepthBuffer : public Dx12DescriptorHeapChunk {
	using HeapPool = Dx12DescriptorHeapPool_DepthBuffer;

	void create(InNameId name, HeapPool& heapPool, Int size, bool fullyUsed) {
		return _create(name, heapPool, size, fullyUsed);
	}
	
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
	void create(InNameId name, AX_ID3D12Device* dev, Int numDescriptors, bool isShaderVisible = true) {
		_create(name, dev,
		        numDescriptors,
		        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		        isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}
};

struct Dx12DescriptorHeapChunk_Sampler : public Dx12DescriptorHeapChunk {
	using HeapPool = Dx12DescriptorHeapPool_Sampler;

	void create(InNameId name, HeapPool& heapPool, Int size, bool fullyUsed) {
		return _create(name, heapPool, size, fullyUsed);
	}
	
	Dx12Descriptor_Sampler setSampler(Int index, const SamplerState& ss) {
		D3D12_SAMPLER_DESC desc;
		desc.Filter           = Dx12Util::getDxSamplerFilter(ss.filter);
		desc.AddressU         = Dx12Util::getDxSamplerWrap(ss.wrap.u);
		desc.AddressV         = Dx12Util::getDxSamplerWrap(ss.wrap.v);
		desc.AddressW         = Dx12Util::getDxSamplerWrap(ss.wrap.w);
		desc.MipLODBias       = 0;
		desc.MaxAnisotropy    = 0;
		desc.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
		desc.BorderColor[0]   = 0;
		desc.BorderColor[1]   = 0;
		desc.BorderColor[2]   = 0;
		desc.BorderColor[3]   = 0;
		desc.MinLOD           = ss.minLOD;
		desc.MaxLOD           = ss.maxLOD;

		auto h = _getHandle<Dx12Descriptor_Sampler>(index);
		_dev->CreateSampler(&desc, h.handle.cpu);
		return h;
	}

	Dx12Descriptor_Sampler addSampler(const SamplerState& ss) {
		return setSampler(_addHandle(), ss);
	}

	Dx12Descriptor_Sampler setDescriptor(Int index, const Dx12Descriptor_Sampler& srcDesc) {
		auto dst = _getHandle<Dx12Descriptor_Sampler>(index);
		_dev->CopyDescriptorsSimple(1, dst.handle.cpu, srcDesc.handle.cpu, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return dst;
	}

	Dx12Descriptor_Sampler addDescriptor(const Dx12Descriptor_Sampler& srcDesc) {
		return setDescriptor(_addHandle(), srcDesc);
	}	
};

class Dx12DescriptorHeapPool_CBV_SRV_UAV : public Dx12DescriptorHeapPool {
public:
	void create(InNameId name, AX_ID3D12Device* dev, Int numDescriptors, bool isShaderVisible = true) {
		_create(name, dev,
		        numDescriptors,
		        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		        isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}
};

struct Dx12DescriptorHeapChunk_CBV_SRV_UAV : public Dx12DescriptorHeapChunk {
	using HeapPool = Dx12DescriptorHeapPool_CBV_SRV_UAV;
	using This = Dx12DescriptorHeapChunk_CBV_SRV_UAV;

	void create(InNameId name, HeapPool& heapPool, Int size, bool fullyUsed) {
		return _create(name, heapPool, size, fullyUsed);
	}
	
	Dx12Descriptor_ConstBuffer setCBV(Int index, const Dx12Resource_GpuBuffer& res) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = ax_const_cast(res).gpuAddress();
		desc.SizeInBytes    = Dx12Util::castUINT(res.bufferSize());
		auto h = _getHandle<Dx12Descriptor_ConstBuffer>(index);
		_dev->CreateConstantBufferView(&desc, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_ConstBuffer addCBV(const Dx12Resource_GpuBuffer& res) { return setCBV(_addHandle(), res); }

	Dx12Descriptor_SRV setSRV(Int index, const Dx12Resource_GpuBuffer& res, Int first, Int count, Int stride) {
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                          = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
		desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Buffer.FirstElement             = ax_safe_cast_from(first);
		desc.Buffer.NumElements              = ax_safe_cast_from(count);
		desc.Buffer.StructureByteStride      = ax_safe_cast_from(stride);
		desc.Buffer.Flags                    = D3D12_BUFFER_SRV_FLAG_NONE;
		auto h                               = _getHandle<Dx12Descriptor_SRV>(index);
		_dev->CreateShaderResourceView(ax_const_cast(res).d3dResource(), &desc, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_SRV addSRV(const Dx12Resource_GpuBuffer& res, Int first, Int count, Int stride) {
		return setSRV(_addHandle(), res, first, count, stride);
	}
	
	Dx12Descriptor_UAV setUAV(Int index, const Dx12Resource_GpuBuffer& buf, Int first, Int count, Int stride, bool isRaw) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format                           = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement              = ax_safe_cast_from(first);
		desc.Buffer.NumElements               = ax_safe_cast_from(count);
		desc.Buffer.StructureByteStride       = ax_safe_cast_from(stride);
		desc.Buffer.Flags                     = isRaw ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE;  
		desc.Buffer.CounterOffsetInBytes      = 0;
		auto h                                = _getHandle<Dx12Descriptor_UAV>(index);
		_dev->CreateUnorderedAccessView(ax_const_cast(buf).d3dResource(), nullptr, &desc, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_UAV addUAV(Int index, const Dx12Resource_GpuBuffer& buf, Int first, Int count, Int stride, bool isRaw) {
		return setUAV(_addHandle(), buf, first, count, stride, isRaw);
	}
	
	Dx12Descriptor_RawUAV setTypelessUAV(Int index, const Dx12Resource_GpuBuffer& buf) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = Dx12Util::castUINT(buf.bufferSize() / 4);
		desc.Buffer.StructureByteStride = 0;
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		auto h = _getHandle<Dx12Descriptor_RawUAV>(index);
		_dev->CreateUnorderedAccessView(ax_const_cast(buf).d3dResource(), nullptr, &desc, h.handle.cpu);
		return h;
	}
	Dx12Descriptor_RawUAV addTypelessUAV(const Dx12Resource_GpuBuffer& buf) {
		return setTypelessUAV(_addHandle(), buf);
	}

	Dx12Descriptor_Texture2D setTexture2D(Int index, const Dx12Resource_Texture2D& res) {
		//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		auto h = _getHandle<Dx12Descriptor_Texture2D>(index);
		_dev->CreateShaderResourceView(ax_const_cast(res).d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
	
	Dx12Descriptor_Texture2D setDescriptor(Int index, const Dx12Descriptor_Texture2D& srcDesc) {
		auto dst = _getHandle<Dx12Descriptor_Texture2D>(index);
		_dev->CopyDescriptorsSimple(1, dst.handle.cpu, srcDesc.handle.cpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return dst;
	}

	Dx12Descriptor_Texture2D addDescriptor(const Dx12Descriptor_Texture2D& srcDesc) {
		return setDescriptor(_addHandle(), srcDesc);
	}
};


} // namespace

#endif