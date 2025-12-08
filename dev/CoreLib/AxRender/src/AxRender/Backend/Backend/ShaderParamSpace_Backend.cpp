module;
module AxRender.ShaderParamSpace_Backend;
import AxRender.ShaderParamSpace;
import AxRender.MaterialParamSpace_Backend;
import AxRender.StockObjects;
import AxRender.Renderer_Backend;

namespace ax::AxRender {

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

	_varInfos.reserve(info.variables.size());
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
//				param->setPropDefaultValue(propInfo);
			}
		} break;
		
		default: Error_Undefined(AX_SRC_LOC);
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

} // namespace
