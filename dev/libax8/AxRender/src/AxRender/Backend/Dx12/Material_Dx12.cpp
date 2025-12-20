module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto& cmdList = req->_graphCmdBuf_dx12;

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	Int CBV_SRV_UAV_index = 0;
	Int samplerIndex = 0;
	
	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Dx12>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto bindSpace = ax_enum_int(paramSpace->bindSpace());
		if (bindSpace < 0 || bindSpace >= ax_enum_int(BindSpace::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

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
					tex->_bindImage(req);
					
					_CBV_SRV_UAV_DescHeap.setTexture(CBV_SRV_UAV_index, tex->_texResource);
					CBV_SRV_UAV_index++;
				} break;
				default: throw Error_Undefined();
			}
		}

		//TODO move to static sampler
		for (auto& samplerParam : paramSpace->samplerParams()) {
			auto* sampler = rttiCastCheck<Sampler_Dx12>(samplerParam.sampler());
			auto& ss      = sampler->samplerState();
			_samplerDescHeap.setSampler(samplerIndex, ss.filter, ss.wrap);
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
