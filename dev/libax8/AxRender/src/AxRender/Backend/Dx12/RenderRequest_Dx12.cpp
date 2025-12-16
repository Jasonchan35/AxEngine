module AxRender;
import :RenderRequest_Dx12;
import :Renderer_Dx12;
import :RenderPass_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderRequest_Dx12::RenderRequest_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	auto* dev = Renderer_Dx12::s_d3dDevice();

	_uploadCmdBuf_dx12.create(dev, CommandBufferType::Copy);
	_graphCmdBuf_dx12.create(dev, CommandBufferType::Direct);
	_computeCmdList_dx12.create(dev, CommandBufferType::Compute);

	_completedFence_dx12.create(dev);
}

void RenderRequest_Dx12::onFrameBegin() {
	_uploadCmdBuf_dx12.commandBegin();
	_graphCmdBuf_dx12.commandBegin();
}

void RenderRequest_Dx12::onFrameEnd() {
	_uploadCmdBuf_dx12.commandEnd();
	_graphCmdBuf_dx12.commandEnd();
}

void RenderRequest_Dx12::onWaitCompleted() {
	// AX_ASSERT_TODO	
	// _completedFence_dx12.wait();
	// _uploadCmdBuf_dx12.resetAndReleaseResource();
	// _graphCmdBuf_dx12.resetAndReleaseResource();
}

void RenderRequest_Dx12::onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) {
	D3D12_VIEWPORT tmp;
	tmp.TopLeftX = rect.x;
	tmp.TopLeftY = rect.y;
	tmp.Width    = rect.w;
	tmp.Height   = rect.h;
	tmp.MinDepth = minDepth;
	tmp.MaxDepth = maxDepth;
	_graphCmdBuf_dx12->RSSetViewports(1, &tmp);
}

void RenderRequest_Dx12::onSetScissorRect(const Rect2f& rect) {
	D3D12_RECT tmp;
	tmp.left   = static_cast<LONG>(rect.xMin());
	tmp.top    = static_cast<LONG>(rect.yMin());
	tmp.right  = static_cast<LONG>(rect.xMax());
	tmp.bottom = static_cast<LONG>(rect.yMax());
	_graphCmdBuf_dx12->RSSetScissorRects(1, &tmp);
}

void RenderRequest_Dx12::onDrawCall(Cmd_DrawCall& cmd) {
	// TODO
}

void RenderRequest_Dx12::onRenderPassBegin(RenderPass* pass_) {
	auto* pass = rttiCastCheck<RenderPass_Dx12>(pass_);
	pass->colorBuf0_resourceBarrier(this, D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto& renderTargetDescriptors = pass->_colorViewHandles_dx12;

	UINT                         rtCount        = SafeCast(renderTargetDescriptors.size());
	D3D12_CPU_DESCRIPTOR_HANDLE* rtViews        = renderTargetDescriptors.data();
	D3D12_CPU_DESCRIPTOR_HANDLE* depthView      = &pass->_depthViewHandle_dx12;
	BOOL                         rtSingleHandle = FALSE;

	AX_ASSERT(rtCount > 0);
	AX_ASSERT(rtViews);
	AX_ASSERT(depthView);
	
	_graphCmdBuf_dx12->OMSetRenderTargets(rtCount, rtViews, rtSingleHandle, depthView);

	for (auto& colorBuf : pass->colorBuffers()) {
		if (!colorBuf.buffer.ptr()) continue;
		auto* buffer = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorBuf.buffer.ptr());

		if (colorBuf.attachment.loadOp == RenderBufferLoadOp::Clear) {
			_graphCmdBuf_dx12->ClearRenderTargetView(buffer->_view_dx12.handle.cpu, colorBuf.attachment.clearColor.data(), 0, nullptr);
		}
	}
	if (auto* depthBuffer = pass->depthBuffer_dx12()) {
		if (depthBuffer->attachment().loadOp == RenderBufferLoadOp::Clear) {
			float depthValue   = depthBuffer->attachment().clearDepth;
			u8    stencilValue = static_cast<u8>(depthBuffer->attachment().clearStencil);
			_graphCmdBuf_dx12->ClearDepthStencilView(depthBuffer->_view_dx12.handle.cpu,
			                                         D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			                                         depthValue,
			                                         stencilValue,
			                                         0,
			                                         nullptr);
		}
	}
}

void RenderRequest_Dx12::onRenderPassEnd(RenderPass* pass_) {
	auto* pass = rttiCastCheck<RenderPass_Dx12>(pass_);
	pass->colorBuf0_resourceBarrier(this, D3D12_RESOURCE_STATE_PRESENT);
}



} // namespace

#endif // #if AX_RENDERER_DX12