module;
module AxRender.Texture_Backend;
import AxRender.ImageIO;
import AxRender.ResourceManager_Backend;

namespace ax::AxRender {

SPtr<Sampler> Sampler_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<Sampler_Backend> o;
	if (ResourceManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.samplerState))
		o->_create(desc);
	return o;	
}

Sampler_Backend::Sampler_Backend(const CreateDesc& desc)
: Base(desc)
, resourceHandle(this)
{}


SPtr<Texture2D_Backend> Texture2D_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<Texture2D_Backend> o;
	if (ResourceManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.assetPath))
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
	desc.info = info;
	desc.pixelData = pixelData;
	auto tex = s_new(req, desc);
	return tex;
}

Texture2D_Backend::Texture2D_Backend(const CreateDesc& desc)
: Base(desc)
, resourceHandle(this)
{}

void Texture2D_Backend::hotReloadFile() {
	Renderer_Backend::s_instance()->waitAllRenderCompleted();
	_loadFile();
}

void Texture2D_Backend::hotCreateFromImage(const ImageInfo& info, ByteSpan pixelData) {
	Renderer_Backend::s_instance()->waitAllRenderCompleted();
	_loadImage(info, pixelData);
}

void Texture2D_Backend::_loadFile() {
	if (_assetPath) {
		resourceHandle.markDirty();
		ImageIO::loadFile([&](ImageIO_ReadResult& result) {
			_onImageIO_ReadResult(result);
		}, _assetPath);
	}
}

void Texture2D_Backend::_loadImage(const ImageInfo& info, ByteSpan pixelData) {
	ImageIO_ReadResult result;
	result.desc.info = info;
	result.desc.dataSize = pixelData.size();
	result.copyPixelsFunc = [&](MutByteSpan outSpan) {
		outSpan.copyValues(pixelData);
	};
	_onImageIO_ReadResult(result);
}

void Texture2D_Backend::onCreate(const CreateDesc& desc) {
	if (desc.pixelData) {
		_loadImage(desc.info, desc.pixelData);
		return;
	}

	if (desc.assetPath) {
		_loadFile();
		return;
	}
}

Texture3D_Backend::Texture3D_Backend(const CreateDesc& desc)
: Base(desc)
, resourceHandle(this)
{}

} // namespace