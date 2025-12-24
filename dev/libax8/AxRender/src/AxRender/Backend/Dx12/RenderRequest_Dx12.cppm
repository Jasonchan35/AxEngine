module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
export import :Dx12Resource;
export import :Renderer_Backend;
export import :RenderRequest_Backend;
export import :CommandBuffer_Dx12;
export import :RenderPass_Dx12;
export import :RenderContext_Dx12;

namespace ax {

class RenderRequest_Dx12 : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Dx12, RenderRequest_Backend)
public:
	
	RenderRequest_Dx12(const CreateDesc& desc);
	
	RenderContext_Dx12*	renderContext_dx12()		{ return rttiCastCheck<RenderContext_Dx12>(_renderContext); }
	RenderPass_Dx12*	currentRenderPass_dx12()	{ return rttiCastCheck<RenderPass_Dx12   >(_currentRenderPass); }
	CommandBuffer_Dx12&	uploadCmdBuf_dx12()			{ return _uploadCmdBuf_dx12; }
	CommandBuffer_Dx12&	graphCmdBuf_dx12()			{ return _graphCmdBuf_dx12; }

#if AX_RENDER_BINDLESS
	virtual void onBindlessResourceUpdates(Span<SPtr<Sampler_Backend  >>) {};
	virtual void onBindlessResourceUpdates(Span<SPtr<Texture2D_Backend>>) {};
#endif
	
	AX_INLINE u64 fenceValue_dx12() const { return static_cast<u64>(_renderSeqId); }
	void signalFence(Dx12CommandQueue& cmdQueue) { cmdQueue.signal(_fence, fenceValue_dx12()); }
	
	CommandBuffer_Dx12	_uploadCmdBuf_dx12; // submit earlier than graphCmdBuf
	CommandBuffer_Dx12	_graphCmdBuf_dx12;
	CommandBuffer_Dx12	_computeCmdList_dx12;

	Dx12Fence			_fence;
	Dx12CpuEvent		_cpuEvent;

	Dx12DescriptorHandle_ConstBuffer addDescCBV(const Dx12Resource_GpuBuffer& res) {
		return _heap_CBV_SRV_UAV.addCBV(_d3dDevice, res);
	}

	Dx12DescriptorHandle_UAV addDescUAV(const Dx12Resource_GpuBuffer& buf) {
		return _heap_CBV_SRV_UAV.addUAV(_d3dDevice, buf);
	}

	Dx12DescriptorHandle_Texture2D addDescTexture(const Dx12Resource_Texture2D& res) {
		return _heap_CBV_SRV_UAV.addTexture(_d3dDevice, res);
	}	

	Dx12DescriptorHandle_Sampler addDescSampler(SamplerFilter filter, SamplerWrapUVW wrap) {
		return _heap_sampler.addSampler(_d3dDevice, filter, wrap);
	}	

	Dx12DescripterHeap_ColorBuffer		_heap_ColorBuffer;
	Dx12DescripterHeap_DepthBuffer		_heap_DepthBuffer;
	Dx12DescripterHeap_CBV_SRV_UAV		_heap_CBV_SRV_UAV;
	Dx12DescripterHeap_Sampler			_heap_sampler;
	
	Array<ID3D12DescriptorHeap*, 2>		_d3dDescHeaps;

	Dx12_ID3D12Device* _d3dDevice = nullptr;
	
	AX_RenderRequest_Backend_FunctionInterfaces(override)
};

} // namespace

#endif //AX_RENDERER_DX12