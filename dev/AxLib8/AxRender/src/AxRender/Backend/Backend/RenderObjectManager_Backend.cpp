module AxRender;
import :RenderObjectManager_Backend;
import :RenderRequest_Backend;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

static UPtr<RenderObjectManager_Backend> RenderObjectManager_Backend_instance;

RenderObjectManager_Backend* RenderObjectManager_Backend::s_instance() {
	return RenderObjectManager_Backend_instance;
}

void RenderObjectManager_Backend::s_create(const MemAllocRequest& req) {
	AX_ASSERT(RenderObjectManager_Backend_instance == nullptr);
	RenderObjectManager_CreateDesc desc;
	auto p = RenderSystem_Backend::s_instance()->newRenderObjectManager(req, desc);
	RenderObjectManager_Backend_instance = std::move(p);
	RenderObjectManager_Backend_instance->_postCreate();
}

void RenderObjectManager_Backend::s_destroy() {
	AX_ASSERT(RenderObjectManager_Backend_instance);
	RenderObjectManager_Backend_instance.unref();
}

RenderObjectManager_Backend::RenderObjectManager_Backend(const CreateDesc& desc) {
}

void RenderObjectManager_Backend::onFrameBegin(RenderRequest_Backend* req) {
}

void RenderObjectManager_Backend::onFrameEnd(RenderRequest_Backend* req) {
	visit([&](auto& table){ table.scopedLock()->onFrameEnd(req); });
}

void RenderObjectManager_Backend::onFileChanged(FileDirWatcher_Result& result) {
	for (auto& e : result.list) {
		if (e.action == FileDirWatcher_Action::Modified) {
			hotReloadFile(e.filename);
		}
	}
}

void RenderObjectManager_Backend::hotReloadFile(StrView filename) {
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

void RenderObjectManager_Backend::_postCreate() {
	_globalCommonMaterial = Material_Backend::s_new(AX_NEW, "ImportedAssets/Shaders/core/AxGlobalCommon.axShader");
	_indirectDrawMaterial = Material_Backend::s_new(AX_NEW, "ImportedAssets/Shaders/core/IndirectDraw.axComputeShader");
	
	RenderStockObjects::s_create();

#if AX_RENDER_BINDLESS
	auto* commonShaderPass = ShaderPass_Backend::s_globalCommonShaderPass();
	auto* bindlessSpace    = commonShaderPass->getParamSpace(ShaderParamBindSpace::Bindless);

	bindless.AxBindless_SamplerState = bindlessSpace->findSamplerParam(AX_NAMEID("AxBindless_SamplerState"));
	if (!bindless.AxBindless_SamplerState) throw Error_Undefined();
	
	bindless.AxBindless_Texture2D = bindlessSpace->findTextureParam(AX_NAMEID("AxBindless_Texture2D"));
	if (!bindless.AxBindless_Texture2D) throw Error_Undefined();
	
	bindless.AxBindless_Texture3D = bindlessSpace->findTextureParam(AX_NAMEID("AxBindless_Texture3D"));
	if (!bindless.AxBindless_Texture3D) throw Error_Undefined();
#endif

	onPostCreate();
}

} // namespace
