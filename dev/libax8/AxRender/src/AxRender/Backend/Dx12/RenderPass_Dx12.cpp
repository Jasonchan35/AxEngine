module AxRender;

#if AX_RENDERER_DX12
import :RenderPass_Dx12;
import :RenderContext_Dx12;
import :RenderRequest_Dx12;

namespace ax {

RenderPassColorBuffer_Dx12::RenderPassColorBuffer_Dx12(const CreateDesc& desc): Base(desc) {
	if (desc.backBufferRef) {
		auto* renderContext_dx12 = rttiCastCheck<RenderContext_Dx12>(desc.backBufferRef.renderContext);
		if (!renderContext_dx12) throw Error_Undefined();
		UINT backBufIndex = ax_safe_cast(desc.backBufferRef.index);
		_resource_dx12.createFromSwapChain(renderContext_dx12->_swapChain_dx12, backBufIndex);
	} else {
		_resource_dx12.create(desc.frameSize, desc.attachment.colorType);
	}
	_descHeap_dx12.create(1);
	_view_dx12 = _descHeap_dx12.createView(0, _resource_dx12);
}

RenderPassDepthBuffer_Dx12::RenderPassDepthBuffer_Dx12(const CreateDesc& desc)
: Base(desc)
{
	_resource_dx12.create(desc.frameSize);
	_descHeap_dx12.create(1);
	_view_dx12 = _descHeap_dx12.createView(0, _resource_dx12);
}

RenderPass_Dx12::RenderPass_Dx12(const CreateDesc& desc)
: Base(desc)
{
	RenderContext_Dx12*					 renderContext_dx12 = nullptr;
	RenderContext_Dx12::BackBuffer_Dx12* backBuffer_dx12    = nullptr;
	if (desc.isBackBuffer) {
		renderContext_dx12	= rttiCastCheck<RenderContext_Dx12>(desc.renderContext);
		backBuffer_dx12		= renderContext_dx12->_getBackBuffer(desc.backBufferIndex);
	}
	
	//---- color buffers ----
	const Int colorBufferCount = desc.colorBufferAttachments.size();
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& newColorBuffer      = _colorBuffers.emplaceBack();
		newColorBuffer.attachment = desc.colorBufferAttachments[i];

		if (desc.isBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			newColorBuffer.buffer = backBuffer_dx12->_colorBuf_dx12;
			
		} else {
			RenderPassColorBuffer_CreateDesc colorBuf_createDesc;
			colorBuf_createDesc.name = Fmt("{}-color", desc.name);
			colorBuf_createDesc.attachment = newColorBuffer.attachment;
			newColorBuffer.buffer = RenderPassColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);
		
		}

		auto* colorBuffer_dx12 = rttiCastCheck<RenderPassColorBuffer_Dx12>(newColorBuffer.buffer.ptr());
		_colorViewHandles_dx12.emplaceBack(colorBuffer_dx12->_view_dx12.handle.cpu);
	}


	//----- depth ------
	_depthBuffer.attachment = desc.depthBufferAttachment;

	bool hasDepth = _depthBuffer.attachment.isEnabled();
	if (hasDepth) {
		if (desc.isBackBuffer) {
			_depthBuffer.buffer = renderContext_dx12->_depthBuffer_dx12;
			
		} else {
			RenderPassDepthBuffer_CreateDesc depthBufDesc;
			depthBufDesc.name = Fmt("{}-depth", desc.name);
			depthBufDesc.frameSize = desc.frameSize;
			depthBufDesc.attachment = desc.depthBufferAttachment;

			_depthBuffer.buffer = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
		}

		auto* depthBuffer_vk = rttiCastCheck<RenderPassDepthBuffer_Dx12>(_depthBuffer.buffer.ptr());
		_depthViewHandle_dx12 = depthBuffer_vk->_view_dx12.handle.cpu;
	}	
	
}

void RenderPass_Dx12::colorBuf0_resourceBarrier(RenderRequest* req_, D3D12_RESOURCE_STATES state) {
	auto* colorBuf = _colorBuffers.tryGetElement(0);
	if (!colorBuf) return;

	auto* buffer = rttiCastCheck<RenderPassColorBuffer_Dx12>(colorBuf->buffer.ptr());

	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	buffer->_resource_dx12.resourceBarrier(req->_graphCmdBuf_dx12, state);
}

} // namespace

#endif // #if AX_RENDERER_DX12
