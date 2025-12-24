module;

export module AxRender:Dx12DescripterHeap;
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
};

struct Dx12DescripterHeapReserved {
	Dx12DescriptorHandle  handle;
	ID3D12DescriptorHeap* d3dHeap = nullptr;
};

class Dx12DescripterHeap_Base : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;

	void destroy();
	void reset();

	Dx12DescripterHeapReserved reserveHandles(Dx12_ID3D12Device* dev, Int count);

protected:
	void _create(Int numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	template<class HANDLE>
	HANDLE _allocHandle() {
		if (_currentChunk >= _chunks.size()) throw Error_Undefined();
		auto& chunk = _chunks[_currentChunk];
		if (chunk.remain() < 1) throw Error_Undefined();
		
		HANDLE h;
		h.handle = chunk.currentHandle();
		++chunk._used;
		return h;
	}

	struct Chunk {
		ComPtr<ID3D12DescriptorHeap> _d3dHeap;
		Dx12DescriptorHandle         _startHandle;
		Int                          _size   = 0;
		Int                          _used   = 0;
		Int                          _stride = 0;
		Int                          remain() const { return _size - _used; }

		Dx12DescriptorHandle currentHandle() {
			Dx12DescriptorHandle h = _startHandle;
			Int offset = _stride * _used;
			if (h.cpu.ptr) h.cpu.ptr += offset;
			if (h.gpu.ptr) h.gpu.ptr += offset;
			return h;
		}
		
		Chunk(Dx12_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc);
	};

	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	Array<Chunk>					_chunks;
	Int								_currentChunk = 0;
};

struct Dx12DescriptorHandle_ColorBuffer { Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_ColorBuffer : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	Dx12DescriptorHandle_ColorBuffer addRenderTargetView(Dx12_ID3D12Device* dev, Dx12Resource_ColorBuffer& res) {
		auto h = _allocHandle<Dx12DescriptorHandle_ColorBuffer>();
		dev->CreateRenderTargetView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
};

struct Dx12DescriptorHandle_DepthBuffer { Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_DepthBuffer : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	Dx12DescriptorHandle_DepthBuffer addDepthStencilView(Dx12_ID3D12Device* dev, Dx12Resource_DepthBuffer& res) {
		//	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		auto h = _allocHandle<Dx12DescriptorHandle_DepthBuffer>();
		dev->CreateDepthStencilView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
};

struct Dx12DescriptorHandle_Sampler	{ Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_Sampler : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	Dx12DescriptorHandle_Sampler addSampler(Dx12_ID3D12Device* dev, SamplerFilter filter, SamplerWrapUVW wrap) {
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

		auto h = _allocHandle<Dx12DescriptorHandle_Sampler>();
		dev->CreateSampler(&desc, h.handle.cpu);
		return h;
	}	
};

struct Dx12DescriptorHandle_ConstBuffer  { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_UAV			 { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_RawUAV		 { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_Texture		 { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_Texture2D	 { Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_CBV_SRV_UAV : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	Dx12DescriptorHandle_ConstBuffer addCBV(Dx12_ID3D12Device* dev, const Dx12Resource_GpuBuffer& res) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = ax_const_cast(res).gpuAddress();
		desc.SizeInBytes    = Dx12Util::castUINT(res.bufferSize());
		auto h = _allocHandle<Dx12DescriptorHandle_ConstBuffer>();
		dev->CreateConstantBufferView(&desc, h.handle.cpu);
		return h;
	}

	Dx12DescriptorHandle_UAV addUAV(Dx12_ID3D12Device* dev, const Dx12Resource_GpuBuffer& buf) {
		auto h = _allocHandle<Dx12DescriptorHandle_UAV>();
		dev->CreateUnorderedAccessView(ax_const_cast(buf).d3dResource(),
																nullptr,
																nullptr,
																h.handle.cpu);
		return h;
	}

	Dx12DescriptorHandle_RawUAV addTypelessUAV(Dx12_ID3D12Device* dev, const Dx12Resource_GpuBuffer& buf) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = Dx12Util::castUINT(buf.dataSize() / 4);
		desc.Buffer.StructureByteStride = 0;
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		auto h = _allocHandle<Dx12DescriptorHandle_RawUAV>();
		dev->CreateUnorderedAccessView(ax_const_cast(buf).d3dResource(),
																nullptr,
																&desc,
																h.handle.cpu);
		return h;
	}

	Dx12DescriptorHandle_Texture2D addTexture(Dx12_ID3D12Device* dev, const Dx12Resource_Texture2D& res) {
		//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		auto h = _allocHandle<Dx12DescriptorHandle_Texture2D>();
		dev->CreateShaderResourceView(ax_const_cast(res).d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
};

} // namespace

#endif