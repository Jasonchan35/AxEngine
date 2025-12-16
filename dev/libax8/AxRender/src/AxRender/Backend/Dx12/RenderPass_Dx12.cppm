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

	void releaseResources() {
		_resource_dx12.destroy();
		_descHeap_dx12.destroy();
		_view_dx12 = {};
	}

	Dx12Resource_ColorBuffer			_resource_dx12;
	Dx12DescripterHeap_ColorBuffer		_descHeap_dx12;
	Dx12DescriptorHandle_ColorBuffer	_view_dx12;
};

class RenderPassDepthBuffer_Dx12 : public RenderPassDepthBuffer_Backend {
	AX_RTTI_INFO(RenderPassDepthBuffer_Dx12, RenderPassDepthBuffer_Backend)
public:
	RenderPassDepthBuffer_Dx12(const CreateDesc& desc);

	Dx12Resource_DepthBuffer			_resource_dx12;
	Dx12DescripterHeap_DepthBuffer		_descHeap_dx12;
	Dx12DescriptorHandle_DepthBuffer	_view_dx12;
};

class RenderPass_Dx12 : public RenderPass_Backend {
	AX_RTTI_INFO(RenderPass_Dx12, RenderPass_Backend)
public:	
	RenderPass_Dx12(const CreateDesc& desc);

	void colorBuf0_resourceBarrier(RenderRequest* req_, D3D12_RESOURCE_STATES state);
	
	void releaseResources() {}

	Array<D3D12_CPU_DESCRIPTOR_HANDLE>	_colorViewHandles_dx12;
		  D3D12_CPU_DESCRIPTOR_HANDLE	_depthViewHandle_dx12;
};


} // namespace

#endif // #if AX_RENDERER_DX12