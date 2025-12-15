module;
module AxRender;
import :Renderer_Backend;
import :RenderPass_Backend;

namespace ax /*::AxRender*/ {

RenderPassColorBuffer::RenderPassColorBuffer(const CreateDesc& desc) {
	_name			 = desc.name;
	_attachment = desc.attachment;
	_backBufferRef	 = desc.backBufferRef;
}

RenderPassDepthBuffer::RenderPassDepthBuffer(const CreateDesc& desc) 
: Base(RenderDataType::DepthTexture)
{
	_type            = RenderDataType::DepthTexture;
	_attachment = desc.attachment;
}

RenderPass::RenderPass(const CreateDesc& desc) {
	_name = desc.name;
	_frameSize = desc.frameSize;

	if (_frameSize.x <= 0 || _frameSize.y <= 0) {
		AX_ASSERT(false);
	}
}

bool RenderPass::isCompatible(const CreateDesc& desc) const {
	if (_frameSize != desc.frameSize 
	 && _depthBuffer.attachment != desc.depthBufferAttachment
	 && _colorBuffers.size() != desc.colorBufferAttachments.size())
		return false;

	Int n = desc.colorBufferAttachments.size();
	for (Int i = 0; i < n; ++i) {
		if (_colorBuffers[i].attachment != desc.colorBufferAttachments[i]) return false;
	}

	return true;
}

} // namespace