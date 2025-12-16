module;

export module AxRender:RenderRequest_Dx12;

#if AX_RENDERER_DX12
export import :Dx12Util;
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
	ComPtr<ID3D12Fence>	_computeFence_dx12;

	virtual void onFrameBegin() override;
	virtual void onFrameEnd() override;
};

} // namespace

#endif //AX_RENDERER_DX12