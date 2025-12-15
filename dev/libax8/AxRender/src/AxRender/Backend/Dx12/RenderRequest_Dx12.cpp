module AxRender;
import :RenderRequest_Dx12;
import :Renderer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderRequest_Dx12::RenderRequest_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	auto* dev = Renderer_Dx12::s_d3dDevice();

	_uploadCmdBuf_dx12.create(dev, CommandBufferType::Copy);

	_graphCmdBuf_dx12.create(dev, CommandBufferType::Direct);
	Base::_graphCmdBuf = &_graphCmdBuf_dx12; 
	
	_computeCmdList_dx12.create(dev, CommandBufferType::Compute);
	
	{
		auto hr = Renderer_Dx12::s_d3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_computeFence_dx12.ptrForInit()));
		Dx12Util::throwIfError(hr);
	}
}

} // namespace

#endif // #if AX_RENDERER_DX12