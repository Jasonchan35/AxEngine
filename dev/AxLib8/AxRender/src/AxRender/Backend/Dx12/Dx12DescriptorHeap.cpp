module AxRender;
import :Dx12DescriptorHeap;

#if AX_RENDERER_DX12

namespace  ax {

void Dx12DescriptorHeap::create(InNameId name, AX_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc) {
	destroy();
	_name   = name;
	_desc   = desc;
	
	// AX_LOG("Dx12DescriptorHeap::create        name={:30} size={:8} type={:12}",
	//        name,
	//        desc.NumDescriptors,
	//        desc.Type);
	
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
	_desc        = {};
	_startHandle = {};
	_size        = 0;
	_stride      = 0;
	_used        = 0;
}

void Dx12DescriptorHeapPool::destroy() {
	_heap.destroy();
}

void Dx12DescriptorHeapPool::reset() {
	_heap.reset();
}

void Dx12DescriptorHeapPool::_onCreateChunk(Dx12DescriptorHeapChunk& outHeapChunk, InNameId name, Int size, bool fullyUsed) {
	// AX_LOG("  Dx12DescriptorHeapChunk::create name={:30} size={:8} type={:12} from Pool({})-remain={}",
	//        name,
	//        size,
	//        _heap.desc().Type,
	//        _heap.name(),                        
	//        _heap.remain() - size);
	
	auto h = _heap.addHandle(size);
	outHeapChunk._name        = name;
	outHeapChunk._startHandle = h;
	outHeapChunk._size        = size;
	outHeapChunk._used        = fullyUsed ? size : 0;
	outHeapChunk._d3dHeap     = _heap.d3dHeap();
	outHeapChunk._stride      = _heap.stride();
	outHeapChunk._dev         = _dev;
}

void Dx12DescriptorHeapPool::_create(InNameId name, AX_ID3D12Device* dev, Int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
//	AX_LOG("Dx12DescriptorHeapPool::create name={:20} size={:8} type={}", name, desc.NumDescriptors, desc.Type);
	
	destroy();
	_dev = dev;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = ax_safe_cast_from(numDescriptors);
	desc.Type  = type;
	desc.Flags = flags;
	_heap.create(name, dev, desc);
}

} // namespace

#endif // #if AX_RENDERER_DX12

