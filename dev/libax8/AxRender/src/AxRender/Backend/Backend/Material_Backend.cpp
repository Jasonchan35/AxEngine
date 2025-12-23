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
void MaterialParamSpace_Backend_cloneParams(IArray<DST>& dst, const IArray<SRC>& srcArray) {
	Int n = srcArray.size();
	dst.resize(n);
	for (Int i = 0; i < n; i++) {
	 	dst[i].create(srcArray[i]);
	}
}

MaterialParamSpace_Backend::MaterialParamSpace_Backend(const CreateDesc& desc)
: Base(desc)
{
	_shaderParamSpace = rttiCastCheck<const ShaderParamSpace_Backend>(desc.shaderParamSpace);
	if (!_shaderParamSpace) {
		AX_ASSERT(false);
		return;
	}

	MaterialParamSpace_Backend_cloneParams(_constBuffers		, _shaderParamSpace->_constBuffers       );
	MaterialParamSpace_Backend_cloneParams(_samplerParams		, _shaderParamSpace->_samplerParams      );
	MaterialParamSpace_Backend_cloneParams(_textureParams		, _shaderParamSpace->_textureParams      );
	MaterialParamSpace_Backend_cloneParams(_storageBufferParams	, _shaderParamSpace->_storageBufferParams);
}

void MaterialParamSpace_Backend::ConstBufferParam::create(const ShaderParamSpace_Backend::ConstBuffer& shaderParam) {
	ParamBase::create(shaderParam);

	_shaderParam = &shaderParam;
	_dynamicGpuBuffer.create(	"Material_Backend-Dynamic-ConstBuffer",
								GpuBufferType::Const,
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
	auto f = _findParam(_textureParams, name);
	if (!f) return false;
	f->value.setTexture(tex);
	return true;
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

	auto f = _findParam(_samplerParams, name);
	if (!f) return false;
	f->value.setSampler(sampler);
	return true;
#endif
}

#if 0
#pragma mark "============ MaterialPass_Backend ==============="
#endif

void MaterialPass_Backend::logWarningOnce(StrView msg) {
	if (_material) _material->logWarningOnce(msg);
}

MaterialPass_Backend::MaterialPass_Backend(const CreateDesc& desc)
	: _material(desc.material)
	, _shaderPass(desc.shaderPass)
	, _passIndex(desc.passIndex)
{
	AX_ASSERT(_material);
	AX_ASSERT(_shaderPass);

	auto* commonMaterialPass = Renderer_Backend::s_instance()->commonMaterialPass();
		
	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto i = ax_enum_int(bindSpace);
		auto& ownParamSpace = _materialParamSpaces[i];
		if (!isOwnParamSpace(bindSpace)) {
			ownParamSpace = commonMaterialPass->getParamSpace(bindSpace);
			continue;
		}
		
		auto* shaderParamSpace = _shaderPass->getParamSpace(bindSpace);
		if (!shaderParamSpace) continue;
		ownParamSpace = shaderParamSpace->newMaterialParamSpace(AX_ALLOC_REQ); 
	}
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

void Material_Backend::onSetShader() {
	for (auto& pass : _passes) {
		if (pass) { pass->onSetShader(); }
	}
}

void Material_Backend::setShader_backend(Shader* shader_) {
	auto* shader = rttiCastCheck<Shader_Backend>(shader_);

	if (_shader_backend == shader) return;
	_shader_backend = shader;
	_passes.clear();
	Int passCount = shader->passCount();

	_passes.ensureCapacity(passCount);

	for (Int passIndex = 0; passIndex < passCount; passIndex++) {
		auto* shaderPass = shader->getPass(passIndex);
		if (!shaderPass) {
			throw Error_Undefined();
		}
		
		MaterialPass_CreateDesc passDesc;
		passDesc.material = this;
		passDesc.shaderPass = shaderPass;
		passDesc.passIndex = passIndex;
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

} // namespace