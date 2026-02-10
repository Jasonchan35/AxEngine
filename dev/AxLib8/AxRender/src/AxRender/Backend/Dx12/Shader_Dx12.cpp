module;

module AxRender;
import :Shader_Dx12;
import :RenderSystem_Dx12;

#if AX_RENDER_DX12

namespace ax {

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
		dst.SemanticName = ax_enum_entry_strlit(VertexSemanticUtil::getType(input.semantic)).c_str();
		dst.SemanticIndex = VertexSemanticUtil::getIndex(input.semantic);
		dst.Format = Dx12Util::getDxDataType(src->dataType);
		dst.InputSlot = 0;
		dst.AlignedByteOffset = src->offset;
		dst.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		dst.InstanceDataStepRate = 0;
	}

	return true;
}

ShaderParamSpace_Dx12::ShaderParamSpace_Dx12(const CreateDesc& desc): Base(desc) {

}

ShaderPass_Dx12::ShaderPass_Dx12(const CreateDesc& desc)
: Base(desc)
{
	using ParamBase    = ShaderParamSpace_Backend::ParamBase;
	using SamplerParam = ShaderParamSpace_Backend::SamplerParam; 
	
//	AX_LOG("--- Shader Pass {} -------", debugName());

	constexpr D3D12_SHADER_VISIBILITY kDefaultShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	auto addRootDescTable = [this](const ShaderParamSpace_Dx12* paramSpace,
	                               const Dx12DescriptorTable&   tbl,
	                               Dx12RootParamType            rootParamType
	) {
		if (tbl.size() <= 0) return;
//		AX_LOG("--- add RootDescTable tableSize={} type={:24} [{}]", tbl.size(), rootParamType, paramSpace->debugName());
		_pipelineRootParamList.addRootDescriptorTable(kDefaultShaderVisibility, tbl);
		_rootParamBindings.emplaceBack(rootParamType, paramSpace->bindSpace());
	};

	// auto addRootStaticSampler = [&](SamplerParam& samplerParam) {
	// 	auto& ss = samplerParam.
	// 	_pipelineRootParamList.addRootStaticSampler(shaderVisibility, samplerParam.bindPoint(), samplerParam.bindCount() );
	// 	_rootParamBindings.emplaceBack(RootStaticSampler, paramSpace->bindSpace());
	// };

	auto addDescriptor = [](const ShaderParamSpace_Dx12* paramSpace, Dx12DescriptorTable& tbl, const ParamBase& p, D3D12_DESCRIPTOR_RANGE_TYPE type) {
//		AX_LOG("--- addDescriptor name={:30} bindPoint={:8} bindCount={:8} type={:8}, [{}] ",
//		       p.name(), p.bindPoint(), p.bindCount(), type, paramSpace->debugName());
		
		if (p.bindCount() <= 0) throw Error_Undefined();
		tbl.addDescriptor(type, p.bindPoint(), p.bindCount(), paramSpace->bindSpace());
	};
	
	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* ownParamSpace = getOwnParamSpace_dx12(bindSpace);
		if (!ownParamSpace) continue;
		
		if (bindSpace != BindSpace::RootConst) {
			for (auto& param : ownParamSpace->_constBufferParams) {
				addDescriptor(ownParamSpace, ownParamSpace->_CBV_SRV_UAV_DescTable, param, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
			}
		}
		
		for (auto& param : ownParamSpace->_structuredBufferParams) {
			addDescriptor(ownParamSpace, ownParamSpace->_CBV_SRV_UAV_DescTable, param, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		}

		for (auto& param : ownParamSpace->_textureParams) {
			addDescriptor(ownParamSpace, ownParamSpace->_CBV_SRV_UAV_DescTable, param, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		}

		for (auto& param : ownParamSpace->_samplerParams) {
			if (param.dynamicSampler()) {
				addDescriptor(ownParamSpace, ownParamSpace->_samplerDescTable, param, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
			} else {
				//addRootStaticSampler();
				AX_ASSERT_TODO();
			}
		}
	}

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace_dx12(bindSpace);
		if (!paramSpace) continue;

		if (bindSpace == BindSpace::RootConst) {
			for (auto& param : paramSpace->_constBufferParams) {
				//	AX_LOG("--- add RootConst bindPoint={} dataSize={} [{}]", param.bindPoint(), param.dataSize(), paramSpace->debugName());
				_rootConstBindPoint      = param.bindPoint();
				_rootConstSizeInBytes    = param.dataSize();
				_rootConstRootParamIndex = _pipelineRootParamList.addRoot32BitConst(kDefaultShaderVisibility,
																					param.bindPoint(), 
																					bindSpace, 
																					param.dataSize());
				_rootParamBindings.emplaceBack(Dx12RootParamType::RootUInt32, bindSpace);
			}
		}
		
		addRootDescTable(paramSpace, paramSpace->_CBV_SRV_UAV_DescTable, Dx12RootParamType::DescTable_CBV_SRV_UAV);
		addRootDescTable(paramSpace, paramSpace->_samplerDescTable     , Dx12RootParamType::DescTable_Sampler);
	}

	auto loadStage = [&](Stage& stage, ShaderStageFlags stageFlags) {
		if (!desc.info->getFuncName(stageFlags)) return;
		auto filename = Fmt("{}/Dx12/Shader_Dx12-{}-{}.bin", shader()->assetPath(), _name, stageFlags);
		File::readBytes(filename, stage.bytecode);
	};

	_visitStages(loadStage);
	_pipelineRootParamList.createRootSignature(_rootSignature);
}

void ShaderPass_Dx12::_createRootSignature(Dx12RootParameterList& rootParamList) {
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
	auto& desc = rootSignatureDesc.Desc_1_0;
	desc = {};

	if (!isComputeShader()) {
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}

#if AX_RENDER_BINDLESS
	desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
	desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
#endif
	
	desc.NumParameters		= Dx12Util::castUINT(rootParamList.parameters.size());
	desc.pParameters		= rootParamList.parameters.data();
	desc.NumStaticSamplers	= Dx12Util::castUINT(rootParamList.staticSamplers.size());
	desc.pStaticSamplers	= rootParamList.staticSamplers.data();

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	auto hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc,
	                                               rootSignatureBlob.ptrForInit(),
	                                               errorBlob.ptrForInit());
	Dx12Util::throwIfError(hr, errorBlob);

	auto* d3dDevice = RenderSystem_Dx12::s_d3dDevice();
	hr = d3dDevice->CreateRootSignature(0, 
										rootSignatureBlob->GetBufferPointer(), 
										rootSignatureBlob->GetBufferSize(), 
										IID_PPV_ARGS(_rootSignature.ptrForInit()));
	Dx12Util::throwIfError(hr);
}

template<class PSO_DESC>
void ShaderPass_Dx12::_commonPsoDesc(RenderRequest_Dx12* req, PSO_DESC& psoDesc) {
	auto& renderState =  _info->renderState;
	{ // rasterizer
		constexpr bool debugWireframe = false;
		psoDesc.RasterizerState.FillMode = debugWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;

		psoDesc.RasterizerState.FrontCounterClockwise = true;
		psoDesc.RasterizerState.CullMode              = Dx12Util::getDxCullMode(renderState.cull);
		psoDesc.DepthStencilState.DepthEnable         = renderState.depthTest.isEnable();
		psoDesc.DepthStencilState.DepthWriteMask      = renderState.depthTest.writeMask
															? D3D12_DEPTH_WRITE_MASK_ALL
															: D3D12_DEPTH_WRITE_MASK_ZERO;
		psoDesc.DepthStencilState.DepthFunc           = Dx12Util::getDxDepthTestOp(renderState.depthTest.op);
		psoDesc.DepthStencilState.StencilEnable       = false;
		psoDesc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthClipEnable       = TRUE;
		
		psoDesc.RasterizerState.MultisampleEnable     = FALSE;
		psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
		psoDesc.RasterizerState.ForcedSampleCount     = 0;
		psoDesc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}	

	// blend
	if constexpr (true) {
		psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
		psoDesc.BlendState.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC rt;
		rt.LogicOpEnable = false;

		rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		if (renderState.blend.isEnable()) {
			rt.BlendEnable	= true;
			rt.BlendOp			= Dx12Util::getDxBlendOp(renderState.blend.rgb.op);
			rt.BlendOpAlpha		= Dx12Util::getDxBlendOp(renderState.blend.alpha.op);
			rt.SrcBlend			= Dx12Util::getDxBlendFactor(renderState.blend.rgb.srcFactor);
			rt.DestBlend		= Dx12Util::getDxBlendFactor(renderState.blend.rgb.dstFactor);
			rt.SrcBlendAlpha	= Dx12Util::getDxBlendFactor(renderState.blend.alpha.srcFactor);
			rt.DestBlendAlpha	= Dx12Util::getDxBlendFactor(renderState.blend.alpha.dstFactor);
		}else{
			rt.BlendEnable	= false;
		}

		psoDesc.BlendState.RenderTarget[0] = rt;

		rt.BlendEnable = false;
		for (UINT i = 1; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			psoDesc.BlendState.RenderTarget[ i ] = rt;
	}	
	
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc.Count = 1;
	
	// render target
	if constexpr (true) {
		auto* pass = req->currentRenderPass();
		auto rtv = Span(psoDesc.RTVFormats);
		Int colorAttachmentCount = pass->colorAttachments().size();
		if (colorAttachmentCount > rtv.size()) {
			throw Error_Undefined("render pass has too many color attachments");
		}

		for (Int i = 0; i < colorAttachmentCount; ++i) {
			auto* ca = pass->colorAttachment(i);
			psoDesc.RTVFormats[i] = Dx12Util::getDxColorType(ca->desc.colorType);
		}

		if (pass->depthAttachment()) {
			psoDesc.DSVFormat = Dx12Util::getDxDepthType(pass->depthAttachment().desc.depthType);
		}
	}
}

auto ShaderPass_Dx12::_getOrAddGraphicsPipeline(RenderRequest_Dx12* req, const PsoKey& psoKey) -> Pipeline* {
	for (auto& pipeline : _pipelineTable) {
		if (pipeline->key == psoKey) {
			return pipeline.ptr();
		}
	}
	
	Pipeline* outPipeline = nullptr;
	if (isMeshShader()) {
		outPipeline = _createMeshShaderPipeline(req, psoKey);
	} else {
		outPipeline = _createVertexShaderPipeline(req, psoKey);
	}
	outPipeline->key = psoKey;

	outPipeline->pipelineState->SetName(TempStringW::s_utf(name().toString()).c_str());
	return outPipeline;
}

auto ShaderPass_Dx12::_createVertexShaderPipeline(RenderRequest_Dx12* req, const PsoKey& psoKey) 
-> Pipeline*
{
	if (_vertexStage.bytecode.size() <= 0) { AX_ASSERT(false);  return nullptr; }
	if ( _pixelStage.bytecode.size() <= 0) { AX_ASSERT(false); return nullptr; }
	
	VertexInputLayoutDesc_Dx12 vertexInputLayoutDesc;
	if (!vertexInputLayoutDesc.init(*_stageInfo, psoKey.vertexLayout)) {
		AX_ASSERT(false);
		return nullptr;
	}	
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout.NumElements = Dx12Util::castUINT(vertexInputLayoutDesc.desc_dx12.size());
	psoDesc.InputLayout.pInputElementDescs = vertexInputLayoutDesc.desc_dx12.data();
	psoDesc.PrimitiveTopologyType = Dx12Util::getDxPrimitiveTopologyType(psoKey.primitiveType);
	//-----
	psoDesc.pRootSignature = _rootSignature;
	psoDesc.VS = Dx12Util::getDxBytecode(_vertexStage.bytecode);
	psoDesc.PS = Dx12Util::getDxBytecode(_pixelStage.bytecode);
	psoDesc.GS = Dx12Util::getDxBytecode(_geometryStage.bytecode);
	//-----
	_commonPsoDesc(req, psoDesc);
	
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	auto& outPipeline = _pipelineTable.emplaceNewObject(AX_NEW);
	auto hr = dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(outPipeline->pipelineState.ptrForInit()));
	Dx12Util::throwIfError(hr);
	
	return outPipeline;
}

auto ShaderPass_Dx12::_createMeshShaderPipeline(RenderRequest_Dx12* req, const PsoKey& posKey) 
-> Pipeline*
{
	if ( _meshStage.bytecode.size() <= 0) { AX_ASSERT(false); return nullptr; }
	if (_pixelStage.bytecode.size() <= 0) { AX_ASSERT(false); return nullptr; }
	
	D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = _rootSignature.ptr();
	psoDesc.PS = Dx12Util::getDxBytecode(_pixelStage.bytecode);
	psoDesc.MS = Dx12Util::getDxBytecode(_meshStage.bytecode);
	psoDesc.AS = Dx12Util::getDxBytecode(_amplificationStage.bytecode); 

	_commonPsoDesc(req, psoDesc);

	auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(psoDesc);
	
	D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
	streamDesc.pPipelineStateSubobjectStream = &psoStream;
	streamDesc.SizeInBytes                   = sizeof(psoStream);

	auto* dev = RenderSystem_Dx12::s_d3dDevice();

	auto& outPipeline = _pipelineTable.emplaceNewObject(AX_NEW);
	auto hr = dev->CreatePipelineState(&streamDesc, IID_PPV_ARGS(outPipeline->pipelineState.ptrForInit()));
	Dx12Util::throwIfError(hr);
	
	return outPipeline;
}

bool ShaderPass_Dx12::bindPipeline(RenderRequest_Dx12* req, AxVertexShaderDraw& draw) const {
	ShaderPass_Dx12::PsoKey psoKey;
	psoKey.vertexLayout  = draw.vertexLayout;
	psoKey.primitiveType = draw.primitiveType;
	return _bindPipeline(req, psoKey);
}

bool ShaderPass_Dx12::bindPipeline(RenderRequest_Dx12* req, AxMeshShaderDraw& draw) const {
	ShaderPass_Dx12::PsoKey psoKey;
	return _bindPipeline(req, psoKey);
}

bool ShaderPass_Dx12::_bindPipeline(RenderRequest_Dx12* req, const PsoKey& psoKey) const {
	auto* pipeline = ax_const_cast(this)->_getOrAddGraphicsPipeline(req, psoKey);
	if (!pipeline) { AX_ASSERT(false); return false; }

	auto& cmdList = req->graphCmdList_dx12();
	cmdList->SetGraphicsRootSignature(ax_const_cast(_rootSignature));
	cmdList->SetPipelineState(pipeline->pipelineState);

	return true;
}

} // namespace

#endif // #if AX_RENDER_DX12
