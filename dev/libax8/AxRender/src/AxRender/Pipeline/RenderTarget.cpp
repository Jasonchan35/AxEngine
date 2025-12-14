module AxRender;
import :Renderer_Backend;
import :RenderRequest_Backend;
import :RenderContext_Backend;
import :RenderTarget_Backend;

namespace ax /*::AxRender*/ {

RenderTargetColorBuffer::RenderTargetColorBuffer(const CreateDesc& desc) {
	_name			= desc.name;
	_colorType		= desc.colorType;
	_size			= desc.size;
	_backBufferRef	= desc.backBufferRef;
}

void RenderTargetColorBuffer_CreateDesc::setBackBuffer(
	RenderContext* renderContext_, 
	Int index_, 
	ColorType colorType_,
	const Vec2i& size_
) {
	colorType	= colorType_;
	size		= size_;

	backBufferRef.renderContext	= renderContext_;
	backBufferRef.index			= index_;
}

RenderTargetDepthBuffer::RenderTargetDepthBuffer(const CreateDesc& desc) 
: Base(RenderDataType::DepthTexture)
{
	_type	   = RenderDataType::DepthTexture;
	_depthType = desc.depthType;
	_frameSize = desc.frameSize;
}

} // namespace