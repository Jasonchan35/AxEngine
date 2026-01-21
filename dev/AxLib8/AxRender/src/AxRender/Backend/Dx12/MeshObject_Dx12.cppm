module;

export module AxRender:MeshObject_Dx12;
export import :MeshObject_Backend; 

export namespace ax {

struct MeshObject_GpuData_Dx12 {
	D3D12_GPU_VIRTUAL_ADDRESS vertexBufferLocation = 0;
	D3D12_GPU_VIRTUAL_ADDRESS indexBufferLocation  = 0;
	UINT vertexBufferSizeInBytes = 0;
	UINT indexBufferSizeInBytes  = 0;
};

class MeshObject_Dx12 : public MeshObject_Backend {
	AX_RTTI_INFO(MeshObject_Dx12, MeshObject_Backend)
public:
	MeshObject_Dx12(const CreateDesc& desc) : Base(desc) {}
};

} // namespace