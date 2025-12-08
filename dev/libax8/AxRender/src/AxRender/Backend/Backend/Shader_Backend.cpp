module;
module AxRender;
import :StockObjects;
import :Renderer_Backend;
import :ResourceManager_Backend;

namespace ax::AxRender {

#if 0
#pragma mark ------ ShaderPass_Backend ------
#endif

ShaderPass_Backend::ShaderPass_Backend(const CreateDesc& desc)
	: _shader(desc.shader)
	, _name(NameId::s_make(desc.info->name))
	, _info(desc.info)
	, _stageInfo(desc.stageInfo)
{
	_createParamSpaces();
}

template<class T>
void ShaderPass_Backend::_addParamSpace(const Array<T>& paramInfoSpan) {
	auto isGlobalCommonShader = _shader->isGlobalCommonShader();

	for (auto& param : paramInfoSpan) {
		if (!isGlobalCommonShader) {
			if (param.bindSpace != BindSpace::Default)
				continue;
		}

		auto bindSpace = ax_enum_int(param.bindSpace);
		if (bindSpace < 0 || bindSpace >= ax_enum_int(BindSpace::_COUNT)) {
			AX_ASSERT(false);
			return;
		}

		auto& space = _shaderParamSpaces.ensureAt(bindSpace);

		if (!space) {
			ShaderParamSpace_CreateDesc blockDesc;
			blockDesc.bindSpace = param.bindSpace;
			auto p = ShaderParamSpace::s_new(AX_ALLOC_REQ, blockDesc);
			space = rttiCastCheck<ShaderParamSpace_Backend>(p.ptr());
		}

		space->addParam(param);
	}
}

void ShaderPass_Backend::_createParamSpaces() {
	_addParamSpace(_stageInfo->constBuffers);
	_addParamSpace(_stageInfo->storageBuffers);
	_addParamSpace(_stageInfo->textures);
	_addParamSpace(_stageInfo->samplers);

	for (auto& prop : _shader->info()->declare.props) {
		auto propName = NameId::s_make(prop.name);

		for (auto& space : _shaderParamSpaces) {
			if (space) space->setPropDefaultValue(propName, prop);
		}
	}

}

#if 0
#pragma mark ------ Shader_Backend ------
#endif

SPtr<Shader_Backend> Shader_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<Shader_Backend> o;
	if (ResourceManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.assetPath))
		o->_create(desc);
	return o;
}

SPtr<Shader_Backend> Shader_Backend::s_new(const MemAllocRequest& req, StrView assetPath) {
	CreateDesc desc;
	desc.assetPath = assetPath;
	return s_new(req, desc);
}

Shader_Backend::Shader_Backend(const CreateDesc& desc)
: Base(desc)
, resourceHandle(this)
{}

void Shader_Backend::onLoadFile() {
	auto* renderer = Renderer::s_instance();
	auto infoFilename = Fmt("{}/{}/shaderResult.json", _assetPath, renderer->api());
	JsonIO::readFile(infoFilename, _info);

	_isGlobalCommonShader = _info.declare.isGlobalCommonShader;

	_propNameIds.clear();
	_propNameIds.reserve(_info.declare.props.size());

	TempString samplerName;

	for (auto& src : _info.declare.props) {
		auto& dst = _propNameIds.emplaceBack();
		dst.name = NameId::s_make(src.name);
		dst.prop = &src;

		if (ShaderPropType_isTextureType(src.propType)) {
			samplerName = src.name;
			samplerName << "_AxSamplerState";
			dst.samplerName = NameId::s_make(samplerName);
		}
	}

	Int n = _info.declare.passes.size();

	if (_info.passStages.size() != n)
		throw Error_Undefined("Shader PassStages[] size mismatch");

	_passes.clear();
	_passes.reserve(n);

	for (Int i = 0; i < n; i++) {
		ShaderPass_Backend::CreateDesc passDesc;
		passDesc.shader = this;
		passDesc.info = &_info.declare.passes[i];
		passDesc.stageInfo = &_info.passStages[i];

		auto pass = onNewPass(passDesc);
		_passes.emplaceBack(std::move(pass));
	}
}

void Shader_Backend::onDestroy() {
	_passes.clear();
}

void Shader_Backend::hotReloadFile() {
	Renderer_Backend::s_instance()->waitAllRenderCompleted();
	onLoadFile();

	// TODO: Reload Material
}

} // namespace