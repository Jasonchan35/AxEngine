module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

MaterialParamSpace_Dx12::MaterialParamSpace_Dx12(const CreateDesc& desc): Base(desc) {
	auto* shaderParamSpace = rttiCastCheck<ShaderParamSpace_Dx12>(desc.shaderParamSpace);

//	_samplerDescHeap.create(shaderParamSpace->_samplerDescTable.size());
	_textureDescHeap.create(shaderParamSpace->_textureDescTable.size());
	_storageBufferDescHeap.create(shaderParamSpace->_storageBufferDescTable.size());
}

bool MaterialParamSpace_Dx12::onSetParam(SamplerParam& param, Int index, Sampler* sampler) {
	D3D12_SAMPLER_DESC desc = {};
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
	_samplerDescHeap.setSampler(index, desc);
	return true;
}

void MaterialParamSpace_Dx12::_onDrawcall(RenderRequest_Dx12* req, const ShaderPass_Dx12* shdPass) {
	auto& cmdList = req->_graphCmdBuf_dx12;
	
	Int cbIndex = 0;
	for (auto& cb : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(cb.getUploadedGpuBuffer(req));
		if (!gpuBuf) throw Error_Undefined();

		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		if (shdPass->isCompute()) {
			cmdList->SetComputeRootConstantBufferView(ax_safe_cast_from(cbIndex), gpuBuf->gpuAddress());
		} else {
			cmdList->SetGraphicsRootConstantBufferView(ax_safe_cast_from(cbIndex), gpuBuf->gpuAddress());
		}
		cbIndex++;
	}

	auto s = ax_enum_int(spaceType());

	cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdPass->_descTableRootIndices[s]),
	                                        _textureDescHeap.handleStart().gpu);
	
	cmdList->SetGraphicsRootDescriptorTable(ax_safe_cast_from(shdPass->_samplerDescTableRootIndices[s]),
	                                        _samplerDescHeap.handleStart().gpu);
}


bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	auto* renderer = Renderer_Backend::s_instance();

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Dx12>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto spaceType = ax_enum_int(paramSpace->spaceType());
		if (spaceType >= ax_enum_int(SpaceType::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

		paramSpace->_onDrawcall(req, shdPass);
	}

	auto* commonMaterial = renderer->commonMaterial();
	if (!commonMaterial) { AX_ASSERT(false); return false; }

	return true;
}

} // namespace

#endif //AX_RENDERER_DX12
