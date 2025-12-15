module;

export module AxRender:RenderPass_Dx12;
export import :Dx12Util;
export import :RenderPass_Backend;

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {

class RenderPass_Dx12 : public RenderPass_Backend {
	AX_RTTI_INFO(RenderPass_Dx12, RenderPass_Backend)
public:	
	RenderPass_Dx12(const CreateDesc& desc);

	void releaseResources() {}


	Array<D3D12_CPU_DESCRIPTOR_HANDLE>	_renderTargetDescriptors;
		  D3D12_CPU_DESCRIPTOR_HANDLE	_depthStencilDescriptor;
};


} // namespace

#endif // #if AX_RENDERER_DX12