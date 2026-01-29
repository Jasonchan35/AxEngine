module;

export module AxRender:GpuBuffer_Dx12;
import :Dx12Resource;
import :GpuBuffer_Backend;

#if AX_RENDERER_DX12

import :RenderSystem_Backend;

namespace ax {

class GpuBufferPool_Dx12 : public GpuBufferPool_Backend {
	AX_RTTI_INFO(GpuBufferPool_Dx12, GpuBufferPool_Backend)
public:
	GpuBufferPool_Dx12(const CreateDesc& desc);
	static constexpr Int kTileSizeInBytes = 64 * 1024; 

	struct Page_Dx12 : public Page {
		ComPtr<AX_ID3D12Heap>	_d3dHeap;
	};
	
	virtual void onAllocateBlock(GpuBuffer* buf) override { _pagePool.onAllocateBlock(buf, _pageSize); }
	virtual void onFreeBlock(GpuBuffer* buf) override { _pagePool.onFreeBlock(buf, _pageSize); }
	
	virtual void onGpuUpdatePages(RenderRequest_Backend* req_) override;

	PagePool_<Page_Dx12>   _pagePool;
	Dx12Resource_GpuBuffer _resource_dx12;
};

class GpuBuffer_Dx12 : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Dx12, GpuBuffer_Backend)
public:
	GpuBuffer_Dx12(const CreateDesc& desc);
	
	virtual void onUploadToGpu(Int offset, ByteSpan data) final {
		_resourceWithoutPool.uploadToGpu(offset, data);
	}
	
	static Int s_getMinAlignement(GpuBufferType type) {
		return Dx12Resource_GpuBuffer::s_getMinAlignement(type);
	}

	ID3D12Resource*	d3dResource() { return _d3dResource; }
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() const { return _gpuAddress; }

	Dx12Resource_GpuBuffer* resource_dx12() {
		if (_pool) {
			return &rttiCastCheck<GpuBufferPool_Dx12>(_pool)->_resource_dx12;
		} else {
			return &_resourceWithoutPool;
		}
	}
	
protected:
	virtual MutByteSpan	onMapMemory(IntRange range) override	{ return _getResource_dx12()._mapMemory(range + _bufferOffset); }
	virtual void		onUnmapMemory() override				{ return _getResource_dx12()._unmapMemory(); }

	virtual void		onFlush(IntRange range) override { AX_ASSERT_TODO(); }
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) override;

private:
	Dx12Resource_GpuBuffer& _getResource_dx12() {
		return _pool ? rttiCastCheck<GpuBufferPool_Dx12>(_pool)->_resource_dx12 : _resourceWithoutPool;
	}
	
	Dx12Resource_GpuBuffer	_resourceWithoutPool;
	
// pool
	ComPtr<AX_ID3D12Resource>	_d3dResource;
	D3D12_GPU_VIRTUAL_ADDRESS	_gpuAddress = 0;
};

class GpuStructuredBuffer_Dx12 : public GpuStructuredBuffer_Backend {
	AX_RTTI_INFO(GpuStructuredBuffer_Dx12, GpuStructuredBuffer_Backend)
public:
	GpuStructuredBuffer_Dx12(const CreateDesc& desc) : Base(desc) {}
};

} // namespace

#endif