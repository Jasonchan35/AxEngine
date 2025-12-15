module;

export module AxRender:Dx12DescripterHeap;
import :Dx12Resource;
import :Renderer_Dx12;

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {

struct Dx12DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu = {};

	operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return cpu; }
	operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return gpu; }
};

class Dx12DescripterHeapArray : public NonCopyable {
public:
	void create(Int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	void destroy();
	
	ID3D12DescriptorHeap* d3dHeap() { return _d3dHeap; }
	bool isValid() const { return _d3dHeap.ptr() != nullptr; }

	Dx12DescriptorHandle getHandle(Int index) {
		UINT i = SafeCast(index);
		if (i >= _desc.NumDescriptors) throw Error_Undefined();
		auto o = _handle;
		auto offset = i * _stride;
		o.cpu.ptr += offset;
		o.gpu.ptr += offset;
		return o;
	}

	Int size() const { return static_cast<Int>(_desc.NumDescriptors); }

protected:
	ComPtr<ID3D12DescriptorHeap>	_d3dHeap;
	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	Dx12DescriptorHandle			_handle = {};
	UINT							_stride = 0;
};


class Dx12DescripterHeap_Base : public NonCopyable {
public:
	ID3D12DescriptorHeap*	d3dHeap() { return _d3dHeap; }

	bool isValid() const { return _d3dHeap.ptr() != nullptr; }
	void destroy();

	Dx12DescriptorHandle heapStartHandle() {
		return _heapStartHandle;
	}

	Dx12DescriptorHandle elementHandle(Int i) {
		if (i < 0 || i >= _numDescriptors)
			throw Error_Undefined();

		auto o = _heapStartHandle;
		auto offset = i * _stride;
		o.cpu.ptr += offset;
		o.gpu.ptr += offset;
		return o;
	}

protected:
	void _init(Int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	ComPtr<ID3D12DescriptorHeap>	_d3dHeap;
	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	Int								_numDescriptors = 0;
	Dx12DescriptorHandle			_heapStartHandle;
	UINT							_stride = 0;
};

class Dx12DescripterHeap_ColorBuffer : public Dx12DescripterHeap_Base {
public:
	void init(Int numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	Dx12DescriptorHandle createView(Int i, Dx12Resource_ColorBuffer& res) {
		//	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		auto h = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateRenderTargetView(res.d3dResource(), nullptr, h);
		return h;
	}
};

class Dx12DescripterHeap_DepthStencilBuffer : public Dx12DescripterHeap_Base {
public:
	void init(Int numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	Dx12DescriptorHandle createView(Int i, Dx12Resource_DepthBuffer& res) {
		//	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		auto h = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateDepthStencilView(res.d3dResource(), nullptr, h);
		return h;
	}
};

class Dx12DescripterHeap_CBV_SRV_UAV : public Dx12DescripterHeap_Base {
public:
	void init(Int numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	Dx12DescriptorHandle createViewCBV(Int i, Dx12Resource_Buffer& res) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = res.gpuAddress();
		desc.SizeInBytes    = Dx12Util::castUINT(res.alignedDataSize());

		auto h = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateConstantBufferView(&desc, h);
		return h;
	}

	Dx12DescriptorHandle createViewStructuredUAV(Int i, Dx12Resource_Buffer& buf) {
		auto h = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateUnorderedAccessView(buf.d3dResource(), nullptr, nullptr, h);
		return h;
	}

	Dx12DescriptorHandle createViewRawUAV(Int i, Dx12Resource_Buffer& buf) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = Dx12Util::castUINT(buf.dataSize() / 4);
		desc.Buffer.StructureByteStride = 0;
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		auto h = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateUnorderedAccessView(buf.d3dResource(), nullptr, &desc, h);
		return h;
	}

	Dx12DescriptorHandle createViewSRV(Int i, Dx12Resource_Texture& res) {
		//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		auto h = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateShaderResourceView(res.d3dResource(), nullptr, h);
		return h;
	}
};

class Dx12DescripterHeap_Sampler : public Dx12DescripterHeap_Base {
public:
	void init(Int numDescriptors) {
		_init(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	Dx12DescriptorHandle createSampler(Int i, const D3D12_SAMPLER_DESC& desc) {
		auto o = elementHandle(i);
		Renderer_Dx12::s_d3dDevice()->CreateSampler(&desc, o.cpu);
		return o;
	}
};

} // namespace

#endif