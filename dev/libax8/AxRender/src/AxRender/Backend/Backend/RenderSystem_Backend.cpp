module;
module AxRender;
import :Material_Backend;
import :StockObjects;
import :RenderRequest_Backend;
import :RenderContext_Backend;
import :RenderResourceManager_Backend;

namespace ax /*::AxRender*/ {

struct RenderSystem_Backend::PrivateData {
	Array<UPtr<RenderRequest_Backend>>	renderRequests;
	SPtr<Material_Backend>	commonMaterial;
};

RenderSystem_Backend::RenderSystem_Backend(const CreateDesc& desc) 
: Base(desc)
{
	_imguiFontAtlas.AddFontDefault();
	_privateData.newObject(AX_ALLOC_REQ);
}

RenderSystem_Backend::~RenderSystem_Backend() {
}

void RenderSystem_Backend::onCreate() {
	Base::onCreate();

	RenderResourceManager_Backend::s_create(AX_ALLOC_REQ);
	StockObjects::s_create();

//----- common material
	{
		auto& mtl = _privateData->commonMaterial;

		auto commonShaderFilename = StrView("ImportedAssets/Shaders/core/Common.axShader");
		auto commonParamShader = Shader::s_new(AX_ALLOC_REQ, commonShaderFilename);
		mtl = rttiCastCheck<Material_Backend>(Material::s_new(AX_ALLOC_REQ).ptr());
		mtl->setShader(commonParamShader);
	}

//----- render request
	if (_info.renderRequest.count > AxRenderConfig::kMaxRenderRequestCount)
		throw Error_Undefined();

	for (Int i = 0; i < _info.renderRequest.count; i++) {
		auto req = RenderRequest_Backend::s_new(AX_ALLOC_REQ, this, i);
		_privateData->renderRequests.emplaceBack(std::move(req));
	}
}

void RenderSystem_Backend::onDestroy() {
	StockObjects::s_destroy();
	_privateData.unref();
	RenderResourceManager_Backend::s_destroy();
	Base::onDestroy();
}

RenderRequest_Backend* RenderSystem_Backend::nextRenderRequest() {
	auto& reqs = _privateData->renderRequests;

	if (reqs.size() <= 0) {
		throw Error_Undefined("RenderRequest count is 0");
	}

	_renderSeqId++; // must start with 1, because lastUpdate seqId in resource is 0 by default
	auto* req = reqs[_renderSeqId % reqs.size()].ptr();
	req->waitCompletedAndReset(_renderSeqId);
	return req;
}

Material_Backend* RenderSystem_Backend::commonMaterial() {
	return _privateData->commonMaterial;
}

MaterialPass_Backend* RenderSystem_Backend::commonMaterialPass() {
	auto* p = commonMaterial();
	if (!p || p->passCount() <= 0) return nullptr;
	return p->getPass(0);  
}

Shader_Backend* RenderSystem_Backend::commonShader() {
	auto& m = _privateData->commonMaterial;
	return m ? m->shader_backend() : nullptr;
}

ShaderPass_Backend* RenderSystem_Backend::commonShaderPass() {
	auto* p = commonShader();
	if (!p || p->passCount() <= 0) return nullptr;
	return p->getPass(0);
}

void RenderSystem_Backend::waitAllRenderCompleted() {
	for (auto& req : _privateData->renderRequests) {
		if (!req) { AX_ASSERT(false); continue; }
		req->waitCompleted();
	}
}

void RenderSystem_Backend::onFileChanged(FileDirWatcher_Result& result) {
	waitAllRenderCompleted();

	if (auto* mgr = RenderResourceManager_Backend::s_instance()) {
		mgr->onFileChanged(result);
	}
}

RenderRequest_Backend* RenderSystem_Backend::getRenderRequest(Int i) {
	return _privateData->renderRequests[i];
}

} // namespace ax /*::AxRender*/
