module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

void MaterialParamSpace_Dx12::onSetSamplerParam(SamplerParam& param) {
	D3D12_SAMPLER_DESC desc = {};
	auto* sampler = param.sampler();
	if (sampler) {
		auto& ss            = sampler->samplerState();
		desc.Filter         = Dx12Util::getDxSamplerFilter(ss.filter);
		desc.AddressU       = Dx12Util::getDxSamplerWrap(ss.wrap.u);
		desc.AddressV       = Dx12Util::getDxSamplerWrap(ss.wrap.v);
		desc.AddressW       = Dx12Util::getDxSamplerWrap(ss.wrap.w);
		desc.MipLODBias     = 0;
		desc.MaxAnisotropy  = 1;
		desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		desc.MinLOD         = 0;
		desc.MaxLOD         = D3D12_FLOAT32_MAX;
		desc.BorderColor[0] = 0; 
		desc.BorderColor[1] = 0; 
		desc.BorderColor[2] = 0; 
		desc.BorderColor[3] = 0; 
	}
	// _samplerDescHeap.setSampler(param.paramIndex(), desc);
}

void MaterialParamSpace_Dx12::onSetTextureParam(TextureParam& param) {
	switch (param.dataType()) {
		case RenderDataType::Texture2D: {
			// auto* tex = rttiCastCheck<Texture2D_Dx12>(param.texture());
			// _textureDescHeap.setTexture(param.paramIndex(), tex->_texResource);
			return;
		}
		default: AX_ASSERT_TODO; return;
	}
}

void MaterialParamSpace_Dx12::_onDrawcall(RenderRequest_Dx12* req, const ShaderPass_Dx12* shdPass) {
#if 0	
	auto& cmdList = req->_graphCmdBuf_dx12;
	
	auto s = ax_enum_int(bindSpace());

	for (auto& cb : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(cb.getUploadedGpuBuffer(req));
		if (!gpuBuf) throw Error_Undefined();

		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		_constBufferDescHeap.setCBV(cb.paramIndex(), gpuBuf->resource());

		//TODO : move CBV to root parameter ? may help performance ?
		// if (shdPass->isCompute()) {
		// 	cmdList->SetComputeRootConstantBufferView(ax_safe_cast_from(cbIndex), gpuBuf->gpuAddress());
		// } else {
		// 	cmdList->SetGraphicsRootConstantBufferView(ax_safe_cast_from(cbIndex), gpuBuf->gpuAddress());
		// }
	}

	if (_constBufferDescHeap._numDescriptors()) {
		cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdPass->_constBufferDescTableRootIndices[s]),
												_constBufferDescHeap.handleStart().gpu);
	}
	
	if (_textureDescHeap._numDescriptors()) {
		cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdPass->_textureDescTableRootIndices[s]),
												_textureDescHeap.handleStart().gpu);
	}

	if (_samplerDescHeap._numDescriptors()) {
		cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdPass->_samplerDescTableRootIndices[s]),
												_samplerDescHeap.handleStart().gpu);
	}
#endif
}


bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto& cmdList = req->_graphCmdBuf_dx12;

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Dx12>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto bindSpace = ax_enum_int(paramSpace->bindSpace());
		if (bindSpace < 0 || bindSpace >= ax_enum_int(BindSpace::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

		Int CBV_SRV_UAV_index = 0;
		for (auto& cb : paramSpace->constBuffers()) {
			auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(cb.getUploadedGpuBuffer(req));
			if (!gpuBuf) throw Error_Undefined();

			gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			_CBV_SRV_UAV_DescHeap.setCBV(CBV_SRV_UAV_index, gpuBuf->resource());
			CBV_SRV_UAV_index++;
		}

		for (auto& texParam : paramSpace->textureParams()) {
			switch (texParam.dataType()) {
				case RenderDataType::Texture2D: {
					auto* tex = rttiCastCheck<Texture2D_Dx12>(texParam.texture());
					_CBV_SRV_UAV_DescHeap.setTexture(CBV_SRV_UAV_index, tex->_texResource);
					CBV_SRV_UAV_index++;
				} break;
				default: throw Error_Undefined();
			}
		}

		//TODO move to static sampler
		Int samplerIndex = 0;
		for (auto& samplerParam : paramSpace->samplerParams()) {
			auto* sampler = rttiCastCheck<Sampler_Dx12>(samplerParam.sampler());
			auto& ss      = sampler->samplerState();
		
			D3D12_SAMPLER_DESC samplerDesc = {};
			samplerDesc.Filter         = Dx12Util::getDxSamplerFilter(ss.filter);
			samplerDesc.AddressU       = Dx12Util::getDxSamplerWrap(ss.wrap.u);
			samplerDesc.AddressV       = Dx12Util::getDxSamplerWrap(ss.wrap.v);
			samplerDesc.AddressW       = Dx12Util::getDxSamplerWrap(ss.wrap.w);
			samplerDesc.MipLODBias     = 0;
			samplerDesc.MaxAnisotropy  = 1;
			samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			samplerDesc.MinLOD         = 0;
			samplerDesc.MaxLOD         = D3D12_FLOAT32_MAX;
			samplerDesc.BorderColor[0] = 0; 
			samplerDesc.BorderColor[1] = 0; 
			samplerDesc.BorderColor[2] = 0; 
			samplerDesc.BorderColor[3] = 0; 
	
			_samplerDescHeap.setSampler(samplerIndex, samplerDesc);
			samplerIndex++;
		}
	}

	cmdList->SetDescriptorHeaps(ax_safe_cast_from(_d3dDescHeaps.size()), _d3dDescHeaps.data());

	auto setRootDescTable = [&cmdList](auto& shaderTable, auto& heap) {
		if (heap.numDescriptors() > 0) {
			cmdList->SetGraphicsRootDescriptorTable(shaderTable.rootParamIndex, heap.handleStart().gpu);
		}
	};

	setRootDescTable(shdPass->_CBV_SRV_UAV_DescTable,	_CBV_SRV_UAV_DescHeap);
	setRootDescTable(shdPass->_samplerDescTable,		_samplerDescHeap);

	return true;
}

void MaterialPass_Dx12::onSetShader() {
	auto* shaderPass = shaderPass_dx12();

	_CBV_SRV_UAV_DescHeap.create(shaderPass->_CBV_SRV_UAV_DescTable.size());
	     _samplerDescHeap.create(shaderPass->_samplerDescTable.size());

	_d3dDescHeaps.append(_CBV_SRV_UAV_DescHeap.d3dHeap());
	_d3dDescHeaps.append(_samplerDescHeap.d3dHeap());
}

} // namespace

#endif //AX_RENDERER_DX12
