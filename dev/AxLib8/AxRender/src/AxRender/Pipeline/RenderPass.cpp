module;
module AxRender;
import :RenderSystem_Backend;
import :RenderPass_Backend;

namespace ax /*::AxRender*/ {

RenderPassColorBuffer::RenderPassColorBuffer(const CreateDesc& desc) {
	_name			 = desc.name;
	_colorType		 = desc.colorType;
	_backBufferRef	 = desc.fromBackBuffer;
}

RenderPassDepthBuffer::RenderPassDepthBuffer(const CreateDesc& desc) 
: Base(RenderDataType::DepthTexture)
{
	_textureType = RenderDataType::DepthTexture;
	_depthType   = desc.depthType;
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
	 && _depthAttachment.desc != desc.depthAttachmentDesc
	 && _colorAttachments.size() != desc.colorAttachmentDescs.size())
		return false;

	Int n = desc.colorAttachmentDescs.size();
	for (Int i = 0; i < n; ++i) {
		if (_colorAttachments[i].desc != desc.colorAttachmentDescs[i]) return false;
	}

	return true;
}

} // namespace