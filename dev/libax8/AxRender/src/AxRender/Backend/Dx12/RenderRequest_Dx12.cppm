module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
export import :Dx12Resource;
export import :Renderer_Backend;
export import :RenderRequest_Backend;
export import :CommandBuffer_Dx12;

namespace ax {

class RenderRequest_Dx12 : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Dx12, RenderRequest_Backend)
public:
	
	RenderRequest_Dx12(const CreateDesc& desc);
	
	CommandBuffer_Dx12	_uploadCmdBuf_dx12; // submit earlier than graphCmdBuf
	CommandBuffer_Dx12	_graphCmdBuf_dx12;
	CommandBuffer_Dx12	_computeCmdList_dx12;
	
	Dx12Fence			_completedFence_dx12;
	AX_RenderRequest_Backend_FunctionInterfaces(override)
};

} // namespace

#endif //AX_RENDERER_DX12