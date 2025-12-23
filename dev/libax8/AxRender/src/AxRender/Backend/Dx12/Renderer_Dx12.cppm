module;

export module AxRender:Renderer_Dx12;
#if AX_RENDERER_DX12

export import :Dx12Util;
export import :Renderer_Backend;

namespace ax {


class Renderer_Dx12 : public Renderer_Backend {
	AX_RTTI_INFO(Renderer_Dx12, Renderer_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE();
	Renderer_Dx12(const CreateDesc& desc);

	static ID3D12Device* s_d3dDevice() { auto* t = s_instance(); return t ? t->d3dDevice() : nullptr; }

	void createDevice();
	
	ID3D12Device*		d3dDevice	()	{ return _d3dDevice; }
	IDXGIFactory4*		dxgiFactory	()	{ return _dxgiFactory; }

#if _DEBUG
	IDXGIDebug1*		dxgiDebug	()	{ return _dxgiDebug; }
#endif

protected:
	AX_Renderer_FunctionInterfaces(Dx12, override)
	
	virtual void onGetMemoryInfo(MemoryInfo& info) override;

private:
	void _getHardwareAdapter();

	D3D_FEATURE_LEVEL				_d3dFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
	ComPtr<AX_DX12_IDXGIFactory>	_dxgiFactory;
	ComPtr<AX_DX12_ID3D12Device>	_d3dDevice;
	ComPtr<AX_DX12_IDXGIAdapter>	_dxgiAdapter;

#if _DEBUG
	ComPtr<AX_DX12_IDXGIDebug>		_dxgiDebug;
	ComPtr<AX_DX12_ID3D12Debug>		_d3dDebug;
#endif
};

} // namespace ax {

#endif // AX_RENDERER_DX12