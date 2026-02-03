module AxRender;
import :Shader_Backend;
import :RenderSystem_Backend;

namespace ax /*::AxRender*/ {

void ShaderStageInfo::loadFile(StrView filename_) {
	filename = filename_;
	JsonIO::readFile(filename_, *this);
}

const String& ShaderPassInfo::getFuncName(ShaderStageFlags mask) const {
	switch (mask) {
		case ShaderStageFlags::Vertex:			return vertexFunc;
		case ShaderStageFlags::Pixel:			return pixelFunc;
		case ShaderStageFlags::Geometry:		return geometryFunc;
		case ShaderStageFlags::Compute:			return computeFunc;
		case ShaderStageFlags::Mesh:			return meshFunc;
		case ShaderStageFlags::Amplification:	return amplificationFunc;
		default: return String::kEmpty();
	}
}

ShaderPassInfo* ShaderDeclareInfo::findPass(StrView name) {
	for (auto& p : passes) {
		if (p.name == name) return &p;
	}
	return nullptr;
}

SPtr<Shader> Shader::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return Shader_Backend::s_new(req, desc);
}

SPtr<Shader> Shader::s_new(const MemAllocRequest& req, StrView assetPath) {
	return Shader_Backend::s_new(req, assetPath);
}

Shader::Shader(const CreateDesc& desc) 
	: _assetPath(desc.assetPath)
	, objectSlot(this)
{
}

} // namespace