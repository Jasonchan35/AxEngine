module AxRender;
import :RenderIndirectExecute_Dx12;
import :Shader_Dx12;
import :RenderObjectManager_Dx12;

namespace ax {

void RenderIndirectExecute_Dx12::create(Shader* shader_) {
	auto* shader     = rttiCastCheck<Shader_Dx12>(shader_);
	auto* shaderPass = rttiCastCheck<ShaderPass_Dx12>(shader->getPass(0));

	Array<D3D12_INDIRECT_ARGUMENT_DESC, 4> argumentDescList;
	{
		auto& dst = argumentDescList.emplaceBack();
		dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
		dst.Constant.RootParameterIndex = shaderPass->_rootConstRootParamIndex;
		dst.Constant.DestOffsetIn32BitValues = 0;
		dst.Constant.Num32BitValuesToSet = Dx12Util::castUINT(shaderPass->_rootConstSizeInBytes / 4);
	}
	
	{
		auto& dst = argumentDescList.emplaceBack();
		dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
	}

	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
	commandSignatureDesc.ByteStride       = sizeof(AxMeshShaderIndirectDrawArgs_Dx12);
	commandSignatureDesc.NumArgumentDescs = ax_safe_cast_from(argumentDescList.size());
	commandSignatureDesc.pArgumentDescs   = argumentDescList.data();

	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	auto hr = dev->CreateCommandSignature(&commandSignatureDesc,
										  shaderPass->_rootSignature,
										  IID_PPV_ARGS(_commandSignature.ptrForInit()));
	Dx12Util::throwIfError(hr);
	
	auto* objMgr = RenderObjectManager_Dx12::s_instance();

	_drawArgsBuffer.create(AX_NEW,
	                       GpuBufferType::IndirectArgument,
	                       FmtName("{}_Indirect", shader->name()),
	                       objMgr->_meshShaderIndirectDrawArgsBufferPool); 
}

} // namespace
