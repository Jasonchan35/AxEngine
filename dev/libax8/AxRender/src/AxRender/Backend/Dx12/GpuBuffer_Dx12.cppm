module;

export module AxRender:GpuBuffer_Dx12;
import :Dx12Resource;
import :GpuBuffer_Backend;

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {

class GpuBuffer_Dx12 : public GpuBuffer_Backend {
	AX_RTTI_INFO(GpuBuffer_Dx12, GpuBuffer_Backend)
public:
	

	void onCreate(const CreateDesc& desc, Int bufferSize) {
		_p.create(desc.bufferType, bufferSize);
	}

	virtual void onUploadToGpu(Int offset, ByteSpan data) final {
		_p.uploadToGpu(offset, data);
	}

	Dx12Resource_Buffer& resource() { return _p; }

	ID3D12Resource*	d3dResource() { return _p.d3dResource(); }
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress() { return _p.gpuAddress(); }


private:
	Dx12Resource_Buffer	_p;
};

class StorageBuffer_Dx12 : public GpuBuffer_Dx12 {
	AX_RTTI_INFO(StorageBuffer_Dx12, GpuBuffer_Dx12)
public:
	

	StorageBuffer_Dx12(CreateDesc& desc) : Base(desc) {}

	GpuBuffer_Dx12* gpuBuffer() { return rttiCastCheck<GpuBuffer_Dx12>(_gpuBuffer.ptr()); }
	ID3D12Resource*	d3dResource() { return gpuBuffer()->d3dResource(); }
};

class StageBuffer_Dx12 : public GpuBuffer_Dx12 {
	AX_RTTI_INFO(StageBuffer_Dx12, GpuBuffer_Dx12)
public:
	

	StageBuffer_Dx12(CreateDesc& desc) : Base(desc) {}

	GpuBuffer_Dx12* gpuBuffer() { return rttiCastCheck<GpuBuffer_Dx12>(_gpuBuffer.ptr()); }

	ID3D12Resource*	d3dResource() { return gpuBuffer()->d3dResource(); }

	void* onMap() override {
		auto* buf = gpuBuffer();

		D3D12_RANGE readRange = {0, Dx12Util::castUINT64(buf->bufferSize()) }; // We do not intend to read from this resource on the CPU.
		UINT8* dst = nullptr;

		auto hr = buf->d3dResource()->Map(0, &readRange, reinterpret_cast<void**>(&dst));
		Dx12Util::throwIfError(hr);

		return dst;
	}

	void onUnmap() override {
		auto* buf = gpuBuffer();
		if (!buf) throw Error_Undefined();
		buf->d3dResource()->Unmap(0, nullptr);
	}
};


} // namespace

#endif