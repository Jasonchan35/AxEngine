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

	Dx12DescriptorHandle operator+(Int offset) const {
		Dx12DescriptorHandle h;
		h.cpu.ptr = cpu.ptr ? cpu.ptr + offset : 0;
		h.gpu.ptr = gpu.ptr ? gpu.ptr + offset : 0;
		return h;
	}
};

struct Dx12DescripterHeap : public NonCopyable {
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


struct Dx12DescripterHeapPool_Block : public NonCopyable {
	template<class HANDLE>
	HANDLE _allocHandle() {
		if (_used >= _size) throw Error_Undefined();
			
		HANDLE h;
		h.handle = _startHandle + _stride * _used;
		_used++;
		return h;
	}

	Int size() const { return _size; }
	Int used() const { return _used; }
	Int remain() const { return _size - _used; }
	ID3D12DescriptorHeap* d3dHeap() const { return _d3dHeap; }
	Dx12DescriptorHandle  startHandle() const { return _startHandle; }

protected:
	friend class Dx12DescripterHeapPool;
		
	Dx12DescriptorHandle  _startHandle;
	Int                   _size    = 0;
	Int                   _used    = 0;
	Int                   _stride  = 0;
	ID3D12DescriptorHeap* _d3dHeap = nullptr;
};

class Dx12DescripterHeapPool : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;
	using BlockBase = Dx12DescripterHeapPool_Block;
	using Chunk     = Dx12DescripterHeap;

	void destroy();
	void reset();

protected:
	void _allocaBlock(BlockBase & block, Dx12_ID3D12Device* dev, Int count) {
		if (count > _desc.NumDescriptors) throw Error_Undefined();
		auto& chunk = _getReserveChunk(dev, count);
		
		block._startHandle = chunk.currentHandle();
		block._size        = count;
		block._d3dHeap     = chunk.d3dHeap();
		block._stride      = chunk.stride();
	}

	void _returnBlockRemain(BlockBase& block) {
		auto& chunk = _chunks[_currentChunk];
		if (chunk.d3dHeap() != block.d3dHeap()) throw Error_Undefined();
		if (chunk.used() < block.remain()) throw Error_Undefined();
		if (block.remain() < 0) throw Error_Undefined();
		if (block.remain() == 0) return;
		chunk.adjustUsed(-block.remain());
		block._used = block._size;
	}	
	
	void _create(Int numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

	Chunk& _getReserveChunk(Dx12_ID3D12Device* dev, Int count) {
		Chunk* outChunk = nullptr;
		for (Int i = _currentChunk; i < _chunks.size(); ++i) {
			auto& chunk = _chunks[i]; 
			if (chunk.remain() >= count) {
				_currentChunk = i;
				outChunk = &chunk;
				break;
			}
		}

		if (!outChunk) {
			_currentChunk = _chunks.size();
			outChunk = &_chunks.emplaceBack();
			outChunk->create(dev, _desc);
		}
		
		outChunk->adjustUsed(count);
		return *outChunk;
	}
	
	D3D12_DESCRIPTOR_HEAP_DESC		_desc = {};
	Array<Chunk>					_chunks;
	Int								_currentChunk = 0;
};

struct Dx12DescriptorHandle_ColorBuffer { Dx12DescriptorHandle handle; };

class Dx12DescripterHeapPool_ColorBuffer : public Dx12DescripterHeapPool {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	struct Block : public BlockBase {
		Dx12DescriptorHandle_ColorBuffer addRenderTargetView(Dx12_ID3D12Device* dev, Dx12Resource_ColorBuffer& res) {
			auto h = _allocHandle<Dx12DescriptorHandle_ColorBuffer>();
			dev->CreateRenderTargetView(res.d3dResource(), nullptr, h.handle.cpu);
			return h;
		}
	};

	void allocaBlock(Block& outBlock, Dx12_ID3D12Device* dev, Int size) { return _allocaBlock(outBlock, dev, size); }
	void returnBlockRemain(Block& outBlock) { _returnBlockRemain(outBlock); }
};

struct Dx12DescriptorHandle_DepthBuffer { Dx12DescriptorHandle handle; };

class Dx12DescripterHeapPool_DepthBuffer : public Dx12DescripterHeapPool {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	}

	struct Block : public BlockBase {
		Dx12DescriptorHandle_DepthBuffer addDepthStencilView(Dx12_ID3D12Device* dev, Dx12Resource_DepthBuffer& res) {
			//	D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
			auto h = _allocHandle<Dx12DescriptorHandle_DepthBuffer>();
			dev->CreateDepthStencilView(res.d3dResource(), nullptr, h.handle.cpu);
			return h;
		}
	};	

	void allocaBlock(Block& outBlock, Dx12_ID3D12Device* dev, Int size) { return _allocaBlock(outBlock, dev, size); }
	void returnBlockRemain(Block& outBlock) { _returnBlockRemain(outBlock); }
};

struct Dx12DescriptorHandle_Sampler	{ Dx12DescriptorHandle handle; };

class Dx12DescripterHeapPool_Sampler : public Dx12DescripterHeapPool {
public:
	void create(Int numDescriptors) {
		_create(numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	struct Block : public BlockBase {
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

	void allocaBlock(Block& outBlock, Dx12_ID3D12Device* dev, Int size) { return _allocaBlock(outBlock, dev, size); }
	void returnBlockRemain(Block& outBlock) { _returnBlockRemain(outBlock); }
};

struct Dx12DescriptorHandle_ConstBuffer  { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_UAV			 { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_RawUAV		 { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_Texture		 { Dx12DescriptorHandle handle; };
struct Dx12DescriptorHandle_Texture2D	 { Dx12DescriptorHandle handle; };

class Dx12DescripterHeapPool_CBV_SRV_UAV : public Dx12DescripterHeapPool {
public:
	void create(Int numDescriptorsPerChunk) {
		_create(numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	}

	struct Block : public BlockBase {
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

	void allocaBlock(Block& outBlock, Dx12_ID3D12Device* dev, Int size) { return _allocaBlock(outBlock, dev, size); }
	void returnBlockRemain(Block& outBlock) { _returnBlockRemain(outBlock); }
};

} // namespace

#endif