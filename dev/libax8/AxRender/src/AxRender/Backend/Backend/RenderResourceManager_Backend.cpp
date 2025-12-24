module AxRender;
import :RenderResourceManager_Backend;
import :RenderRequest_Backend;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

static UPtr<RenderResourceManager_Backend> ResourceManager_Backend_instance;

RenderResourceManager_Backend* RenderResourceManager_Backend::s_instance() {
	return ResourceManager_Backend_instance;
}

void RenderResourceManager_Backend::s_create(const MemAllocRequest& req) {
	AX_ASSERT(ResourceManager_Backend_instance == nullptr);
	RenderResourceManager_CreateDesc desc;
	auto p = RenderSystem_Backend::s_instance()->newRenderResourceManager(req, desc);
	ResourceManager_Backend_instance = std::move(p);
}

void RenderResourceManager_Backend::s_destroy() {
	AX_ASSERT(ResourceManager_Backend_instance);
	ResourceManager_Backend_instance.unref();
}

void RenderResourceManager_Backend::onFrameBegin(RenderRequest_Backend* req) {
}

void RenderResourceManager_Backend::onFrameEnd(RenderRequest_Backend* req) {
	visit([&](auto& table){ table.scopedLock()->onFrameEnd(req); });
}

void RenderResourceManager_Backend::onFileChanged(FileDirWatcher_Result& result) {
	for (auto& e : result.list) {
		if (e.action == FileDirWatcher_Action::Modified) {
			hotReloadFile(e.filename);
		}
	}
}

void RenderResourceManager_Backend::hotReloadFile(StrView filename) {
	auto ext = FilePath::extension(filename);
	auto basenameWithExt = FilePath::basename(filename, true);

	auto imageFileType = ImageFileType_fromFileExt(ext);
	if (imageFileType != ImageFileType::None) {
		auto table = table_texture2D().scopedLock();
		if (auto* tex = table->findObject(filename)) {
			AX_LOG("Hot reload texture {}", filename);
			tex->hotReloadFile();
		}
		return;
	}

	if (basenameWithExt == "shaderResult.json") {
		auto shaderAssetPath = FilePath::dirname_sv(FilePath::dirname_sv(filename));
		auto table = table_shader().scopedLock();
		if (auto* shader = table->findObject(shaderAssetPath)) {
			AX_LOG("Hot reload shader {}", shaderAssetPath);
			shader->hotReloadFile();
		}
		return;
	}
}

} // namespace
