module;

export module AxRender:MeshObject_Dx12;
export import :MeshObject_Backend; 

export namespace ax {
class MeshObject_Dx12 : public MeshObject_Backend {
	AX_RTTI_INFO(MeshObject_Dx12, MeshObject_Backend)
public:
	MeshObject_Dx12(const CreateDesc& desc) : Base(desc) {}
};

} // namespace