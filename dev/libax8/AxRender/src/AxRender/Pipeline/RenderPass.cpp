module;
module AxRender;
import :Renderer_Backend;
import :RenderPass_Backend;

namespace ax::AxRender {

RenderPass::RenderPass(const CreateDesc& desc) {
	_name = desc.name;
	_frameSize = desc.frameSize;

	if (_frameSize.x <= 0 || _frameSize.y <= 0) {
		AX_ASSERT(false);
	}
}

bool RenderPass::isCompatible(const CreateDesc& desc) const {
	if (_frameSize != desc.frameSize 
	 && _depthBuffer.desc != desc.depthBuffer
	 && _colorBuffers.size() != desc.colorBuffers.size())
		return false;

	Int n = desc.colorBuffers.size();
	for (Int i = 0; i < n; i++) {
		if (_colorBuffers[i].desc != desc.colorBuffers[i]) return false;
	}

	return true;
}

} // namespace