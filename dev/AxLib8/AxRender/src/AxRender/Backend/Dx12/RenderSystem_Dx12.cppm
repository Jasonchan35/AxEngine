module;

export module AxRender:RenderSystem_Dx12;
#if AX_RENDERER_DX12

export import :Dx12Util;
export import :RenderSystem_Backend;

namespace ax {


class RenderSystem_Dx12 : public RenderSystem_Backend {
	AX_RTTI_INFO(RenderSystem_Dx12, RenderSystem_Backend)
public:
	AX_DOWNCAST_GET_INSTANCE();
	RenderSystem_Dx12(const CreateDesc& desc);
	virtual ~RenderSystem_Dx12() override;

	static Dx12_ID3D12Device* s_d3dDevice() { auto* t = s_instance(); return t ? t->d3dDevice() : nullptr; }

	Dx12_ID3D12Device*		d3dDevice	()	{ return _d3dDevice; }
	Dx12_IDXGIFactory*		dxgiFactory	()	{ return _dxgiFactory; }
	D3D12MA::Allocator*		d3dAllocator() { return _d3dAllocator; }

#if _DEBUG
	Dx12_IDXGIDebug*		dxgiDebug	()	{ return _dxgiDebug; }
#endif

protected:
	AX_RenderSystem_FunctionInterfaces(Dx12, override)
	
	virtual void onGetMemoryInfo(MemoryInfo& info) override;

private:
	void _createDevice();
	void _createAllocator();
	
	void _getHardwareAdapter();

	D3D_FEATURE_LEVEL			_d3dFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
	ComPtr<Dx12_IDXGIFactory>	_dxgiFactory;
	ComPtr<Dx12_ID3D12Device>	_d3dDevice;
	ComPtr<Dx12_IDXGIAdapter>	_dxgiAdapter;

#if _DEBUG
	ComPtr<Dx12_IDXGIDebug>		_dxgiDebug;
	ComPtr<Dx12_ID3D12Debug>	_d3dDebug;
#endif
	
	ComPtr<D3D12MA::Allocator>	_d3dAllocator;
};

} // namespace ax {

#endif // AX_RENDERER_DX12