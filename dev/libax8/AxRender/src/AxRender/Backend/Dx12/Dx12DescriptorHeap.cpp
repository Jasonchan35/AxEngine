module AxRender;
import :Dx12DescriptorHeap;

#if AX_RENDERER_DX12

namespace  ax {

void Dx12DescriptorHeap::create(Dx12_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc) {
	destroy();

	AX_LOG("Dx12DescriptorHeap::create type={} num={}", ax_enum_int(desc.Type), desc.NumDescriptors);
	
	auto hr = dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_d3dHeap.ptrForInit()));
	Dx12Util::throwIfError(hr);

	_stride = dev->GetDescriptorHandleIncrementSize(desc.Type);
	_size   = ax_safe_cast_from(desc.NumDescriptors);
	
	_startHandle.cpu = _d3dHeap->GetCPUDescriptorHandleForHeapStart();
	if (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
		_startHandle.gpu = _d3dHeap->GetGPUDescriptorHandleForHeapStart();
	}
}

void Dx12DescriptorHeap::destroy() {
	_d3dHeap.unref();
	_startHandle = {};
	_size        = 0;
	_stride      = 0;
	_used        = 0;
}

void Dx12DescriptorHeapPool::destroy() {
	_heap.destroy();
	_desc.NumDescriptors = 0;
}

void Dx12DescriptorHeapPool::reset() {
	_heap.reset();
}

void Dx12DescriptorHeapPool::_onCreateAllocator(Dx12DescriptorAllocator& allocator, Int size) {
	// AX_LOG("Dx12DescriptorHeapPool::_createAllocator type={} count={}", ax_enum_int(_desc.Type), count);
	if (size > _desc.NumDescriptors) throw Error_Undefined();
	allocator._startHandle = _heap.currentHandle();
	allocator._size        = size;
	allocator._d3dHeap     = _heap.d3dHeap();
	allocator._stride      = _heap.stride();
	allocator._dev         = _dev;

	_heap.adjustUsed(size);
}

void Dx12DescriptorHeapPool::_create(Dx12_ID3D12Device* dev, Int numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	destroy();
	_dev = dev;
	_desc.NumDescriptors = ax_safe_cast_from(numDescriptorsPerChunk);
	_desc.Type  = type;
	_desc.Flags = flags;
	_heap.create(dev, _desc);
}

} // namespace

#endif // #if AX_RENDERER_DX12

