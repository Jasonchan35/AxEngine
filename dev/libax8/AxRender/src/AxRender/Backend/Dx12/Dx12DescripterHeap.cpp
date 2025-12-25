module AxRender;
import :Dx12DescripterHeap;

#if AX_RENDERER_DX12

namespace  ax {

void Dx12DescripterHeap::create(Dx12_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc) {
	destroy();
	
	auto hr = dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_d3dHeap.ptrForInit()));
	Dx12Util::throwIfError(hr);

	_stride = dev->GetDescriptorHandleIncrementSize(desc.Type);
	_size   = ax_safe_cast_from(desc.NumDescriptors);
	
	_startHandle.cpu = _d3dHeap->GetCPUDescriptorHandleForHeapStart();
	if (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
		_startHandle.gpu = _d3dHeap->GetGPUDescriptorHandleForHeapStart();
	}
}

void Dx12DescripterHeap::destroy() {
	_d3dHeap.unref();
	_startHandle = {};
	_size        = 0;
	_stride      = 0;
	_used        = 0;
}

void Dx12DescripterHeapPool::destroy() {
	_chunks.clear();
	_desc.NumDescriptors = 0;
}

void Dx12DescripterHeapPool::reset() {
	for (auto& chunk : _chunks) {
		chunk.reset();
	}
	_currentChunk = 0;
}

void Dx12DescripterHeapPool::_create(Int numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	destroy();
	_desc.NumDescriptors = ax_safe_cast_from(numDescriptorsPerChunk);
	_desc.Type  = type;
	_desc.Flags = flags;
}

} // namespace

#endif // #if AX_RENDERER_DX12

