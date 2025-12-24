module AxRender;
import :Dx12DescripterHeap;

#if AX_RENDERER_DX12

namespace  ax {

void Dx12DescripterHeap_Base::destroy() {
	_chunks.clear();
	_desc.NumDescriptors = 0;
}

void Dx12DescripterHeap_Base::reset() {
	for (auto& chunk : _chunks) {
		chunk._used = 0;
	}
	_currentChunk = 0;
}

Dx12DescripterHeapReserved Dx12DescripterHeap_Base::reserveHandles(Dx12_ID3D12Device* dev, Int count) {
	if (count > _desc.NumDescriptors) {
		throw Error_Undefined();
	}
	
	Chunk* chunk = nullptr;

	//---- pick chunk ----
	for (Int i = _currentChunk; i < _chunks.size(); ++i) {
		auto& c = _chunks[i]; 
		if (c.remain() >= count) {
			c._used += count;
			
			chunk = &c;
			_currentChunk = i;
			break;
		}
	}
	
	if (!chunk) {
		_currentChunk = _chunks.size();
		chunk = &_chunks.emplaceBack(dev, _desc);
	}

	return {.handle = chunk->currentHandle(), .d3dHeap = chunk->_d3dHeap};
}

void Dx12DescripterHeap_Base::_create(Int numDescriptorsPerChunk, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	destroy();
	_desc.NumDescriptors = ax_safe_cast_from(numDescriptorsPerChunk);
	_desc.Type  = type;
	_desc.Flags = flags;
}

Dx12DescripterHeap_Base::Chunk::Chunk(Dx12_ID3D12Device* dev, D3D12_DESCRIPTOR_HEAP_DESC desc) {
	auto hr = dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_d3dHeap.ptrForInit()));
	Dx12Util::throwIfError(hr);

	_stride = dev->GetDescriptorHandleIncrementSize(desc.Type);
	_size   = ax_safe_cast_from(desc.NumDescriptors);
	
	_startHandle.cpu = _d3dHeap->GetCPUDescriptorHandleForHeapStart();
	if (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
		_startHandle.gpu = _d3dHeap->GetGPUDescriptorHandleForHeapStart();
	}
}

} // namespace

#endif // #if AX_RENDERER_DX12

