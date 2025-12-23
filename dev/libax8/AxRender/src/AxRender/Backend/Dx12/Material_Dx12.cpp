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

	Int CBV_SRV_UAV_index = 0;
	
#if !AX_RENDER_BINDLESS
	Int samplerIndex = 0;
#endif

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace(bindSpace);
		if (!paramSpace) continue;
		
		for (auto& cb : paramSpace->_constBuffers) {
			auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(cb.getUploadedGpuBuffer(req)));
			if (!gpuBuf) throw Error_Undefined();

			gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			_CBV_SRV_UAV_DescHeap.setCBV(CBV_SRV_UAV_index, gpuBuf->resource());
			++CBV_SRV_UAV_index;
		}

#if !AX_RENDER_BINDLESS
		for (auto& texParam : paramSpace->_textureParams) {
			switch (texParam.dataType()) {
				case RenderDataType::Texture2D: {
					auto* tex = rttiCastCheck<Texture2D_Dx12>(texParam.texture());
					if (!tex) throw Error_Undefined();
					_CBV_SRV_UAV_DescHeap.setTexture(CBV_SRV_UAV_index, ax_const_cast(tex)->_bindImage(req));
					++CBV_SRV_UAV_index;
				} break;
				default: throw Error_Undefined();
			}
		}

		//TODO move to static sampler
		for (auto& samplerParam : paramSpace->_samplerParams) {
			auto* sampler = rttiCastCheck<Sampler_Dx12>(samplerParam.sampler());
			auto& ss      = sampler->samplerState();
			_samplerDescHeap.setSampler(samplerIndex, ss.filter, ss.wrap);
			++samplerIndex;
		}
#endif // #if !AX_RENDER_BINDLESS
	}

	cmdList->SetDescriptorHeaps(ax_safe_cast_from(_d3dDescHeaps.size()), _d3dDescHeaps.data());
	
	// Bindless: SetGraphicsRootSignature:
	// SetDescriptorHeaps must be called to bind a sampler descriptor before setting a root signature
	// with D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED flag.
	if (!shdPass->_bindPipeline(req, cmd)) return false;

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
