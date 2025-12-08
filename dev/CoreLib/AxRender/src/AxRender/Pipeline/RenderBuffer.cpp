module AxRender;
import :RenderBuffer_Backend;

namespace ax::AxRender {

RenderColorBuffer::RenderColorBuffer(const CreateDesc& desc) {
	_name			= desc.name;
	_colorType		= desc.colorType;
	_size			= desc.size;
	_backBufferRef	= desc.backBufferRef;
}

void RenderColorBuffer_CreateDesc::setBackBuffer(
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

RenderDepthBuffer::RenderDepthBuffer(const CreateDesc& desc) 
: Base(DataType::DepthTexture)
{
	_type	   = DataType::DepthTexture;
	_depthType = desc.depthType;
	_frameSize = desc.frameSize;
}

} // namespace