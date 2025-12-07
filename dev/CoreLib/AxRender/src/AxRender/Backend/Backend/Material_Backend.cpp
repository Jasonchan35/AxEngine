module;
module AxRender.Material_Backend;
import AxRender.RenderContext;
import AxRender.StockObjects;
import AxRender.Texture_Backend;
import AxRender.Renderer_Backend;

namespace ax::AxRender {

#if 0
#pragma mark "============ Material_Backend ==============="
#endif

Material_Backend::Material_Backend(const CreateDesc& desc) 
: Base(desc) {
}

void Material_Backend::logWarningOnce(StrView msg) {
	if (!_bShowWarning) return;
	_bShowWarning = false;
	AX_LOG_WARNING("Materai: {}\n shader={}", msg, shaderAssetPath());
}

void Material_Backend::setShader(Shader* shader_) {
	auto* shader = rttiCastCheck<Shader_Backend>(shader_);

	if (_shader == shader) return;
	_shader = shader;
	_passes.clear();
	Int passCount = shader->passes().size();

	_passes.reserve(passCount);

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