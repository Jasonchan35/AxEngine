module AxRender;
import :RenderRequest_Dx12;
import :Renderer_Dx12;
import :RenderPass_Dx12;
import :Material_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

RenderRequest_Dx12::RenderRequest_Dx12(const CreateDesc& desc)
	: Base(desc)
{
	auto* dev = Renderer_Dx12::s_d3dDevice();

	_uploadCmdBuf_dx12.create(dev, CommandBufferType::Direct); // CommandBufferType::Copy
	_graphCmdBuf_dx12.create(dev, CommandBufferType::Direct);
	_computeCmdList_dx12.create(dev, CommandBufferType::Compute);
	_fence.create(dev, 1);
	_cpuEvent.create();
	_cpuEvent.signalOnFenceCompletion(_fence, 1);
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
	if (!_cpuEvent.wait(AxRenderConfig::kMaxRenderWaitTime())) {
		throw Error_Undefined("Render - timeout");
	}
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

void RenderRequest_Dx12::onDrawCall(Cmd_DrawCall& drawcall) {
	auto topology  = Dx12Util::getDxPrimitiveTopology(drawcall.primitiveType);
	auto& cmdList = _graphCmdBuf_dx12;
	
	cmdList->IASetPrimitiveTopology(topology);

	{ // bind vertex buffer
		auto vertexLayout = drawcall.vertexLayout;

		auto* vb = rttiCastCheck<GpuBuffer_Dx12>(drawcall.vertexBuffer);
		if (!vb) throw Error_Undefined();

		vb->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		D3D12_VERTEX_BUFFER_VIEW vbView = {};
		vbView.BufferLocation = ax_safe_cast_from(vb->gpuAddress());
		vbView.SizeInBytes    = ax_safe_cast_from(vb->bufferSize());
		vbView.StrideInBytes  = ax_safe_cast_from(vertexLayout->strideInBytes);
		cmdList->IASetVertexBuffers(0, 1, &vbView);
	}
	
	if (drawcall.indexType == IndexType::None) {
		cmdList->DrawInstanced(Dx12Util::castUINT(drawcall.vertexCount),
		                       ax_safe_cast_from(drawcall.instanceCount),
		                       ax_safe_cast_from(drawcall.vertexStart),
		                       ax_safe_cast_from(drawcall.instanceStart));
		
	} else {
		auto indexType = Dx12Util::getDxIndexType(drawcall.indexType);
		if (!drawcall.indexBuffer) throw Error_Undefined();
		auto* ib = rttiCastCheck<GpuBuffer_Dx12>(drawcall.indexBuffer);
		if (!ib) throw Error_Undefined();

		//ib->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_INDEX_BUFFER);

		D3D12_INDEX_BUFFER_VIEW ibView = {};
		ibView.BufferLocation = Dx12Util::castUINT64(ib->gpuAddress());
		ibView.SizeInBytes    = Dx12Util::castUINT(ib->bufferSize());
		ibView.Format = indexType;

		cmdList->IASetIndexBuffer(&ibView);
		cmdList->DrawIndexedInstanced(Dx12Util::castUINT(drawcall.indexCount),
		                              ax_safe_cast_from(drawcall.instanceCount),
		                              ax_safe_cast_from(drawcall.indexStart),
		                              ax_safe_cast_from(drawcall.vertexStart),
		                              ax_safe_cast_from(drawcall.instanceStart));
	}
}

void RenderRequest_Dx12::onRenderPassBegin(RenderPass* pass_) {
	auto* pass = rttiCastCheck<RenderPass_Dx12>(pass_);
	pass->colorBuf0_resourceBarrier(this, D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto& renderTargetDescriptors = pass->_colorViewHandles_dx12;
	auto& cmdBuf = _graphCmdBuf_dx12;

	UINT                         rtCount        = ax_safe_cast_from(renderTargetDescriptors.size());
	D3D12_CPU_DESCRIPTOR_HANDLE* rtViews        = renderTargetDescriptors.data();
	D3D12_CPU_DESCRIPTOR_HANDLE* depthView      = &pass->_depthViewHandle_dx12;
	BOOL                         rtSingleHandle = FALSE;

	AX_ASSERT(rtCount > 0);
	AX_ASSERT(rtViews);
	AX_ASSERT(depthView);
	
	cmdBuf->OMSetRenderTargets(rtCount, rtViews, rtSingleHandle, depthView);

	for (auto& colorAttachment : pass->colorAttachments()) {
		auto& desc = colorAttachment.desc;
		auto* colorBuffer = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorAttachment.buffer.ptr());
		if (!colorBuffer) continue;
		if (desc.loadOp == RenderBufferLoadOp::Clear) {
			cmdBuf->ClearRenderTargetView(	colorBuffer->_view_dx12.handle.cpu,
											desc.clearColor.data(),
											0,
											nullptr);
		}
	}

	if (auto* depthBuffer = pass->depthBuffer_dx12()) {
		auto& desc = pass->depthAttachment().desc;
		if (desc.loadOp == RenderBufferLoadOp::Clear) {
			cmdBuf->ClearDepthStencilView(depthBuffer->_view_dx12.handle.cpu,
			                              D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			                              desc.clearDepth,
			                              ax_safe_cast_to<u8>(desc.clearStencil),
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