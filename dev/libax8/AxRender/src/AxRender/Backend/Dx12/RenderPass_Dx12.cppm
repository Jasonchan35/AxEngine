module;

export module AxRender:RenderPass_Dx12;
export import :Dx12DescripterHeap;
export import :RenderPass_Backend;

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {


class RenderPassColorBuffer_Dx12 : public RenderPassColorBuffer_Backend {
	AX_RTTI_INFO(RenderPassColorBuffer_Dx12, RenderPassColorBuffer_Backend)
public:
	RenderPassColorBuffer_Dx12(const CreateDesc& desc);

	void createFromSwapChain(AX_DX12_IDXGISwapChain* swapChain, UINT backBufIndex);
	void releaseResources() {
		_d3dResource.destroy();
		_descHeap.destroy();
		_renderTargetView = {};
	}

	Dx12Resource_ColorBuffer		_d3dResource;
	Dx12DescripterHeapArray			_descHeap;
	Dx12DescriptorHandle			_renderTargetView;
};

class RenderPassDepthBuffer_Dx12 : public RenderPassDepthBuffer_Backend {
	AX_RTTI_INFO(RenderPassDepthBuffer_Dx12, RenderPassDepthBuffer_Backend)
public:
	RenderPassDepthBuffer_Dx12(const CreateDesc& desc) : Base(desc) {}
};

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