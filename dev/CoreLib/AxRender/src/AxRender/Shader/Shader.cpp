module AxRender.Shader;
import AxRender.Shader_Backend;

namespace ax::AxRender {

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