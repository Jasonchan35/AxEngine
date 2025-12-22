module AxRender;
import :ResourceManager_Backend;
import :RenderRequest_Backend;

namespace ax /*::AxRender*/ {

static ResourceManager_Backend* ResourceManager_Backend_instance = nullptr;

ResourceManager_Backend* ResourceManager_Backend::s_instance() {
	return ResourceManager_Backend_instance;
}

void ResourceManager_Backend::s_create(const MemAllocRequest& req) {
	AX_ASSERT(ResourceManager_Backend_instance == nullptr);
	ResourceManager_Backend_instance = new (req) ResourceManager_Backend();
}

void ResourceManager_Backend::s_destroy() {
	AX_ASSERT(ResourceManager_Backend_instance);
	AxDelete::deleteObject(ResourceManager_Backend_instance);
}

void ResourceManager_Backend::onFrameBegin(RenderRequest_Backend* req) {
}

void ResourceManager_Backend::onFrameEnd(RenderRequest_Backend* req) {
	visit([&](auto& table){ table.scopedLock()->onFrameEnd(req); });
}

void ResourceManager_Backend::onFileChanged(FileDirWatcher_Result& result) {
	for (auto& e : result.list) {
		if (e.action == FileDirWatcher_Action::Modified) {
			hotReloadFile(e.filename);
		}
	}
}

void ResourceManager_Backend::hotReloadFile(StrView filename) {
	auto ext = FilePath::extension(filename);
	auto basenameWithExt = FilePath::basename(filename, true);

	auto imageFileType = ImageFileType_fromFileExt(ext);
	if (imageFileType != ImageFileType::None) {
		auto table = texture2DTable.scopedLock();
		if (auto* tex = table->findObject(filename)) {
			AX_LOG("Hot reload texture {}", filename);
			tex->hotReloadFile();
		}
		return;
	}

	if (basenameWithExt == "shaderResult.json") {
		auto shaderAssetPath = FilePath::dirname_sv(FilePath::dirname_sv(filename));
		auto table = shaderTable.scopedLock();
		if (auto* shader = table->findObject(shaderAssetPath)) {
			AX_LOG("Hot reload shader {}", shaderAssetPath);
			shader->hotReloadFile();
		}
		return;
	}
}

} // namespace
