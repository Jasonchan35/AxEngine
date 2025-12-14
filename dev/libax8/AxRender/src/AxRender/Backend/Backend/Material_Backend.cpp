module;
module AxRender;
import :RenderContext;
import :StockObjects;
import :Texture_Backend;
import :Renderer_Backend;
import :Material_Backend;

namespace ax /*::AxRender*/ {

#if 0
#pragma mark "============ MaterialParamSpace_Backend ==============="
#endif

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
	_shaderParamSpace = rttiCastCheck<const ShaderParamSpace_Backend>(desc.paramSpace);
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
	AX_ASSERT(false);
	return false;
#else
	auto* dst = _findParam(_storageBufferParams, name);
	return dst ? dst->setStorageParam(v) : false;
#endif
}

#if 0
#pragma mark "============ Material_Backend ==============="
#endif

Material_Backend::Material_Backend(const CreateDesc& desc) 
: Base(desc) {
}

void Material_Backend::logWarningOnce(StrView msg) {
	if (!_bShowWarning) return;
	_bShowWarning = false;
	AX_LOG_WARNING("Material: {}\n shader={}", msg, shaderAssetPath());
}

void Material_Backend::setShader_backend(Shader* shader_) {
	auto* shader = rttiCastCheck<Shader_Backend>(shader_);

	if (_shader == shader) return;
	_shader = shader;
	_passes.clear();
	Int passCount = shader->passes().size();

	_passes.ensureCapacity(passCount);

	for (Int i = 0; i < passCount; i++) {
		MaterialPass_Backend_CreateDesc passDesc;
		passDesc.material = this;
		passDesc.passIndex = i;
		_passes.emplaceBack(onNewPass(passDesc));
	}

	onSetShader();
}

SPtr<Material_Backend> Material_Backend::s_new(const MemAllocRequest& req, Shader* shader) {
	CreateDesc desc;
	auto sp = SPtr_fromUPtr(Renderer_Backend::s_instance()->newMaterial(req, desc));
	if (sp) sp->setShader(shader);
	return sp;
}

SPtr<Material_Backend> Material_Backend::s_new(const MemAllocRequest& req, StrView shaderAssetPath) {
	auto shd = Shader::s_new(req, shaderAssetPath);
	return s_new(req, shd);
}

#if 0
#pragma mark "============ MaterialPass_Backend ==============="
#endif

void MaterialPass_Backend::logWarningOnce(StrView msg) {
	if (_material) _material->logWarningOnce(msg);
}

MaterialPass_Backend::MaterialPass_Backend(const CreateDesc& desc)
	: _material(desc.material)
	, _passIndex(desc.passIndex)
{
	AX_ASSERT(_material);
}

} // namespace