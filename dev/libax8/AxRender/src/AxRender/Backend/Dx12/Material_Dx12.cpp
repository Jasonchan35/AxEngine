module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

#if AX_RENDERER_DX12

namespace ax {

auto MaterialParamSpace_Dx12::_updatedPerFrameData(RenderRequest_Dx12*                    req,
                                                   Dx12DescripterHeap_CBV_SRV_UAV::Block& cbvHeapBlock,
                                                   Dx12DescripterHeap_Sampler::Block&     samplerHeapBlock
) -> PerFrameData& {
//	AX_LOG("update {} {} {}", (void*)this, this->_materialPass->shader()->assetPath(), bindSpace());
	_lastRenderSeqId = req->renderSeqId();
	
//--- update ----
	_perFrameData._CBV_SRV_UAV.update(cbvHeapBlock);
	_perFrameData._sampler.update(samplerHeapBlock);
	
	auto* dev = req->_d3dDevice;
	auto& cmdList = req->_graphCmdBuf_dx12;

	for (auto& cb : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(cb.getUploadedGpuBuffer(req)));
		if (!gpuBuf) throw Error_Undefined();

		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		cbvHeapBlock.addCBV(dev, gpuBuf->resource());
		_perFrameData._CBV_SRV_UAV.bindCount++;
	}

#if !AX_RENDER_BINDLESS
	for (auto& texParam : _textureParams) {
		switch (texParam.dataType()) {
			case RenderDataType::Texture2D: {
				auto* tex = rttiCastCheck<Texture2D_Dx12>(texParam.texture());
				if (!tex) throw Error_Undefined();
				cbvHeapBlock.addTexture(dev, ax_const_cast(tex)->_bindImage(req));
				_perFrameData._CBV_SRV_UAV.bindCount++;
			} break;
			default: throw Error_Undefined();
		}
	}

	//TODO move to static sampler
	for (auto& samplerParam : _samplerParams) {
		auto* sampler = rttiCastCheck<Sampler_Dx12>(samplerParam.sampler());
		auto& ss      = sampler->samplerState();
		samplerHeapBlock.addSampler(dev, ss.filter, ss.wrap);
		_perFrameData._sampler.bindCount++;
	}
#endif // #if !AX_RENDER_BINDLESS

	auto* shd = shaderParamSpace_dx12();
	if (shd->_CBV_SRV_UAV_DescTable.size() != _perFrameData._CBV_SRV_UAV.bindCount) throw Error_Undefined();
	if (shd->_samplerDescTable.size()      != _perFrameData._sampler.bindCount    ) throw Error_Undefined();
	
	return _perFrameData;
}

bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	auto* dev = req->_d3dDevice;

	auto& cmdList = req->_graphCmdBuf_dx12;

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	// Bindless: SetGraphicsRootSignature:
	// SetDescriptorHeaps must be called to bind a sampler descriptor before setting a root signature
	// with D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED flag.
	if (!shdPass->_bindPipeline(req, cmd)) return false;

	// alloc block to ensure same heap can hold all descriptors
	Dx12DescripterHeap_CBV_SRV_UAV::Block cbvHeapBlock;
	req->_heap_CBV_SRV_UAV.allocaBlock(cbvHeapBlock, dev,
		  shdPass->allBindCount_constBuffers()
		+ shdPass->allBindCount_textureParams()
		+ shdPass->allBindCount_storageBufferParams());

	Dx12DescripterHeap_Sampler::Block samplerHeapBlock;
	req->_heap_sampler.allocaBlock(samplerHeapBlock, dev, shdPass->allBindCount_samplerParams());
	
	//----- SetDescriptorHeaps ----
	auto descHeaps = Span({cbvHeapBlock.d3dHeap(), samplerHeapBlock.d3dHeap()});
	if (req->_currentDescHeaps != descHeaps) {
		req->_currentDescHeaps = descHeaps;
		cmdList->SetDescriptorHeaps(ax_safe_cast_from(descHeaps.size()), descHeaps.data());
	}

	//---- SetGraphicsRootDescriptorTable ----
	UINT rootParamIndex = 0;
	auto setRootDescTable = [&](const MaterialParamSpace_Dx12::HeapStartHandle& heapStartHandle) {
		if (heapStartHandle.bindCount <= 0) return;
		cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, heapStartHandle.handle.gpu);
		rootParamIndex++;
	};

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace_dx12(bindSpace);
		if (!paramSpace) continue;

		auto& data = paramSpace->getUpdatedPerFrameData(req, cbvHeapBlock, samplerHeapBlock);
		setRootDescTable(data._CBV_SRV_UAV);
		setRootDescTable(data._sampler);
	}

	//----- return Block remain ----
	req->_heap_CBV_SRV_UAV.returnBlockRemain(cbvHeapBlock);
	req->_heap_sampler.returnBlockRemain(samplerHeapBlock);

	if (rootParamIndex != shaderPass_dx12()->_pipelineRootParamList.parameters.size())
		throw Error_Undefined();

	return true;
}

void MaterialPass_Dx12::onSetShader() {
}

} // namespace

#endif //AX_RENDERER_DX12
