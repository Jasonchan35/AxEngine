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

} // namespace

#endif