module;

export module AxRender:RenderIndirectExecute_Dx12;
export import :Shader_Backend;

namespace ax {

class RenderIndirectExecute_Backend : public RenderObject {
	AX_RTTI_INFO(RenderIndirectExecute_Backend, RenderObject)
public:
	using BindPoint = ShaderParamBindPoint;
	using BindCount = ShaderParamBindCount;
	using BindSpace = ShaderParamBindSpace;
};

struct AxMeshShaderIndirectDrawArgs_Dx12 {
	AxMeshShaderDrawRootConst 	rootConst;
	D3D12_DISPATCH_ARGUMENTS	args;
	
	void setGroupCount(UINT x, UINT y, UINT z) {
		args.ThreadGroupCountX = x;
		args.ThreadGroupCountY = y;
		args.ThreadGroupCountZ = z;
	}
};

class RenderIndirectExecute_Dx12 : public RenderIndirectExecute_Backend {
	AX_RTTI_INFO(RenderIndirectExecute_Dx12, RenderIndirectExecute_Backend)
public:
	void create(Shader* shader);
	
	StructuredGpuBuffer_<AxMeshShaderIndirectDrawArgs_Dx12> _drawArgsBuffer;
	
	
	ComPtr<ID3D12CommandSignature> _commandSignature;
};

} // namespace