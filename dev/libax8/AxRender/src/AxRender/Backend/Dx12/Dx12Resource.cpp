module AxRender;
import :Dx12Resource;
import :Renderer_Dx12;

#if AX_RENDERER_DX12

namespace ax {
Dx12ResourceBase::Dx12ResourceBase() {
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	_desc.Alignment = 0;
	_desc.Width = 0;
	_desc.Height = 1;
	_desc.DepthOrArraySize = 1;
	_desc.MipLevels = 1;
	_desc.Format = DXGI_FORMAT_UNKNOWN;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	_heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	_heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	_heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	_heapProps.CreationNodeMask = 1;
	_heapProps.VisibleNodeMask = 1;

	_resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
}

void Dx12ResourceBase::_create(const D3D12_CLEAR_VALUE* clearValue) {
	auto hr = Renderer_Dx12::s_d3dDevice()->CreateCommittedResource(
		&_heapProps,
		D3D12_HEAP_FLAG_NONE,
		&_desc,
		_resourceState,
		clearValue,
		IID_PPV_ARGS(_d3dResource.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

void Dx12ResourceBase::destroy() {
	_d3dResource.unref();
	_dataSize = 0;
	_alignedDataSize = 0;
}

void Dx12Resource_GpuBuffer::create(GpuBufferType type, Int bufferSize) {
	destroy();

	_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Int alignment = 0;
	switch (type) {
		case GpuBufferType::Vertex: {
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case GpuBufferType::Index: {
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case GpuBufferType::Uniform: {
			alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case GpuBufferType::StagingToGpu: {
			_resourceState	= D3D12_RESOURCE_STATE_GENERIC_READ;
			_heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}break;

		case GpuBufferType::StagingToCpu: {
			alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			_resourceState = D3D12_RESOURCE_STATE_COPY_DEST;
			_heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		}break;

		case GpuBufferType::Storage: {
			alignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
			// _resourceState	= D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			_resourceState	= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE; // | D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			//			_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			_desc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		case GpuBufferType::RayTracingShaderRecord: {
			alignment = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
		}break;

		case GpuBufferType::RayTracingInstanceDesc: {
			alignment = D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT;
		}break;

		case GpuBufferType::RayTracingScratch: {
			_resourceState	= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			_desc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		case GpuBufferType::RayTracingAccelStruct: {
			_resourceState	= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
			_desc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}break;

		default: throw Error_Undefined();
	}

	if (bufferSize <= 0) {
		return;
	}

	_alignedDataSize = bufferSize;
	if (alignment > 0) {
		_alignedDataSize = Math::alignTo(_alignedDataSize, alignment);
	}
	_desc.Width = Dx12Util::castUINT(_alignedDataSize);

	_create();

	_dataSize = bufferSize;
}


MutByteSpan Dx12ResourceBase::_mapMemory(IntRange range) {
	D3D12_RANGE dxRange;
	dxRange.Begin = SafeCast(range.begin());
	dxRange.End   = SafeCast(range.end());

	UINT8* dst = nullptr;
	auto hr = _d3dResource->Map(0, &dxRange, reinterpret_cast<void**>(&dst));
	Dx12Util::throwIfError(hr);
	return MutByteSpan(dst, range.size());
}

void Dx12ResourceBase::_unmapMemory() {
	_d3dResource->Unmap(0, nullptr);
}

void Dx12ResourceBase::uploadToGpu(Int offset, ByteSpan data) {
	D3D12_RANGE readRange = {}; // We do not intend to read from this resource on the CPU.
	UINT8* dst = nullptr;

	auto hr = _d3dResource->Map(0, &readRange, reinterpret_cast<void**>(&dst));
	Dx12Util::throwIfError(hr);

	if (offset < 0 || offset + data.size() > _dataSize)
		throw Error_Undefined();

	MemUtil::copy(dst + offset, data.data(), data.sizeInBytes());
	_d3dResource->Unmap(0, nullptr);
}

void Dx12ResourceBase::resourceBarrier(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES state) {
	if (!_d3dResource) throw Error_Undefined();
	if (_resourceState & state) return; // only change when does not have specify bit

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type				   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags				   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = _d3dResource;
	barrier.Transition.StateBefore = _resourceState;
	barrier.Transition.StateAfter  = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(1, &barrier);

	_resourceState = state;
}

void Dx12Resource_ColorBuffer::createFromSwapChain(AX_DX12_IDXGISwapChain* swapChain, UINT backBufIndex) {
	auto hr = swapChain->GetBuffer(backBufIndex, IID_PPV_ARGS(_d3dResource.ptrForInit()));
	Dx12Util::throwIfError(hr);
	_desc = _d3dResource->GetDesc();
	_resourceState = D3D12_RESOURCE_STATE_PRESENT;

	D3D12_HEAP_FLAGS flags;
	_d3dResource->GetHeapProperties(&_heapProps, &flags);
}

void Dx12Resource_DepthBuffer::create(Vec2i size) {
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_desc.MipLevels = 0;
	_desc.Format = DXGI_FORMAT_D32_FLOAT;
	_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	_resourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	_desc.Width  = Dx12Util::castUINT(size.x);
	_desc.Height = Dx12Util::castUINT(size.y);

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = _desc.Format;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	_create(&clearValue);
}

void Dx12Resource_Texture2D::create(Vec2i size, Int mipmapCount, ColorType colorType) {
	_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	_desc.Format = Dx12Util::getDxColorType(colorType);
	_resourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	_desc.Width     = Dx12Util::castUINT(size.x);
	_desc.Height    = Dx12Util::castUINT(size.y);
	_desc.MipLevels = Dx12Util::castUINT16(mipmapCount);

	_create();
}

void Dx12Fence::create(ID3D12Device* dev) {
	// _event = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
	auto hr = Renderer_Dx12::s_d3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ptrForInit()));
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
	auto* factory = Renderer_Dx12::s_instance()->dxgiFactory();
	
	HRESULT hr;
	
	ComPtr<IDXGISwapChain1> swapChain1;
	hr = factory->CreateSwapChainForHwnd(cmdQueue, hwnd, &desc, nullptr, nullptr, swapChain1.ptrForInit());
	Dx12Util::throwIfError(hr);

	hr = swapChain1->QueryInterface(IID_PPV_ARGS(_swapChain.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

} // namespace

#endif //#if AX_RENDERER_DX12
