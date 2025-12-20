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

	// operator D3D12_CPU_DESCRIPTOR_HANDLE () const { return cpu; }
	// operator D3D12_GPU_DESCRIPTOR_HANDLE () const { return gpu; }
};

class Dx12DescripterHeap_Base : public NonCopyable {
public:
	ID3D12DescriptorHeap*	d3dHeap() { return _d3dHeap; }

//	bool isValid() const { return _d3dHeap.ptr() != nullptr; }
	UINT numDescriptors() const { return _desc.NumDescriptors; }
	void destroy();

	const Dx12DescriptorHandle& handleStart() const { return _handleStart; }
	
protected:
	void _create(Int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	template<class HANDLE>
	HANDLE _getHandle(Int index) {
		HANDLE h;
		_baseGetHandle(h.handle, index);
		return h;
	}

	void _baseGetHandle(Dx12DescriptorHandle& outHandle, Int index) {
		UINT i = ax_safe_cast_from(index);
		if (i >= _desc.NumDescriptors) throw Error_Undefined();
		auto offset = i * _stride;
		outHandle.cpu.ptr = _handleStart.cpu.ptr + offset;
		outHandle.gpu.ptr = _handleStart.gpu.ptr + offset;
	}

	ComPtr<ID3D12DescriptorHeap>	_d3dHeap;
	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	Dx12DescriptorHandle			_handleStart;
	UINT							_stride = 0;
};

struct Dx12DescriptorHandle_ColorBuffer { Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_ColorBuffer : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	Dx12DescriptorHandle_ColorBuffer setRenderTargetView(Int i, Dx12Resource_ColorBuffer& res) {
		//	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		auto h = _getHandle<Dx12DescriptorHandle_ColorBuffer>(i);
		Renderer_Dx12::s_d3dDevice()->CreateRenderTargetView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
};

struct Dx12DescriptorHandle_DepthBuffer { Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_DepthBuffer : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	Dx12DescriptorHandle_DepthBuffer setDepthStencilView(Int i, Dx12Resource_DepthBuffer& res) {
		//	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		auto h = _getHandle<Dx12DescriptorHandle_DepthBuffer>(i);
		Renderer_Dx12::s_d3dDevice()->CreateDepthStencilView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
};

struct Dx12DescriptorHandle_Sampler	{ Dx12DescriptorHandle handle; };

class Dx12DescripterHeap_Sampler : public Dx12DescripterHeap_Base {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	Dx12DescriptorHandle_Sampler setSampler(Int i, const D3D12_SAMPLER_DESC& desc) {
		auto h = _getHandle<Dx12DescriptorHandle_Sampler>(i);
		Renderer_Dx12::s_d3dDevice()->CreateSampler(&desc, h.handle.cpu);
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

	Dx12DescriptorHandle_ConstBuffer setCBV(Int i, Dx12Resource_GpuBuffer& res) {
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
		desc.BufferLocation = res.gpuAddress();
		desc.SizeInBytes    = Dx12Util::castUINT(res.alignedDataSize());

		auto h = _getHandle<Dx12DescriptorHandle_ConstBuffer>(i);
		Renderer_Dx12::s_d3dDevice()->CreateConstantBufferView(&desc, h.handle.cpu);
		return h;
	}

	Dx12DescriptorHandle_UAV setUAV(Int i, Dx12Resource_GpuBuffer& buf) {
		auto h = _getHandle<Dx12DescriptorHandle_UAV>(i);
		Renderer_Dx12::s_d3dDevice()->CreateUnorderedAccessView(buf.d3dResource(), nullptr, nullptr, h.handle.cpu);
		return h;
	}

	Dx12DescriptorHandle_RawUAV setTypelessUAV(Int i, Dx12Resource_GpuBuffer& buf) {
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = Dx12Util::castUINT(buf.dataSize() / 4);
		desc.Buffer.StructureByteStride = 0;
		desc.Buffer.CounterOffsetInBytes = 0;
		desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		auto h = _getHandle<Dx12DescriptorHandle_RawUAV>(i);
		Renderer_Dx12::s_d3dDevice()->CreateUnorderedAccessView(buf.d3dResource(), nullptr, &desc, h.handle.cpu);
		return h;
	}

	Dx12DescriptorHandle_Texture2D setTexture(Int i, Dx12Resource_Texture2D& res) {
		//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		auto h = _getHandle<Dx12DescriptorHandle_Texture2D>(i);
		Renderer_Dx12::s_d3dDevice()->CreateShaderResourceView(res.d3dResource(), nullptr, h.handle.cpu);
		return h;
	}
};

} // namespace

#endif