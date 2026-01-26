module;

export module AxRender:GpuBuffer_Dx12;
import :Dx12Resource;
import :GpuBuffer_Backend;

#if AX_RENDERER_DX12

import :RenderSystem_Backend;

namespace ax {

class GpuBuffer_Dx12 : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Dx12, GpuBuffer_Backend)
public:
	GpuBuffer_Dx12(const CreateDesc& desc);

	virtual void onUploadToGpu(Int offset, ByteSpan data) final {
		_p.uploadToGpu(offset, data);
	}

	Dx12Resource_GpuBuffer&       resource() { return _p; }
	const Dx12Resource_GpuBuffer& resource() const { return _p; }

//	ID3D12Resource*	d3dResource() { return _p.d3dResource(); }
//	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() { return _p.gpuAddress(); }

protected:
	struct MemPage {
		ComPtr<ID3D12Heap1>	_d3dHeap;
	};

	struct VirtualMemData {
		Array<MemPage>  _memPages;
	};
	
	UPtr<VirtualMemData>	_virtualMemData;
	
	virtual void onSetCapacity(RenderRequest* req_, Int newCapacity) override;
	
	virtual MutByteSpan	onMapMemory(IntRange range) override	{ return _p._mapMemory(range); }
	virtual void		onUnmapMemory() override				{ return _p._unmapMemory(); }

	virtual void		onFlush(IntRange range) override { AX_ASSERT_TODO(); }
	virtual void		onCopyFromGpuBuffer(RenderRequest* req, GpuBuffer* src, IntRange srcRange, Int dstOffset) override;

private:
	Dx12Resource_GpuBuffer	_p;
};

class GpuVirtualAllocator_Dx12 : public GpuVirtualAllocator_Backend {
	AX_RTTI_INFO(GpuVirtualAllocator_Dx12, GpuVirtualAllocator_Backend)
public:
	GpuVirtualAllocator_Dx12(const CreateDesc& desc) : Base(desc) {}
};

class GpuStructuredBuffer_Dx12 : public GpuStructuredBuffer_Backend {
	AX_RTTI_INFO(GpuStructuredBuffer_Dx12, GpuStructuredBuffer_Backend)
public:
	GpuStructuredBuffer_Dx12(const CreateDesc& desc) : Base(desc) {}
};

} // namespace

#endif