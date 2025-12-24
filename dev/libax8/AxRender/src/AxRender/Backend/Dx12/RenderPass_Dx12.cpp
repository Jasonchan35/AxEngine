module AxRender;

#if AX_RENDERER_DX12
import :RenderPass_Dx12;
import :RenderContext_Dx12;
import :RenderRequest_Dx12;

namespace ax {

RenderPassColorBuffer_Dx12::RenderPassColorBuffer_Dx12(const CreateDesc& desc): Base(desc) {
	if (desc.fromBackBuffer) {
		auto* renderContext_dx12 = rttiCastCheck<RenderContext_Dx12>(desc.fromBackBuffer.renderContext);
		if (!renderContext_dx12) throw Error_Undefined();
		UINT backBufIndex = ax_safe_cast_from(desc.fromBackBuffer.index);
		_resource_dx12.createFromSwapChain(renderContext_dx12->_swapChain_dx12, backBufIndex);
	} else {
		_resource_dx12.create(desc.frameSize, desc.colorType);
	}
}

RenderPassDepthBuffer_Dx12::RenderPassDepthBuffer_Dx12(const CreateDesc& desc)
: Base(desc)
{
	_resource_dx12.create(desc.frameSize, desc.depthType);
}

RenderPass_Dx12::RenderPass_Dx12(const CreateDesc& desc)
: Base(desc)
{
	RenderContext_Dx12*					 renderContext_dx12 = nullptr;
	RenderContext_Dx12::BackBuffer_Dx12* backBuffer_dx12    = nullptr;
	if (desc.fromBackBuffer) {
		renderContext_dx12	= rttiCastCheck<RenderContext_Dx12>(desc.fromBackBuffer.renderContext);
		backBuffer_dx12		= renderContext_dx12->_getBackBuffer(desc.fromBackBuffer.index);
	}
	
	//---- color buffers ----
	const Int count = desc.colorAttachmentDescs.size();
	for (Int i = 0; i < count; i++) {
		auto& newColorAttachment = _colorAttachments.emplaceBack();
		newColorAttachment.desc = desc.colorAttachmentDescs[i];

		if (desc.fromBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			newColorAttachment.buffer = backBuffer_dx12->_colorBuf_dx12;
			
		} else {
			RenderPassColorBuffer_CreateDesc colorBuf_createDesc;
			colorBuf_createDesc.name      = FmtName("{}-color", desc.name);
			colorBuf_createDesc.colorType = newColorAttachment.desc.colorType;
			newColorAttachment.buffer     = RenderPassColorBuffer_Backend::s_new(AX_ALLOC_REQ, colorBuf_createDesc);
		}
	}

	//----- depth ------
	_depthAttachment.desc = desc.depthAttachmentDesc;
	if (_depthAttachment) {
		if (desc.fromBackBuffer) {
			_depthAttachment.buffer = renderContext_dx12->_depthBuffer_dx12;
			
		} else {
			RenderPassDepthBuffer_CreateDesc depthBufDesc;
			depthBufDesc.name = Fmt("{}-depth", desc.name);
			depthBufDesc.frameSize = desc.frameSize;
			depthBufDesc.depthType = desc.depthAttachmentDesc.depthType;

			_depthAttachment.buffer = RenderPassDepthBuffer_Backend::s_new(AX_ALLOC_REQ, depthBufDesc);
		}
	}	
	
}

void RenderPass_Dx12::colorBuf0_resourceBarrier(RenderRequest* req_, D3D12_RESOURCE_STATES state) {
	auto* colorBuf = colorBuffer_dx12(0);
	if (!colorBuf) return;

	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	colorBuf->_resource_dx12.resourceBarrier(req->_graphCmdBuf_dx12, state);
}

} // namespace

#endif // #if AX_RENDERER_DX12
