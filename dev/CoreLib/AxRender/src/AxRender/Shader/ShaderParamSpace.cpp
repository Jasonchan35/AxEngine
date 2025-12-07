module AxRender.ShaderParamSpace;
import AxRender.MaterialParamSpace;
import AxRender.Renderer_Backend;
import AxRender.MaterialParamSpace_Backend;
import AxRender.ShaderParamSpace_Backend;

namespace ax::AxRender {

SPtr<MaterialParamSpace> ShaderParamSpace::newMaterialParamSpace(const MemAllocRequest& req) const {
	MaterialParamSpace_CreateDesc desc;
	desc.paramSpace = this;
	return SPtr_fromUPtr(Renderer_Backend::s_instance()->newMaterialParamSpace(req, desc));
}

SPtr<ShaderParamSpace> ShaderParamSpace::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	return ShaderParamSpace_Backend::s_new(req, desc);
}

} // namespace