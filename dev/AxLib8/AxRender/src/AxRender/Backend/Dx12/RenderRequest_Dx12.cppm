module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
export import :Dx12Resource;
export import :RenderSystem_Backend;
export import :RenderRequest_Backend;
export import :RenderCommandList_Dx12;
export import :RenderPass_Dx12;
export import :RenderContext_Dx12;
export import :RenderObjectManager_Dx12;

namespace ax {

class RenderRequest_Dx12 : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Dx12, RenderRequest_Backend)
public:
	RenderRequest_Dx12(const CreateDesc& desc);

	RenderSystem_Dx12*  renderSystem_dx12()			{ return rttiCastCheck<RenderSystem_Dx12>(_renderSystem); }
	RenderContext_Dx12*	renderContext_dx12()		{ return rttiCastCheck<RenderContext_Dx12>(_renderContext); }
	RenderPass_Dx12*	currentRenderPass_dx12()	{ return rttiCastCheck<RenderPass_Dx12   >(_currentRenderPass); }
	RenderCommandList_Dx12&	uploadCmdList_dx12()			{ return _uploadCmdList_dx12; }
	RenderCommandList_Dx12&	graphCmdList_dx12()			{ return _graphCmdList_dx12; }

	AX_INLINE u64 fenceValue_dx12() const { return static_cast<u64>(_renderSeqId); }
	void signalFence(Dx12CommandQueue& cmdQueue) { cmdQueue.signal(_fence, fenceValue_dx12()); }

	RenderCommandList_Dx12	_uploadCmdList_dx12; // submit earlier than graphCmdList
	RenderCommandList_Dx12	_graphCmdList_dx12;
	RenderCommandList_Dx12	_computeCmdList_dx12;

	Dx12Fence			_fence;
	Dx12CpuEvent		_cpuEvent;
	
	struct IndirectDraw {
		SPtr<GpuStructuredBuffer> drawArguments;
	} indirectDraw;

	struct DynamicDescriptors {
		Dx12DescriptorHeapChunk_ColorBuffer ColorBuffer;
		Dx12DescriptorHeapChunk_DepthBuffer DepthBuffer;
		Dx12DescriptorHeapChunk_CBV_SRV_UAV CBV_SRV_UAV;
		Dx12DescriptorHeapChunk_Sampler     Sampler;
	} _dynamicDescriptors;

	using DescriptorHeapPools  = RenderObjectManager_Dx12::DescriptorHeapPools;
	DescriptorHeapPools*		_descriptorHeapPools = nullptr;

	using ResourceDescriptors = RenderObjectManager_Dx12::ResourceDescriptors;
	ResourceDescriptors*		_resourceDescriptors = nullptr;

#if AX_RENDER_BINDLESS
	using BindlessDescriptors = RenderObjectManager_Dx12::BindlessDescriptors;
	BindlessDescriptors*		_bindlessDescriptors = nullptr;
#endif
	
	void setDescriptorHeaps(Span<ID3D12DescriptorHeap*> heaps) {
		_graphCmdList_dx12->SetDescriptorHeaps(ax_safe_cast_from(heaps.size()), heaps.data());
	}
	
	Dx12_ID3D12Device* _d3dDevice = nullptr;
	
	AX_RenderRequest_Backend_FunctionInterfaces(override)
};

} // namespace

#endif //AX_RENDERER_DX12