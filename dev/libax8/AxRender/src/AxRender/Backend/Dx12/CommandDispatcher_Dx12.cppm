module;
export module AxRender:CommandDispatcher_Dx12;

#if 0

#if AX_RENDERER_DX12

import :Renderer_Backend;

namespace ax {

class CommandDispatcher_Dx12 : public axRenderCommandDispatcherImpl<CommandDispatcher_Dx12> {
	using Base = axRenderCommandDispatcherImpl<CommandDispatcher_Dx12>;
public:
	

	CommandDispatcher_Dx12(axDX12RenderContext* renderContext_, axDX12RenderRequest* renderRequest);

	virtual void onUploadTexture2D(Texture2D_UploadRequest& req) final;
	virtual void onGpuFrameBegin() final;
	virtual void onGpuFrameEnd() final;

	void _execCmdList();

	axRenderGpuRequestImpl_COMMAND_FUNCTIONS;

	axDX12RenderContext*				renderContext = nullptr;
	axDX12_ID3D12Device*				d3dDevice = nullptr;

	ComPtr<ID3D12CommandQueue>		cmdQueue;
	ComPtr<ID3D12CommandQueue>		computeCmdQueue;

	ID3D12GraphicsCommandList*			cmdList = nullptr;
	ID3D12GraphicsCommandList*			computeCmdList = nullptr;

	axDX12DescriptorHandle				renderTarget;
	axDX12DescriptorHandle				depthStencilBuffer;

	axDX12RenderRequest*				renderRequest = nullptr;

	bool _hasComputeCall = false;

#if AX_RENDERER_DX12_DXR
	void buildRayTracingAccelStructs();
	void cmdDrawCallRayTracing	(DrawCall& drawCall);

	axDX12RayTracing*					rayTracing = nullptr;
	axArray<D3D12_RAYTRACING_GEOMETRY_DESC>	rayTracingGeoDescs;
#endif
};

#endif // #if AX_RENDERER_DX12

#endif // #if 0