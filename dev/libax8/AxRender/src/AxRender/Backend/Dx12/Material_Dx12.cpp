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

	//	cmdList->SetDescriptorHeaps(ax_safe_cast_from(_d3dDescHeaps.size()), _d3dDescHeaps.data());
	
	// Bindless: SetGraphicsRootSignature:
	// SetDescriptorHeaps must be called to bind a sampler descriptor before setting a root signature
	// with D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED flag.
	if (!shdPass->_bindPipeline(req, cmd)) return false;

	auto& cbvTable     = shdPass->_CBV_SRV_UAV_DescTable;
	auto& samplerTable = shdPass->_samplerDescTable; 
	
	Array<ID3D12DescriptorHeap*> descHeaps;
	
	auto setRootDescTable = [&](const Dx12DescriptorTable& shaderTable, Dx12DescripterHeap_Base& heap) {
		auto bindCount = shaderTable.totalBindCount();
		auto res = heap.reserveHandles(req->_d3dDevice, bindCount);
		descHeaps.emplaceBack(res.d3dHeap);
		return res.handle;
	};

	auto cbvHandle      = setRootDescTable(cbvTable    , req->_heap_CBV_SRV_UAV);
	auto samplerHandler = setRootDescTable(samplerTable, req->_heap_sampler);

	if (req->_currentDescHeaps != descHeaps) {
		req->_currentDescHeaps = descHeaps;
		cmdList->SetDescriptorHeaps(ax_safe_cast_from(descHeaps.size()), descHeaps.data());
	}

	cmdList->SetGraphicsRootDescriptorTable(cbvTable.rootParamIndex    , cbvHandle.gpu);
	cmdList->SetGraphicsRootDescriptorTable(samplerTable.rootParamIndex, samplerHandler.gpu);
	

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace(bindSpace);
		if (!paramSpace) continue;
		
		for (auto& cb : paramSpace->_constBuffers) {
			auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(cb.getUploadedGpuBuffer(req)));
			if (!gpuBuf) throw Error_Undefined();

			gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			req->addDescCBV(gpuBuf->resource());
		}

#if !AX_RENDER_BINDLESS
		for (auto& texParam : paramSpace->_textureParams) {
			switch (texParam.dataType()) {
				case RenderDataType::Texture2D: {
					auto* tex = rttiCastCheck<Texture2D_Dx12>(texParam.texture());
					if (!tex) throw Error_Undefined();
					req->addDescTexture(ax_const_cast(tex)->_bindImage(req));
				} break;
				default: throw Error_Undefined();
			}
		}

		//TODO move to static sampler
		for (auto& samplerParam : paramSpace->_samplerParams) {
			auto* sampler = rttiCastCheck<Sampler_Dx12>(samplerParam.sampler());
			auto& ss      = sampler->samplerState();
			req->addDescSampler(ss.filter, ss.wrap);
		}
#endif // #if !AX_RENDER_BINDLESS
	}


	
	return true;
}

void MaterialPass_Dx12::onSetShader() {
}

} // namespace

#endif //AX_RENDERER_DX12
