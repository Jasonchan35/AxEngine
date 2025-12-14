module;
module AxRender;
import :Material_Backend;
import :StockObjects;
import :RenderRequest_Backend;
import :RenderContext_Backend;
import :ResourceManager_Backend;

namespace ax::AxRender {

struct Renderer_Backend::PrivateData {
	Array<UPtr<RenderRequest_Backend>>	renderRequests;
	SPtr<Material_Backend>	commonMaterial;
};

Renderer_Backend::Renderer_Backend(const CreateDesc& desc) 
: Base(desc)
{
	_imguiFontAtlas.AddFontDefault();
	_privateData.newObject(AX_ALLOC_REQ);
}

Renderer_Backend::~Renderer_Backend() {
}

void Renderer_Backend::onCreate() {
	Base::onCreate();

	ResourceManager_Backend::s_create(AX_ALLOC_REQ);

//----- common material
	{
		auto& mtl = _privateData->commonMaterial;

		auto commonShaderFilename = StrView("ImportedAssets/Shaders/core/Common.axShader");
		auto commonParamShader = Shader::s_new(AX_ALLOC_REQ, commonShaderFilename);
		mtl = rttiCastCheck<Material_Backend>(Material::s_new(AX_ALLOC_REQ).ptr());
		mtl->setShader(commonParamShader);
	}

	StockObjects::s_create();

//----- render request
	if (_info.renderRequestCount > AxRenderConfig::kMaxRenderRequestCount)
		throw Error_Undefined();

	for (Int i = 0; i < _info.renderRequestCount; i++) {
		auto req = RenderRequest_Backend::s_new(AX_ALLOC_REQ, this, i);
		_privateData->renderRequests.emplaceBack(std::move(req));
	}
}

void Renderer_Backend::onDestroy() {
	StockObjects::s_destroy();
	_privateData.unref();
	ResourceManager_Backend::s_destroy();
	Base::onDestroy();
}

RenderRequest_Backend* Renderer_Backend::nextRenderRequest() {
	auto& reqs = _privateData->renderRequests;

	if (reqs.size() <= 0) {
		throw Error_Undefined("RenderRequest count is 0");
	}

	_renderSeqId++; // must start with 1, because lastUpdate seqId in resource is 0 by default
	auto* req = reqs[_renderSeqId % reqs.size()].ptr();
	req->waitCompletedAndReset(_renderSeqId);
	return req;
}

Material_Backend* Renderer_Backend::commonMaterial() {
	return _privateData->commonMaterial;
}

const Shader_Backend* Renderer_Backend::commonShader() {
	auto& m = _privateData->commonMaterial;
	return m ? m->shader_backend() : nullptr;
}

void Renderer_Backend::waitAllRenderCompleted() {
	for (auto& req : _privateData->renderRequests) {
		if (!req) { AX_ASSERT(false); continue; }
		req->waitCompleted();
	}
}

void Renderer_Backend::onFileChanged(FileDirWatcher_Result& result) {
	waitAllRenderCompleted();

	if (auto* mgr = ResourceManager_Backend::s_instance()) {
		mgr->onFileChanged(result);
	}
}

RenderRequest_Backend* Renderer_Backend::getRenderRequest(Int i) {
	return _privateData->renderRequests[i];
}

} // namespace ax::AxRender
