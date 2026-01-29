module AxRender;
import :Dx12Resource;
import :RenderSystem_Dx12;

#if AX_RENDERER_DX12

namespace ax {
void Dx12ResourceBase::setName(InNameId name) {
	_name = name;
	
	auto hr = _d3dResource->SetName(Fmt(L"{}", name).c_str());
	Dx12Util::throwIfError(hr);
}

Dx12ResourceBase::Dx12ResourceBase() {
	_reset();
}

void Dx12ResourceBase::_reset() {
	_resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	_resourceDesc.Alignment = 0;
	_resourceDesc.Width = 0;
	_resourceDesc.Height = 1;
	_resourceDesc.DepthOrArraySize = 1;
	_resourceDesc.MipLevels = 1;
	_resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	_resourceDesc.SampleDesc.Count = 1;
	_resourceDesc.SampleDesc.Quality = 0;
	_resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	_resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	_resourceState = D3D12_RESOURCE_STATE_COMMON;
	_bufferSize = 0;

	_d3dResource.unref();
	_d3d12maAllocation.unref();
}

void Dx12ResourceBase::_create(const D3D12_CLEAR_VALUE* clearValue) {
	AX_ASSERT(_resourceDesc.Width > 0);

	if (_virtualAddressOnly) {
		auto* dev = RenderSystem_Dx12::s_d3dDevice();
		auto hr = dev->CreateReservedResource(&_resourceDesc, _resourceState, clearValue, IID_PPV_ARGS(_d3dResource.ptrForInit()));
		Dx12Util::throwIfError(hr);
		
	} else {
		auto* sys = RenderSystem_Dx12::s_instance();
		auto hr = sys->d3dAllocator()->CreateResource(&_allocDesc, &_resourceDesc, _resourceState, clearValue, 
														_d3d12maAllocation.ptrForInit(), IID_PPV_ARGS(_d3dResource.ptrForInit()));
		Dx12Util::throwIfError(hr);
	}
	
	_resourceDesc = _d3dResource->GetDesc();
}

void Dx12ResourceBase::destroy() {
	_reset();
}

void Dx12Resource_GpuBuffer::create(GpuBufferType type, Int bufferSize, bool virtualAddressOnly) {
	destroy();
	
	_bufferType = type;
	_virtualAddressOnly = virtualAddressOnly;

	Int alignment = 0;
	switch (type) {
		case GpuBufferType::Vertex: {
			_resourceState      = D3D12_RESOURCE_STATE_COMMON;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		}break;

		case GpuBufferType::Index: {
			_resourceState      = D3D12_RESOURCE_STATE_COMMON;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		}break;

		case GpuBufferType::Const: {
			alignment           = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			_resourceState      = D3D12_RESOURCE_STATE_COMMON;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		}break;

		case GpuBufferType::StagingToGpu: {
			_resourceState      = D3D12_RESOURCE_STATE_GENERIC_READ;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case GpuBufferType::StagingToCpu: {
			_resourceState      = D3D12_RESOURCE_STATE_COMMON;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_READBACK;
		}break;

		case GpuBufferType::Structured: {
			_resourceState      = D3D12_RESOURCE_STATE_COMMON; // | D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			_resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		case GpuBufferType::RayTracingShaderRecord: {
			alignment           = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
			_resourceState      = D3D12_RESOURCE_STATE_COMMON;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		}break;

		case GpuBufferType::RayTracingInstanceDesc: {
			alignment           = D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT;
			_resourceState      = D3D12_RESOURCE_STATE_COMMON;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		}break;

		case GpuBufferType::RayTracingScratch: {
			_resourceState      = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			_resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		} break;

		case GpuBufferType::RayTracingAccelStruct: {
			_resourceState      = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
			_resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			_allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		}break;

		default: throw Error_Undefined();
	}

	if (alignment > 0) {
		bufferSize = Math::alignTo(bufferSize, alignment);
	}

	_bufferSize = bufferSize;
	_resourceDesc.Width  = ax_safe_cast_from(bufferSize);
	_resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	_create();
	
	_gpuAddress = _d3dResource->GetGPUVirtualAddress();
}

MutByteSpan Dx12ResourceBase::_mapMemory(IntRange range) {
	D3D12_RANGE dxRange;
	dxRange.Begin = ax_safe_cast_from(range.start());
	dxRange.End   = ax_safe_cast_from(range.stop());

	UINT8* dst = nullptr;
	auto hr = _d3dResource->Map(0, &dxRange, reinterpret_cast<void**>(&dst));
	Dx12Util::throwIfError(hr);
	
	dst += dxRange.Begin; // ID3D12Resource::Map() ppData pointer returned is never offset by any values in pReadRange.
	return MutByteSpan(dst, range.size());
}

void Dx12ResourceBase::_unmapMemory() {
	_d3dResource->Unmap(0, nullptr);
}

void Dx12ResourceBase::uploadToGpu(Int offset, ByteSpan data) {
	D3D12_RANGE readRange = {}; // We do not intend to read from this resource on the CPU.
	UINT8* dst = nullptr;

	if (offset < 0 || offset + data.size() > _bufferSize)
		throw Error_Undefined();
	
	auto hr = _d3dResource->Map(0, &readRange, reinterpret_cast<void**>(&dst));
	Dx12Util::throwIfError(hr);
	auto scopedMap = ScopedLambda([&]() { _d3dResource->Unmap(0, nullptr); });
	
	MemUtil::copy(dst + offset, data.data(), data.sizeInBytes());
}

D3D12_RESOURCE_STATES Dx12ResourceBase::resourceBarrier_Debug(ID3D12GraphicsCommandList* cmdList,
                                                              D3D12_RESOURCE_STATES      newResourceState,
                                                              const SrcLoc&              srcLoc
) {
	if (_resourceState != newResourceState) {
		AX_LOG("Dx12Debug: [{}:{}] resourceBarrier {} -> {} - file:{}:{}",
		       reinterpret_cast<void*>(_d3dResource.ptr()),
		       _name,
		       _resourceState,
		       newResourceState,
		       FilePath::basename(srcLoc.file(), true), srcLoc.line());
	}
	return resourceBarrier(cmdList, newResourceState);
}

D3D12_RESOURCE_STATES Dx12ResourceBase::resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newResourceState) {
	if (!_d3dResource) throw Error_Undefined();
	if (_resourceState == newResourceState) return _resourceState;
	
//	if (_resourceState & newResourceState) return newResourceState; // only change when does not have specify bit

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = _d3dResource;
	barrier.Transition.StateBefore = _resourceState;
	barrier.Transition.StateAfter  = newResourceState;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(1, &barrier);

	auto oldState = _resourceState;
	_resourceState = newResourceState;
	return oldState;
}

void Dx12Resource_ColorBuffer::createFromSwapChain(AX_IDXGISwapChain* swapChain, UINT backBufIndex) {
	auto hr = swapChain->GetBuffer(backBufIndex, IID_PPV_ARGS(_d3dResource.ptrForInit()));
	Dx12Util::throwIfError(hr);
	_resourceDesc = _d3dResource->GetDesc();
	_resourceState = D3D12_RESOURCE_STATE_PRESENT;
}

void Dx12Resource_ColorBuffer::create(Vec2i size, ColorType colorType, const D3D12_CLEAR_VALUE& clearValue) {
	_resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_resourceDesc.Format    = Dx12Util::getDxColorType(colorType);
	_resourceDesc.Width     = Dx12Util::castUINT(size.x);
	_resourceDesc.Height    = Dx12Util::castUINT(size.y);
	_resourceDesc.MipLevels = 0;
	_resourceDesc.Flags     = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	
	_resourceState = D3D12_RESOURCE_STATE_PRESENT;
	_create(&clearValue);
}

void Dx12Resource_DepthBuffer::create(Vec2i size, RenderDepthType depthType, const D3D12_CLEAR_VALUE& clearValue) {
	_resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_resourceDesc.MipLevels = 0;
	_resourceDesc.Format    = Dx12Util::getDxDepthType(depthType);
	_resourceDesc.Flags     = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	_resourceDesc.Width     = Dx12Util::castUINT(size.x);
	_resourceDesc.Height    = Dx12Util::castUINT(size.y);
	_resourceState  = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	_create(&clearValue);
}

void Dx12Resource_Texture2D::create(Vec2i size, Int mipmapCount, ColorType colorType) {
	_resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_resourceDesc.Format    = Dx12Util::getDxColorType(colorType);
	_resourceDesc.Width     = Dx12Util::castUINT(size.x);
	_resourceDesc.Height    = Dx12Util::castUINT(size.y);
	_resourceDesc.MipLevels = Dx12Util::castUINT16(mipmapCount);
	_resourceState  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	_create();
}

void Dx12Fence::create(ID3D12Device* dev, u64 initialValue) {
	auto hr = RenderSystem_Dx12::s_d3dDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

void Dx12CommandQueue::create(ID3D12Device* dev) {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority	= D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags		= D3D12_COMMAND_QUEUE_FLAG_NONE; // D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
	queueDesc.NodeMask	= 0;
	
	auto hr = dev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_queue.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

void Dx12SwapChain::create(Dx12CommandQueue& cmdQueue, HWND hwnd, DXGI_SWAP_CHAIN_DESC1& desc) {
	auto* factory = RenderSystem_Dx12::s_instance()->dxgiFactory();
	
	HRESULT hr;
	
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = factory->CreateSwapChainForHwnd(cmdQueue, hwnd, &desc, nullptr, nullptr, swapChain1.ptrForInit());
	Dx12Util::throwIfError(hr);

	hr = swapChain1->QueryInterface(IID_PPV_ARGS(_swapChain.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

void Dx12SwapChain::present(UINT SyncInterval, UINT Flags) {
	auto hr = _swapChain->Present(SyncInterval, Flags);
	Dx12Util::throwIfError(hr);
}

void Dx12RootParameterList::createRootSignature(ComPtr<ID3D12RootSignature> &outRootSignature) {
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
	auto& desc = rootSignatureDesc.Desc_1_0;
	desc = {};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
#if AX_RENDER_BINDLESS
	desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
	desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
#endif
	desc.NumParameters		= Dx12Util::castUINT(parameters.size());
	desc.pParameters		= parameters.data();
	desc.NumStaticSamplers	= Dx12Util::castUINT(staticSamplers.size());
	desc.pStaticSamplers	= staticSamplers.data();

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> errorBlob;
	auto hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc,
												   blob.ptrForInit(),
												   errorBlob.ptrForInit());
	Dx12Util::throwIfError(hr, errorBlob);

	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	hr = dev->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), 
										IID_PPV_ARGS(outRootSignature.ptrForInit()));
	Dx12Util::throwIfError(hr);	
}

} // namespace

#endif //#if AX_RENDERER_DX12
