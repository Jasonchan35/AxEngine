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

	AX_INLINE u64 fenceValue_dx12() const { return static_cast<u64>(_renderSeqId); }
	void signalFence(Dx12CommandQueue& cmdQueue) { cmdQueue.signal(_fence, fenceValue_dx12()); }
	
	CommandBuffer_Dx12	_uploadCmdBuf_dx12; // submit earlier than graphCmdBuf
	CommandBuffer_Dx12	_graphCmdBuf_dx12;
	CommandBuffer_Dx12	_computeCmdList_dx12;

	Dx12Fence			_fence;
	Dx12CpuEvent		_cpuEvent;
	
	AX_RenderRequest_Backend_FunctionInterfaces(override)
};

} // namespace

#endif //AX_RENDERER_DX12