module AxRender;
import :MeshObject_Backend;
import :RenderObjectManager_Backend;

namespace ax {

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req, const CreateDesc& desc) {
	SPtr<MeshObject_Backend> o;
	RenderObjectManager_Backend::s_instance()->getOrNewResource(o, req, desc, desc.assetPath);
	return o;	
}

SPtr<MeshObject> MeshObject::s_new(const MemAllocRequest& req) {
	MeshObject_CreateDesc desc;
	return s_new(req, desc);
}

} // namespace
