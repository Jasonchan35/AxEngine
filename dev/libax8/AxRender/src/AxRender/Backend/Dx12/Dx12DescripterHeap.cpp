module AxRender;
import :Dx12DescripterHeap;

#if AX_RENDERER_DX12

namespace  ax {

void Dx12DescripterHeap_Base::destroy() {
	_d3dHeap.unref();
	_numDescriptors = 0;
	_heapStartHandle = Dx12DescriptorHandle();
}

void Dx12DescripterHeap_Base::_init(Int numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	if (numDescriptors <= 0) {
		destroy();
		return;
	}

	if (_d3dHeap && _desc.Type == type && _desc.Flags == flags && _numDescriptors == numDescriptors)
		return;

	auto* renderer = Renderer_Dx12::s_instance();
	auto* d3dDevice = renderer->d3dDevice();
	_stride = d3dDevice->GetDescriptorHandleIncrementSize(type);

	_desc.NumDescriptors = SafeCast(numDescriptors);
	_desc.Type  = type;
	_desc.Flags = flags;
	auto hr = d3dDevice->CreateDescriptorHeap(&_desc, IID_PPV_ARGS(_d3dHeap.ptrForInit()));
	Dx12Util::throwIfError(hr);

	_heapStartHandle.cpu = _d3dHeap->GetCPUDescriptorHandleForHeapStart();
	_heapStartHandle.gpu = _d3dHeap->GetGPUDescriptorHandleForHeapStart();
	_numDescriptors = numDescriptors;
}

} // namespace

#endif // #if AX_RENDERER_DX12

