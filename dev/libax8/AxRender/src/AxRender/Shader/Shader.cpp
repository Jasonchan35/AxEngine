module AxRender;
import :Shader_Backend;
import :Renderer_Backend;
import :Material_Backend;

namespace ax /*::AxRender*/ {

void ShaderStageInfo::loadFile(StrView filename_) {
	filename = filename_;
	JsonIO::readFile(filename_, *this);
}

const String& ShaderPassInfo::getFuncName(ShaderStageFlags mask) const {
	switch (mask) {
		case ShaderStageFlags::Vertex:		return vsFunc;
		case ShaderStageFlags::Pixel:		return psFunc;
		case ShaderStageFlags::Geometry:	return gsFunc;
		case ShaderStageFlags::Compute:		return csFunc;
		default: return String::kEmpty();
	}
}

ShaderPassInfo* ShaderDeclareInfo::findPass(StrView name) {
	for (auto& p : passes) {
		if (p.name == name) return &p;
	}
	return nullptr;
}

SPtr<MaterialParamSpace> ShaderParamSpace::newMaterialParamSpace(const MemAllocRequest& req) const {
	MaterialParamSpace_CreateDesc desc;
	desc.shaderParamSpace = this;
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newMaterialParamSpace(req, desc));
}

SPtr<ShaderParamSpace> ShaderParamSpace::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return ShaderParamSpace_Backend::s_new(req, desc);
}

SPtr<Shader> Shader::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return Shader_Backend::s_new(req, desc);
}

SPtr<Shader> Shader::s_new(const MemAllocRequest& req, StrView assetPath) {
	return Shader_Backend::s_new(req, assetPath);
}

Shader::Shader(const CreateDesc& desc) 
: _assetPath(desc.assetPath)
{
}

} // namespace