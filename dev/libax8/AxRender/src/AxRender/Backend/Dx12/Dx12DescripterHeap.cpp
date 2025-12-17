module AxRender;
import :Dx12DescripterHeap;

#if AX_RENDERER_DX12

namespace  ax {

void Dx12DescripterHeap_Base::destroy() {
	_d3dHeap.unref();
	_desc.NumDescriptors = 0;
	_startHandle = Dx12DescriptorHandle();
}

void Dx12DescripterHeap_Base::_create(Int size, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	UINT numDescriptors = ax_safe_cast_from(size);
	if (numDescriptors <= 0) {
		destroy();
		return;
	}

	if (_d3dHeap && _desc.Type == type && _desc.Flags == flags && _desc.NumDescriptors == numDescriptors)
		return;

	auto* renderer = Renderer_Dx12::s_instance();
	auto* d3dDevice = renderer->d3dDevice();
	_stride = d3dDevice->GetDescriptorHandleIncrementSize(type);

	_desc.NumDescriptors = ax_safe_cast_from(numDescriptors);
	_desc.Type  = type;
	_desc.Flags = flags;
	auto hr = d3dDevice->CreateDescriptorHeap(&_desc, IID_PPV_ARGS(_d3dHeap.ptrForInit()));
	Dx12Util::throwIfError(hr);

	_startHandle.cpu = _d3dHeap->GetCPUDescriptorHandleForHeapStart();
	_startHandle.gpu = _d3dHeap->GetGPUDescriptorHandleForHeapStart();
}

} // namespace

#endif // #if AX_RENDERER_DX12

