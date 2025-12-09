module AxRender;
import :Renderer_Backend;
import :Texture_Backend;

namespace ax::AxRender {

SPtr<Sampler> Sampler::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return Sampler_Backend::s_new(req, desc);
}

SPtr<Texture2D> Texture2D::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return Texture2D_Backend::s_new(req, desc);
}

SPtr<Texture2D> Texture2D::s_new(const MemAllocRequest& req, StrView assetPath) {
	return Texture2D_Backend::s_new(req, assetPath);
}

SPtr<Texture2D> Texture2D::s_new(const MemAllocRequest& req, const ImageInfo& info, ByteSpan pixelData) {
	return Texture2D_Backend::s_new(req, info, pixelData);
}

} // namespace