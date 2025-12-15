module AxRender;

#if AX_RENDERER_DX12
import :RenderPass_Dx12;
import :RenderContext_Dx12;

namespace ax {

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
	const Int colorBufferCount = desc.colorBuffers.size();
	for (Int i = 0; i < colorBufferCount; i++) {
		auto& colorBuf = _colorBuffers.emplaceBack();
		auto& srcColorDesc = desc.colorBuffers[i];
		_colorBuffers[i].desc = srcColorDesc;

		if (desc.isBackBuffer) {
			if (i >= 1) throw Error_Undefined();
			colorBuf.colorBuf = backBuffer_dx12->_colorBuf_dx12;
		}
	}

}


} // namespace

#endif // #if AX_RENDERER_DX12
