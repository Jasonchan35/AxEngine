module AxRender;
import :Shader_Dx12;
import :Renderer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

void ShaderParamSpace_Dx12::createDescTable() {
	auto addDescriptor = [this](Dx12DescriptorTable& tbl, ParamBase& p, D3D12_DESCRIPTOR_RANGE_TYPE type) {
		tbl.addDescriptor(type, p.bindPoint(), p.bindCount(), spaceType());
	};
	
	for (auto& p : _constBuffers) {
		addDescriptor(_descTable, p, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
	}

	for (auto& p : _storageBufferParams) {
		addDescriptor(_descTable, p, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
	}

	for (auto& p : _textureParams) {
		addDescriptor(_descTable, p, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	}

	for (auto& p : _samplerParams) {
		addDescriptor(_samplerDescTable, p, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
	}
}

bool VertexInputLayoutDesc_Dx12::init(const ShaderStageInfo& info, VertexLayout vertexLayout) {
	if (!vertexLayout) {
		vertexLayout = Vertex_None::s_layout();
	}

	for (auto& input : info.inputs) {
		auto* src = vertexLayout->find(input.semantic);
		if (!src) {
			AX_LOG("VertexLayout semantic '{}' not found", input.semantic);
			AX_ASSERT(false);
			return false; 
		}

		auto& dst = desc_dx12.emplaceBack();
		dst.SemanticName = ax_enum_str(VertexSemanticUtil::getType(input.semantic)).c_str();
		dst.SemanticIndex = VertexSemanticUtil::getIndex(input.semantic);
		dst.Format = Dx12Util::getDxDataType(src->dataType);
		dst.InputSlot = 0;
		dst.AlignedByteOffset = src->offset;
		dst.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
	}

	return true;
}

ShaderPass_Dx12::ShaderPass_Dx12(const CreateDesc& desc)
: Base(desc)
{
	D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	for (auto spaceType = SpaceType::Default; spaceType < SpaceType::_COUNT; ++spaceType) {
		auto* space = getParamSpace_dx12(spaceType);
		auto* selectSpace = space;
		if (shouldUseCommonParamSpace(spaceType)) {
			selectSpace = getCommonParamSpace_dx12(spaceType);
		} else {
			space->createDescTable();
		}

		auto i = ax_enum_int(spaceType);
		_pipelineRootParamList.addTable(shaderVisibility, selectSpace->_descTable,        &_descTableRootIndices[i]);
		_pipelineRootParamList.addTable(shaderVisibility, selectSpace->_samplerDescTable, &_samplerDescTableRootIndices[i]);
	}

	auto loadStage = [&](Stage& stage, ShaderStageFlags stageFlags) {
		if (!desc.info->getFuncName(stageFlags)) return;
		auto filename = Fmt("{}/Dx12/Shader_Dx12-{}-{}.bin", shader()->assetPath(), _name, stageFlags);
		stage.bytecode.openFile(filename);
	};

	_visitStages(loadStage);
	_createRootSignature();	
}

void ShaderPass_Dx12::_createRootSignature() {
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedDesc;
	versionedDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
	auto& desc = versionedDesc.Desc_1_0;
	desc = {};

	if (!isCompute()) {
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}	
	desc.NumParameters		= Dx12Util::castUINT(_pipelineRootParamList.parameters.size());
	desc.pParameters		= _pipelineRootParamList.parameters.data();
	desc.NumStaticSamplers	= Dx12Util::castUINT(_pipelineRootParamList.staticSamplers.size());
	desc.pStaticSamplers	= _pipelineRootParamList.staticSamplers.data();

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	auto hr = D3D12SerializeVersionedRootSignature(&versionedDesc,
	                                               rootSignatureBlob.ptrForInit(),
	                                               errorBlob.ptrForInit());
	Dx12Util::throwIfError(hr, errorBlob);

	auto* d3dDevice = Renderer_Dx12::s_d3dDevice();
	hr = d3dDevice->CreateRootSignature(0, 
										rootSignatureBlob->GetBufferPointer(), 
										rootSignatureBlob->GetBufferSize(), 
										IID_PPV_ARGS(_rootSignature.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

auto ShaderPass_Dx12::getOrAddPipeline(const Pipeline::PsoKey& key) -> Pipeline* {
	for (auto& pipeline : _pipelineTable) {
		if (pipeline->key == key) {
			return pipeline.ptr();
		}
	}

	VertexInputLayoutDesc_Dx12 vertexInputLayoutDesc;
	if (_vsStage.bytecode.size() <= 0) { AX_ASSERT(false); return nullptr; }

	if (!vertexInputLayoutDesc.init(*_stageInfo, key.vertexLayout)) {
		AX_ASSERT(false);
		return nullptr;
	}	

	auto& rs =  _info->renderState;
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout.NumElements = Dx12Util::castUINT(vertexInputLayoutDesc.desc_dx12.size());
	psoDesc.InputLayout.pInputElementDescs = vertexInputLayoutDesc.desc_dx12.data();
//-----
	psoDesc.pRootSignature     = _rootSignature;
	psoDesc.VS.pShaderBytecode = _vsStage.bytecode.data();
	psoDesc.VS.BytecodeLength  = _vsStage.bytecode.sizeInBytes();
	psoDesc.PS.pShaderBytecode = _psStage.bytecode.data();
	psoDesc.PS.BytecodeLength  = _psStage.bytecode.sizeInBytes();
	psoDesc.GS.pShaderBytecode = _gsStage.bytecode.data();
	psoDesc.GS.BytecodeLength  = _gsStage.bytecode.sizeInBytes();

//-----
	psoDesc.RasterizerState.FillMode = key.debugWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;

	psoDesc.RasterizerState.FrontCounterClockwise = true;
	psoDesc.RasterizerState.CullMode              = Dx12Util::getDxCullMode(rs.cull);
	psoDesc.DepthStencilState.DepthEnable         = rs.depthTest.isEnable();
	psoDesc.DepthStencilState.DepthWriteMask      = rs.depthTest.writeMask
		                                                ? D3D12_DEPTH_WRITE_MASK_ALL
		                                                : D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc           = Dx12Util::getDxDepthTestOp(rs.depthTest.op);
	psoDesc.DepthStencilState.StencilEnable       = false;
	psoDesc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable       = TRUE;
	
	psoDesc.RasterizerState.MultisampleEnable     = FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	psoDesc.RasterizerState.ForcedSampleCount     = 0;
	psoDesc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
//-----
	{	// blend
		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC rt;
		rt.LogicOpEnable = false;

		rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		if (rs.blend.isEnable()) {
			rt.BlendEnable	= true;
			rt.BlendOp			= Dx12Util::getDxBlendOp(rs.blend.rgb.op);
			rt.BlendOpAlpha		= Dx12Util::getDxBlendOp(rs.blend.alpha.op);
			rt.SrcBlend			= Dx12Util::getDxBlendFactor(rs.blend.rgb.srcFactor);
			rt.DestBlend		= Dx12Util::getDxBlendFactor(rs.blend.rgb.dstFactor);
			rt.SrcBlendAlpha	= Dx12Util::getDxBlendFactor(rs.blend.alpha.srcFactor);
			rt.DestBlendAlpha	= Dx12Util::getDxBlendFactor(rs.blend.alpha.dstFactor);
		}else{
			rt.BlendEnable	= false;
		}

		psoDesc.BlendState.RenderTarget[0] = rt;

		rt.BlendEnable = false;
		for (UINT i = 1; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			psoDesc.BlendState.RenderTarget[ i ] = rt;
	}

//-----
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = Dx12Util::getDxPrimitiveTopologyType(key.primitiveType);
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	auto& outPipeline = _pipelineTable.emplaceNewObject(AX_ALLOC_REQ);
	outPipeline->key = key;

	auto* dev = Renderer_Dx12::s_d3dDevice();
	auto hr = dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(outPipeline->pipelineState.ptrForInit()));
	Dx12Util::throwIfError(hr);
	return outPipeline;
}

bool ShaderPass_Dx12::_bindPipeline(RenderRequest_Dx12* req, Cmd_DrawCall& cmd) const {
	if (!req) { AX_ASSERT(false); return false; }

	auto* renderPass = req->currentRenderPass_dx12();
	if (!renderPass) { AX_ASSERT(false); return false; }

	Pipeline::PsoKey key;
	key.vertexLayout	= cmd.vertexLayout;
	key.primitiveType	= cmd.primitiveType;

	auto* pipeline = ax_const_cast(this)->getOrAddPipeline(key);
	if (!pipeline) { AX_ASSERT(false); return false; }

	auto& cmdList = req->graphCmdBuf_dx12();
	cmdList->SetGraphicsRootSignature(ax_const_cast(_rootSignature));
	cmdList->SetPipelineState(pipeline->pipelineState);
	
	return true;
}

} // namespace

#endif // #if AX_RENDERER_DX12
