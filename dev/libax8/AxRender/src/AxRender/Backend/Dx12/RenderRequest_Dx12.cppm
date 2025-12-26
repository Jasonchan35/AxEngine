module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
export import :Dx12Resource;
export import :RenderSystem_Backend;
export import :RenderRequest_Backend;
export import :CommandBuffer_Dx12;
export import :RenderPass_Dx12;
export import :RenderContext_Dx12;

namespace ax {

class RenderRequest_Dx12 : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Dx12, RenderRequest_Backend)
public:
	
	RenderRequest_Dx12(const CreateDesc& desc);

	RenderSystem_Dx12*  renderSystem_dx12()			{ return rttiCastCheck<RenderSystem_Dx12>(_renderSystem); }
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

	Dx12DescriptorAllocator_ColorBuffer		_descAlloc_ColorBuffer;
	Dx12DescriptorAllocator_DepthBuffer		_descAlloc_DepthBuffer;
	Dx12DescriptorAllocator_CBV_SRV_UAV		_descAlloc_CBV_SRV_UAV;
	Dx12DescriptorAllocator_Sampler			_descAlloc_Sampler;

	void _updatedBindlessResources();

	void setDescriptorHeaps(Span<ID3D12DescriptorHeap*> heaps) {
		_graphCmdBuf_dx12->SetDescriptorHeaps(ax_safe_cast_from(heaps.size()), heaps.data());
	}
	
	Dx12_ID3D12Device* _d3dDevice = nullptr;
	
	AX_RenderRequest_Backend_FunctionInterfaces(override)
};

} // namespace

#endif //AX_RENDERER_DX12