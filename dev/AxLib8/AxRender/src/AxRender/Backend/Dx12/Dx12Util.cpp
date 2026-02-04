module AxRender;
import :Dx12Util;
import :RenderSystem_Dx12;

#if AX_RENDER_DX12

namespace ax {

void Dx12Util::reportError(HRESULT hr) {
	if (hr == DXGI_ERROR_DEVICE_REMOVED) {
		hr = RenderSystem_Dx12::s_d3dDevice()->GetDeviceRemovedReason();
	}
	
	_com_error err(hr);
	auto errMsg = StrView_c_str(err.ErrorMessage());
	AX_LOG_ERROR("HRESULT = {:08x} {}", static_cast<u32>(hr), errMsg); 
	
#if 0 && _DEBUG
	auto* d = renderSystem()->dxgiDebug();
	if (d) {
		d->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
#endif
	AX_ASSERT(false);
}

} // namespace

#endif // #if AX_RENDER_DX12