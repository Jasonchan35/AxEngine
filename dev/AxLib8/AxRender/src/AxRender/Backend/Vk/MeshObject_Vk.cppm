module;

export module AxRender:MeshObject_Vk;
export import :MeshObject_Backend; 

export namespace ax {
class MeshObject_Vk : public MeshObject_Backend {
	AX_RTTI_INFO(MeshObject_Vk, MeshObject_Backend)
public:
	MeshObject_Vk(const CreateDesc& desc) : Base(desc) {}
};

} // namespace