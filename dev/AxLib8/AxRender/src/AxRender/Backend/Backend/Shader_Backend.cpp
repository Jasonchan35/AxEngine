module;
module AxRender;
import :RenderStockObjects;
import :Material_Backend;
import :RenderSystem_Backend;
import :RenderObjectManager_Backend;

namespace ax /*::AxRender*/ {

#if 0
#pragma mark ------ ShaderParamSpace_Backend ------
#endif

SPtr<ShaderParamSpace_Backend> ShaderParamSpace_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return SPtr_fromUPtr(RenderSystem_Backend::s_instance()->newShaderParamSpace(req, desc));
}

TempString ShaderParamSpace_Backend::debugName() const {
	return Fmt("BindSpace.{:10} shaderPass={}", _bindSpace, _shaderPass ? _shaderPass->debugName() : "");
}

ShaderParamSpace_Backend::ShaderParamSpace_Backend(const CreateDesc& desc)
: _shaderPass(desc.shaderPass)
, _bindSpace(desc.bindSpace)
, _isGlobalCommonShader(desc.shaderPass->isGlobalCommonShader())
{}

void ShaderParamSpace_Backend::_postCreate(ShaderPass_Backend* shdPass) {
	auto* info = shdPass->shader()->info();
	for (auto& prop : info->declare.props) {
		auto propName = NameId::s_make(prop.name);
		setPropDefaultValue(propName, prop);
	}
	
	if (_bindSpace == BindSpace::Bindless) {
		return;
	}
	
	Int constBufferIndex = 0;
	for (auto& param : _constBuffers) {
		for (auto& varInfo : param.varInfos()) {
			auto& dst       = _nameToVarInfo.emplaceBack();
			dst.name        = varInfo.name();
			dst.constBufferIndex = constBufferIndex; 
			dst.varInfo     = &varInfo;
		}
		
		++constBufferIndex;
	}
	
	_nameToVarInfo.sort();
}

template<class V>
void ShaderParamSpace_Backend::ConstBufferParam::setVariableDefault(const VarInfo& varInfo, const V& value) {
	varInfo.assignValueToBuffer(_defaultValues, value);
}

template<class T, class INFO>
void ShaderParamSpace_Backend::_addParam(IArray<T>& arr, const INFO& paramInfo) {
	AX_ASSERT(paramInfo.bindSpace == bindSpace());

	auto& dst = arr.emplaceBack();
	dst.create(paramInfo);

	if constexpr (std::is_same_v<SamplerParam, T>) {
		auto str = dst.name().toString();
		if (auto samplerName = str.extractFromPrefix("AxSamplerState_")) {
			_nameToSampler.emplaceBack(Pair_make(NameId::s_make(samplerName), dst.name()));
		}
	}

	if constexpr (std::is_same_v<ConstBufferParam, T> ) {
		for (auto& varInfo : dst.varInfos()) {
			auto str = varInfo.name().toString();
			
			if (auto samplerName = str.extractFromPrefix("AxSamplerState_")) {
				_nameToSampler.emplaceBack(Pair_make(NameId::s_make(samplerName), varInfo.name()));

			} else if (auto tex2dName = str.extractFromPrefix("AxTexture2D_")) {
				_nameToTexture2D.emplaceBack(Pair_make(NameId::s_make(tex2dName), varInfo.name()));

			} else if (auto tex3dName = str.extractFromPrefix("AxTexture3D_")) {
				_nameToTexture3D.emplaceBack(Pair_make(NameId::s_make(tex3dName), varInfo.name()));

			}
		}
	}
}

void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::Texture&          paramInfo) { _addParam(_textureParams,          paramInfo); }
void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::Sampler&          paramInfo) { _addParam(_samplerParams,          paramInfo); }
void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::StructuredBuffer& paramInfo) { _addParam(_structuredBufferParams, paramInfo); }
void ShaderParamSpace_Backend::addParam(const ShaderStageInfo::ConstBuffer&      paramInfo) { _addParam(_constBuffers,           paramInfo); }

inline void ShaderParamSpace_Backend::SamplerParam::create(const Info& info) {
	ParamBase::create(info);
	_dynamicSampler = info.dynamicSampler; 
}

inline void ShaderParamSpace_Backend::TextureParam::create(const Info& info) {
	ParamBase::create(info);
	_dataType = info.dataType;
}

inline void ShaderParamSpace_Backend::ConstBufferParam::create(const Info& info) {
	ParamBase::create(info);
	_defaultValues.resize(info.stride);
	_defaultValues.fillValues(0); // set all to zero by default

	_varInfos.ensureCapacity(info.variables.size());
	for (auto& src : info.variables) {
		_varInfos.emplaceBack(src);
	}
}

