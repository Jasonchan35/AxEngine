module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;
import :RenderResourceManager_Dx12;

#if AX_RENDERER_DX12

namespace ax {

auto MaterialParamSpace_Dx12::_updatedPerFrameData(RenderRequest_Dx12*                    req,
                                                   Dx12DescripterHeapPool_CBV_SRV_UAV::Block& cbvHeapBlock,
                                                   Dx12DescripterHeapPool_Sampler::Block&     samplerHeapBlock
) -> PerFrameData& {
//	AX_LOG("update {} {} {}", (void*)this, this->_materialPass->shader()->assetPath(), bindSpace());
	_lastRenderSeqId = req->renderSeqId();
	
//--- update ----
	_perFrameData.heapStart_CBV_SRV_UAV.update(cbvHeapBlock);
	_perFrameData.heapStart_Sampler.update(samplerHeapBlock);
	
	auto* dev = req->_d3dDevice;
	auto& cmdList = req->_graphCmdBuf_dx12;

	for (auto& cb : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(cb.getUploadedGpuBuffer(req)));
		if (!gpuBuf) throw Error_Undefined();

		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		cbvHeapBlock.addCBV(dev, gpuBuf->resource());
		_perFrameData.heapStart_CBV_SRV_UAV.bindCount++;
	}

#if AX_RENDER_BINDLESS
	
	_perFrameData.heapStart_CBV_SRV_UAV.handle  = req->_bindlessHeap_CBV_SRV_UAV->getHandle(0);
	_perFrameData.heapStart_CBV_SRV_UAV.d3dHeap = req->_bindlessHeap_CBV_SRV_UAV->d3dHeap();

	_perFrameData.heapStart_Sampler.handle  = req->_bindlessHeap_Sampler->getHandle(0);
	_perFrameData.heapStart_Sampler.d3dHeap = req->_bindlessHeap_Sampler->d3dHeap();
	
	return _perFrameData;
	
#else
	for (auto& texParam : _textureParams) {
		switch (texParam.dataType()) {
			case RenderDataType::Texture2D: {
				auto* tex = rttiCastCheck<Texture2D_Dx12>(texParam.texture());
				if (!tex) throw Error_Undefined();
				cbvHeapBlock.addTexture(dev, ax_const_cast(tex)->_bindImage(req));
				_perFrameData.heapStart_CBV_SRV_UAV.bindCount++;
			} break;
			default: throw Error_Undefined();
		}
	}

	//TODO move to static sampler
	for (auto& samplerParam : _samplerParams) {
		auto* sampler = rttiCastCheck<Sampler_Dx12>(samplerParam.sampler());
		auto& ss      = sampler->samplerState();
		samplerHeapBlock.addSampler(dev, ss.filter, ss.wrap);
		_perFrameData.heapStart_Sampler.bindCount++;
	}

	auto* shd = shaderParamSpace_dx12();
	if (shd->_CBV_SRV_UAV_DescTable.size() != _perFrameData.heapStart_CBV_SRV_UAV.bindCount) throw Error_Undefined();
	if (shd->_samplerDescTable.size()      != _perFrameData.heapStart_Sampler.bindCount    ) throw Error_Undefined();
	
	return _perFrameData;
#endif // #if !AX_RENDER_BINDLESS
}

bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	auto* dev = req->_d3dDevice;

	auto& cmdList = req->_graphCmdBuf_dx12;

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	// alloc block to ensure same heap can hold all descriptors
	Dx12DescripterHeapPool_CBV_SRV_UAV::Block cbvHeapBlock;
	req->_heap_CBV_SRV_UAV.allocaBlock(cbvHeapBlock, dev,
		  shdPass->allBindCount_constBuffers()
		+ shdPass->allBindCount_textureParams()
		+ shdPass->allBindCount_storageBufferParams());

	Dx12DescripterHeapPool_Sampler::Block samplerHeapBlock;
	req->_heap_Sampler.allocaBlock(samplerHeapBlock, dev, shdPass->allBindCount_samplerParams());
	
	//----- SetDescriptorHeaps ----
	auto descHeaps = Span({
#if AX_RENDER_BINDLESS
		req->_bindlessHeap_CBV_SRV_UAV->d3dHeap(),
		req->_bindlessHeap_Sampler->d3dHeap(),
#endif
		cbvHeapBlock.d3dHeap(),
		samplerHeapBlock.d3dHeap()
	});
	req->setDescriptorHeaps(descHeaps);

	// SetDescriptorHeaps must be called to bind a sampler descriptor before setting a root signature
	if (!shdPass->_bindPipeline(req, cmd)) return false;

	//---- SetGraphicsRootDescriptorTable ----

	// AX_LOG("--- Material Pass [{}]-------", debugName());

	FixedArray<const MaterialParamSpace_Dx12::PerFrameData*, BindSpace_COUNT> updatedParamSpaceData;
	
	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace_dx12(bindSpace);
		if (!paramSpace) continue;
		auto& data = paramSpace->getUpdatedPerFrameData(req, cbvHeapBlock, samplerHeapBlock);
		updatedParamSpaceData[ax_enum_int(paramSpace->bindSpace())] = &data;
	}
	
	UINT rootParamIndex = 0;
	for (auto& rp : shdPass->_rootParamBindings) {
		// AX_LOG("--- setRootDescriptor bindSpace={:8} rootParamType={}", rp.bindSpace, rp.rootParamType);
		
		switch (rp.rootParamType) {
			case Dx12RootParamType::DescTable_CBV_SRV_UAV: {
				auto* data = updatedParamSpaceData[ax_enum_int(rp.bindSpace)];
				if (!data) throw Error_Undefined();
				cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, data->heapStart_CBV_SRV_UAV.handle.gpu);
			} break;
			case Dx12RootParamType::DescTable_Sampler: {
				auto* data = updatedParamSpaceData[ax_enum_int(rp.bindSpace)];
				if (!data) throw Error_Undefined();
				cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, data->heapStart_Sampler.handle.gpu);
			} break;
		}
		rootParamIndex++;
	}

	//----- return Block remain ----
	req->_heap_CBV_SRV_UAV.returnBlockRemain(cbvHeapBlock);
	req->_heap_Sampler.returnBlockRemain(samplerHeapBlock);

	if (rootParamIndex != shaderPass_dx12()->_pipelineRootParamList.parameters.size())
		throw Error_Undefined();

	return true;
}

void MaterialPass_Dx12::onSetShader() {
}

} // namespace

#endif //AX_RENDERER_DX12
