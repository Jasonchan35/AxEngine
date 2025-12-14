module;
module AxRender;
import :StockObjects;
import :Renderer_Backend;
import :ResourceManager_Backend;

namespace ax::AxRender {

#if 0
#pragma mark ------ ShaderParamSpace_Backend ------
#endif

SPtr<ShaderParamSpace_Backend> ShaderParamSpace_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newShaderParamSpace(req, desc));
}


ShaderParamSpace_Backend::ShaderParamSpace_Backend(const CreateDesc& desc)
	: Base(desc) {}

template<class V>
void ShaderParamSpace_Backend::ConstBuffer::setVariableDefault(const VarInfo& varInfo, const V& value) {
	varInfo.assignValueToBuffer(_defaultValues, value);
}

template<class T, class INFO>
void ShaderParamSpace_Backend::_addParam(IArray<T>& arr, const INFO& paramInfo) {
	AX_ASSERT(paramInfo.bindSpace == bindSpace());

	auto& dst = arr.emplaceBack();
	dst.create(paramInfo);


	if constexpr (std::is_same_v<SamplerParam, T>) {
		auto str = dst.name().toTempString();
		if (auto sampler = str.extractFromPrefix("AxSamplerState_")) {
			_nameToSampler.emplaceBack(Pair_make(NameId::s_make(sampler), dst.name()));
		}
	}

	if constexpr (std::is_same_v<ConstBuffer, T> ) {
		for (auto& varInfo : dst.varInfos()) {
			auto str = varInfo.name().toTempString();
			
			if (auto sampler = str.extractFromPrefix("AxSamplerState_")) {
				_nameToSampler.emplaceBack(Pair_make(NameId::s_make(sampler), varInfo.name()));

			} else if (auto tex2d = str.extractFromPrefix("AxTexture2D_")) {
				_nameToTexture2D.emplaceBack(Pair_make(NameId::s_make(tex2d), varInfo.name()));

			} else if (auto tex3d = str.extractFromPrefix("AxTexture3D_")) {
				_nameToTexture3D.emplaceBack(Pair_make(NameId::s_make(tex3d), varInfo.name()));

			}
		}
	}
}

void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::ConstBuffer&   paramInfo) { _addParam(_constBuffers,        paramInfo); }
void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::Texture&       paramInfo) { _addParam(_textureParams,       paramInfo); }
void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::Sampler&       paramInfo) { _addParam(_samplerParams,       paramInfo); }
void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::StorageBuffer& paramInfo) { _addParam(_storageBufferParams, paramInfo); }

inline void ShaderParamSpace_Backend::SamplerParam::create(const Info& info) {
	ParamBase::create(info);
}

inline void ShaderParamSpace_Backend::TextureParam::create(const Info& info) {
	ParamBase::create(info);
	_dataType = info.dataType;
}

inline void ShaderParamSpace_Backend::ConstBuffer::create(const Info& info) {
	ParamBase::create(info);
	_defaultValues.resize(info.dataSize);
	_defaultValues.fillValues(0);

	_varInfos.ensureCapacity(info.variables.size());
	for (auto& src : info.variables) {
		_varInfos.emplaceBack(src);
	}
}

void ShaderParamSpace_Backend::StorageBufferParam::create(const Info& info) {
	ParamBase::create(info);
}

inline void ShaderParamSpace_Backend::ParamBase::create(const Info& info) {
	_name		= NameId::s_make(info.name);
	_dataType	= info.dataType;
	_stageFlags = info.stageFlags;
	_bindPoint	= info.bindPoint;
	_bindCount  = info.bindCount;
}

void ShaderParamSpace_Backend::setPropDefaultValue(NameId propName, const ShaderPropInfo& propInfo) {

	auto setVariableDefault = [&](auto& value) {
		for (auto& cb : _constBuffers) {
			if (auto* varInfo = cb.findVarInfo(propName)) {
				cb.setVariableDefault(*varInfo, value);
			}
		}
	};

	switch (propInfo.propType) {
	// constBuffer variable
	#define E(T, ...) \
		case ShaderPropType::T: setVariableDefault(propInfo.defaultValue.v_##T); break; \
	//---
		AX_ShaderPropType_Numbers_EnumList(E)
		E(Color3f)
		E(Color4f)
	#undef E
		case ShaderPropType::Texture2D: {
			if (auto* param = findTextureParam(propName)) {
				auto* tex= StockObjects::s_instance()->texture2Ds.get(propInfo.defaultValue.v_stockTextureId);
				param->setDefaultTexture(tex);
			}
		} break;

		case ShaderPropType::Sampler: {
			if (auto* param = findSamplerParam(propName)) {
				AX_UNUSED(param);
//				param->setPropDefaultValue(propInfo);
			}
		} break;
		
		default: throw Error_Undefined();
	}
}

NameId ShaderParamSpace_Backend::getTexture2DName(NameId name) const {
	for (auto& p : _nameToTexture2D) {
		if (p.first == name) return p.second;
	}
	return NameId();
}

NameId ShaderParamSpace_Backend::getTexture3DName(NameId name) const {
	for (auto& p : _nameToTexture3D) {
		if (p.first == name) return p.second;
	}
	return NameId();
}

NameId ShaderParamSpace_Backend::getSamplerName(NameId name) const {
	for (auto& p : _nameToSampler) {
		if (p.first == name) return p.second;
	}
	return NameId();
}

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

		auto& space = _shaderParamSpaces.ensureSizeAndGetElement(bindSpace);

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
	_propNameIds.ensureCapacity(_info.declare.props.size());

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
	_passes.ensureCapacity(n);

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