void ShaderParamSpace_Backend::StructuredBufferParam::create(const Info& info) {
	ParamBase::create(info);
	_stride = info.stride;
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
				auto* tex= RenderStockObjects::s_instance()->texture2Ds->get(propInfo.defaultValue.v_stockTextureId);
				param->setDefaultTexture(tex);
			}
		} break;

		case ShaderPropType::Sampler: {
			if (auto* param = findSamplerParam(propName)) {
				// TODO - pick the sampler base on propInfo.default
				auto* sampler = RenderStockObjects::s_instance()->samplers->defaultValue.ptr();
				param->setDefaultSampler(sampler);
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
	, _isGlobalCommonShader(_shader->isGlobalCommonShader())
	, _passIndex(desc.passIndex)
	, _name(NameId::s_make(desc.info->name))
	, _info(desc.info)
	, _stageInfo(desc.stageInfo)
{
	{ // create BindSpace::Default (Vulkan requires all bindspace)
		auto bindSpace = BindSpace::Default;
		ShaderParamSpace_CreateDesc spaceDesc;
		spaceDesc.bindSpace = bindSpace;
		spaceDesc.shaderPass = this;
		_shaderParamSpaces[ax_enum_int(bindSpace)] = ShaderParamSpace_Backend::s_new(AX_NEW, spaceDesc);
	}
	
	_createParamSpaces();
}

ShaderPass_Backend::~ShaderPass_Backend() {
	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		if (auto* ownParamSpace = getOwnParamSpace(bindSpace)) {
			ownParamSpace->_shaderPass = nullptr; // the lifespan of ParamSpace might be longer
		}
	}
}

const ShaderPass_Backend* ShaderPass_Backend::s_globalCommonShaderPass() {
	auto* pass = MaterialPass_Backend::s_globalCommonMaterialPass();
	return pass ? pass->shaderPass() : nullptr;
}

template<class T>
void ShaderPass_Backend::_addParamToSpace(const Array<T>& paramInfoSpan) {
	for (auto& param : paramInfoSpan) {
		auto s = param.bindSpace;
		if (!isOwnParamSpace(s)) continue;

		auto& paramSpace = _shaderParamSpaces[ax_enum_int(s)];
		if (!paramSpace) {
			ShaderParamSpace_CreateDesc spaceDesc;
			spaceDesc.bindSpace = s;
			spaceDesc.shaderPass = this;
			auto newSpace = ShaderParamSpace_Backend::s_new(AX_NEW, spaceDesc);
			paramSpace.ref(newSpace.ptr());
		}

		ax_const_cast(paramSpace.ptr())->addParam(param);
	}
}

TempString ShaderPass_Backend::debugName() const {
	return Fmt("{} shader={}", _name, _shader ? _shader->debugName() : "");
}

void ShaderPass_Backend::_createParamSpaces() {
	_addParamToSpace(_stageInfo->constBuffers);
	_addParamToSpace(_stageInfo->structuredBuffers);
	_addParamToSpace(_stageInfo->textures);
	_addParamToSpace(_stageInfo->samplers);

	auto* commonShaderPass = s_globalCommonShaderPass(); 

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto i = ax_enum_int(bindSpace);
		if (!isOwnParamSpace(bindSpace)) {
			auto* commonParamSpace = commonShaderPass->getParamSpace(bindSpace);
			_shaderParamSpaces[i] = commonParamSpace;
			continue;
		}

		auto* ownParamSpace = getOwnParamSpace(bindSpace);
		if (!ownParamSpace) continue;

		ownParamSpace->_postCreate(this);
	}
}

#if 0
#pragma mark ------ Shader_Backend ------
#endif

SPtr<Shader_Backend> Shader_Backend::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<Shader_Backend> o;
	if (RenderObjectManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.assetPath))
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
, objectSlot(this, desc.isFallbackDefault)
{}

void Shader_Backend::onLoadFile() {
	auto* renderSystem = RenderSystem::s_instance();
	auto infoFilename = Fmt("{}/{}/shaderResult.json", _assetPath, renderSystem->api());
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

	for (i32 i = 0; i < n; i++) {
		ShaderPass_Backend::CreateDesc passDesc;
		passDesc.shader    = this;
		passDesc.passIndex = i;
		passDesc.info      = &_info.declare.passes[i];
		passDesc.stageInfo = &_info.passStages[i];

		auto pass = onNewPass(passDesc);
		_passes.emplaceBack(std::move(pass));
	}
}

void Shader_Backend::onDestroy() {
	_passes.clear();
}

void Shader_Backend::hotReloadFile() {
	RenderSystem_Backend::s_instance()->waitAllRenderCompleted();
	onLoadFile();

	// TODO: Reload Material
}

TempString Shader_Backend::debugName() const {
	return Fmt("{}", _assetPath);
}

} // namespace