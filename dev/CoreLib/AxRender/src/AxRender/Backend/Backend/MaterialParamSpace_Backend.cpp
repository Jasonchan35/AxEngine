module;
module AxRender.MaterialParamSpace_Backend;
import AxRender.Texture_Backend;

namespace ax::AxRender {

template<class DST, class SRC>
void MaterialParamSpace_Backend_cloneParams(DST& dst, SRC srcSpan) {
	Int n = srcSpan.size();
	dst.resize(n);
	for (Int i = 0; i < n; i++) {
		dst[i].create(srcSpan[i]);
	}
}

MaterialParamSpace_Backend::MaterialParamSpace_Backend(const CreateDesc& desc)
: Base(desc)
{
	_shaderParamSpace = rttiCastCheck<ShaderParamSpace_Backend>(desc.paramSpace);
	if (!_shaderParamSpace) {
		AX_ASSERT(false);
		return;
	}

	MaterialParamSpace_Backend_cloneParams(_constBuffers		, _shaderParamSpace->constBuffers());
	MaterialParamSpace_Backend_cloneParams(_samplerParams		, _shaderParamSpace->samplerParams());
	MaterialParamSpace_Backend_cloneParams(_textureParams		, _shaderParamSpace->textureParams());
	MaterialParamSpace_Backend_cloneParams(_storageBufferParams	, _shaderParamSpace->storageBufferParams());
}

void MaterialParamSpace_Backend::ConstBuffer::create(const ShaderParamSpace_Backend::ConstBuffer& shaderParam) {
	ParamBase::create(shaderParam);

	_shaderParam = &shaderParam;
	_dynamicGpuBuffer.create(	"Material_Backend-Uniform",
								GpuBufferType::Uniform,
								_shaderParam->dataSize());

	_dynamicGpuBuffer.setData(shaderParam.defaultValues());
}

void MaterialParamSpace_Backend::SamplerParam::create(const ShaderParamSpace_Backend::SamplerParam& shaderParam) {
	ParamBase::create(shaderParam);
	_shaderParam = &shaderParam;
	_sampler	= shaderParam.defaultSampler();
}

void MaterialParamSpace_Backend::TextureParam::create(const ShaderParamSpace_Backend::TextureParam& shaderParam) {
	ParamBase::create(shaderParam);
	_shaderParam = &shaderParam;
	_texture	= shaderParam.defaultTexture();
}

void MaterialParamSpace_Backend::StorageBufferParam::create(const ShaderParamSpace_Backend::StorageBufferParam& shaderParam) {
	ParamBase::create(shaderParam);

	_shaderParam = &shaderParam;
}

bool MaterialParamSpace_Backend::setParam(NameId name, Texture2D* tex) {
#if AX_RENDER_BINDLESS
	if (!_shaderParamSpace) return false;
	auto bindlessName = _shaderParamSpace->getTexture2DName(name);
	if (!bindlessName) return false;

	auto slot = ResourceSlotId::None;
	if (auto* tex_ = rttiCastCheck<Texture2D_Backend>(tex)) {
		slot = tex_->resourceHandle.slotId();
	}
	return setParam(bindlessName, ax_enum_int(slot));

#else
	auto* dst = _findParam(_textureParams, name);
	return dst ? dst->setTexture(tex) : false;
#endif
}

bool MaterialParamSpace_Backend::setParam(NameId name, Texture3D* tex) {
#if AX_RENDER_BINDLESS
	if (!_shaderParamSpace) return false;
	auto bindlessName = _shaderParamSpace->getTexture3DName(name);
	if (!bindlessName) return false;

	auto slot = ResourceSlotId::None;
	if (auto* tex_ = rttiCastCheck<Texture3D_Backend>(tex)) {
		slot = tex_->resourceHandle.slotId();
	}
	return setParam(bindlessName, ax_enum_int(slot));

#else
	auto* dst = _findParam(_textureParams, name);
	return dst ? dst->setTexture(tex) : false;
#endif
}

bool MaterialParamSpace_Backend::setParam(NameId name, Sampler* sampler) {
#if AX_RENDER_BINDLESS
	if (!_shaderParamSpace) return false;
	auto bindlessName = _shaderParamSpace->getSamplerName(name);
	if (!bindlessName) return false;

	auto slot = ResourceSlotId::None;
	if (auto* sampler_ = rttiCastCheck<Sampler_Backend>(sampler)) {
		slot = sampler_->resourceHandle.slotId();
	}
	return setParam(bindlessName, ax_enum_int(slot));

#else
	auto samplerName = _shaderParamSpace->getSamplerName(name);
	if (!samplerName) return false;

	auto* dst = _findParam(_samplerParams, samplerName);
	return dst ? dst->setSampler(sampler) : false;
#endif
}

bool MaterialParamSpace_Backend::setParam(NameId name, StorageBuffer* v) {
#if AX_RENDER_BINDLESS
	AX_NOT_IMPLEMENTED();
	return false;
#else
	auto* dst = _findParam(_storageBufferParams, name);
	return dst ? dst->setStorageParam(v) : false;
#endif
}

} // namespace