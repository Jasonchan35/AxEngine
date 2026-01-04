module;
module AxRender;
import :ImageIO;
import :RenderObjectManager_Backend;

namespace ax /*::AxRender*/ {

SPtr<Sampler> Sampler_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<Sampler_Backend> o;
	if (RenderObjectManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.samplerState))
		o->_create(desc);
	return o;
}

SPtr<Texture2D_Backend> Texture2D_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<Texture2D_Backend> o;
	if (RenderObjectManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.assetPath))
		o->_create(desc);
	return o;
}

SPtr<Texture2D_Backend> Texture2D_Backend::s_new(const MemAllocRequest& req, StrView assetPath) {
	Texture2D_CreateDesc desc;
	desc.assetPath = assetPath;
	auto tex = s_new(req, desc);
	return tex;
}

SPtr<Texture2D_Backend> Texture2D_Backend::s_new(const MemAllocRequest& req, const ImageInfo& info, ByteSpan pixelData) {
	Texture2D_CreateDesc desc;
	desc.imageInfo = info;
	desc.pixelData = pixelData;
	auto tex = s_new(req, desc);
	return tex;
}

void Texture2D_Backend::hotReloadFile() {
	RenderSystem_Backend::s_instance()->waitAllRenderCompleted();
	_loadFile();
}

void Texture2D_Backend::hotCreateFromImage(const ImageInfo& info, ByteSpan pixelData) {
	RenderSystem_Backend::s_instance()->waitAllRenderCompleted();
	_loadImage(info, pixelData);
}

void Texture2D_Backend::_loadFile() {
	if (_assetPath) {
		objectSlot.markDirty();
		ImageIO::loadFile([&](ImageIO_ReadHandler& handler) {
			_onImageIO_ReadHandler(handler);
		}, _assetPath);
	}
}

void Texture2D_Backend::_loadImage(const ImageInfo& info, ByteSpan pixelData) {
	ImageIO_ReadHandler handler;
	handler.desc.info = info;
	handler.desc.dataSize = pixelData.size();
	handler.readPixelsFunc = [&](MutByteSpan outSpan) {
		outSpan.copyValues(pixelData);
	};
	_onImageIO_ReadHandler(handler);
}

void Texture2D_Backend::onCreate(const CreateDesc& desc) {
	if (desc.pixelData.size()) {
		_loadImage(desc.imageInfo, desc.pixelData);
		return;
	}

	if (desc.assetPath) {
		_loadFile();
		return;
	}
}

} // namespace