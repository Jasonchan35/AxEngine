module AxRender;
import :Material_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;
import :RenderObjectManager_Dx12;

#if AX_RENDERER_DX12
// Descriptors in System
// - Resource Descriptors Per Resource Type (CPU Write / GPU Read)
//      - access by resource slot Id
//      - use for upload, since CPU writing to GPU visible heap, will make the heap slow on GPU side
//
// - RenderTime Descriptors (GPU Only)
//      - One big Chunk per type in whole system, since changing DescriptorHeap is expensive
//            and binding 2 DescriptorHeaps with same type is not allowed in DX12
//            so far only 2 type in DX12 (CBV_SRV_UAV and Sampler)
//      - Static Zone
//            - alloc / free per MaterialParamSpace
//            - allocator to avoid fragmentation
//            - Bindless Descriptors
//      - Dynamic Zone - RenderRequest#0
//            - linear allocator and reset when frame start
//      - Dynamic Zone - RenderRequest#1
//      - Dynamic Zone - RenderRequest#2
//
//======================================================================
//
//          new texture upload (texture::getUploaded())
//                            | write (CPU -> GPU)
// (CPU Write / GPU Read)     v
// +-----------------------------------------------------------------------------+
// |                      Resource Descriptors (Texture)                         |
// +-----------------------------------------------------------------------------+
//                         | copy once when create     | copy in every drawcall
// RenderTime              |   MaterialParamSpace      |  
// Descriptors (GPU Only)  V                           v
// +------------------------------------------------------------------------------------------+
// |    Static Zone                      | Dynamic Zone - RenderRequest#0  | RenderRequest#1  |
// |      alloc/free                     |   Linear allocate               |                  |
// |      per MaterialParamSpace         |   per MaterialParamSpace        |                  |
// +------------------------------------------------------------------------------------------+
//                                       | set to root table in drawcall
//                                       V
//                        +--------------------------------+
//                        | MaterialPass Root Param List   |
//                        +--------------------------------+
namespace ax {

auto MaterialParamSpace_Dx12::_updatedPerFrameData(RenderRequest_Dx12* req) -> PerFrameData& {
//	AX_LOG("MaterialParamSpace_Dx12::_updatedPerFrameData {} ", debugName());
	
//--- update ----
#if AX_RENDER_BINDLESS
	if (_shaderParamSpace->bindSpace() == BindSpace::Bindless) {
		_perFrameData.heapStart_CBV_SRV_UAV.handle = req->_bindlessDescriptors->CBV_SRV_UAV;
		    _perFrameData.heapStart_Sampler.handle = req->_bindlessDescriptors->Sampler;
		return _perFrameData;
	}
#endif
	
	_perFrameData.heapStart_CBV_SRV_UAV.update(req->_dynamicDescriptors.CBV_SRV_UAV);
	_perFrameData.heapStart_Sampler.update(req->_dynamicDescriptors.Sampler);
	
	auto& cmdList = req->_graphCmdList_dx12;
	
	for (auto& param : _constBufferParams) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(param.getUploadedGpuBuffer(req)));
		if (!gpuBuf) throw Error_Undefined();
		//		AX_LOG("-- addCBV");
		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		req->_dynamicDescriptors.CBV_SRV_UAV.addCBV(gpuBuf->resource());
	}
	
	for (auto& param : _structuredBufferParams) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Dx12>(ax_const_cast(param.getUploadedGpuBuffer(req)));
		if (!gpuBuf) throw Error_Undefined();
		// AX_LOG("-- add StructuredBuffer");
		auto* structBuf = param.buffer();
		gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_COMMON);
		req->_dynamicDescriptors.CBV_SRV_UAV.addSRV(gpuBuf->resource(), 0, structBuf->capacity(), structBuf->stride());
	}

#if !AX_RENDER_BINDLESS
	for (auto& texParam : _textureParams) {
		switch (texParam.dataType()) {
			case RenderDataType::Texture2D: {
				auto* tex = texParam.texture();
				if (!tex) tex = req->_fallback->texture2D;
				
				auto* tex_dx12 = rttiCastCheck<Texture2D_Dx12>(tex);
				if (!tex_dx12) throw Error_Undefined();
				
				auto srcDesc = ax_const_cast(tex_dx12)->_getUpdatedDescriptor(req);
				req->_dynamicDescriptors.CBV_SRV_UAV.addDescriptor(srcDesc);
				_perFrameData.heapStart_CBV_SRV_UAV.bindCount++;
			} break;
			default: throw Error_Undefined();
		}
	}

	//TODO move to static sampler
	for (auto& samplerParam : _samplerParams) {
		auto* sampler = samplerParam.sampler();
		if (!sampler) sampler = req->_fallback->sampler;
		
		auto* sampler_dx12 = rttiCastCheck<Sampler_Dx12>(sampler);
		if (!sampler_dx12) throw Error_Undefined();
		
		auto& ss = sampler_dx12->samplerState();
//		AX_LOG("-- addSampler");
		req->_dynamicDescriptors.Sampler.addSampler(ss);
		_perFrameData.heapStart_Sampler.bindCount++;
	}

	auto* shd = shaderParamSpace_dx12();
	if (shd->_CBV_SRV_UAV_DescTable.size() != _perFrameData.heapStart_CBV_SRV_UAV.bindCount) throw Error_Undefined();
	if (shd->_samplerDescTable.size()      != _perFrameData.heapStart_Sampler.bindCount    ) throw Error_Undefined();
#endif
	return _perFrameData;
}

bool MaterialPass_Dx12::onBindMaterial(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	auto& cmdList = req->_graphCmdList_dx12;

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	// SetDescriptorHeaps must be called to bind a sampler descriptor before setting a root signature
	if (!shdPass->_bindPipeline(req, cmd)) return false;

	//---- SetGraphicsRootDescriptorTable ----

	// AX_LOG("--- Material Pass [{}]-------", debugName());

	FixedArray<const MaterialParamSpace_Dx12::PerFrameData*, BindSpace_COUNT> updatedParamSpaceData;
	
	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace_dx12(bindSpace);
		if (!paramSpace) continue;
		auto& data = paramSpace->getUpdatedPerFrameData(req);
		updatedParamSpaceData[ax_enum_int(paramSpace->bindSpace())] = &data;
	}

	auto mvp = req->viewProjMatrix() * cmd.objectToWorld;
	auto rootConstData =  Span(mvp).toByteSpan();

	UINT rootParamIndex = 0;
	for (auto& rp : shdPass->_rootParamBindings) {
		// AX_LOG("--- setRootDescriptor bindSpace={:8} rootParamType={}", rp.bindSpace, rp.rootParamType);
		
		switch (rp.rootParamType) {
			case Dx12RootParamType::RootUInt32: {
				cmdList->SetGraphicsRoot32BitConstants(rootParamIndex,
				                                       ax_safe_cast_from(rootConstData.sizeInBytes() / 4),
				                                       rootConstData.data(),
				                                       0);
			} break;
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
			default: throw Error_Undefined();
		}
		rootParamIndex++;
	}

	//----- return Block remain ----
	// req->_heap_CBV_SRV_UAV.returnBlockRemain(cbvHeapBlock);
	// req->_heap_Sampler.returnBlockRemain(samplerHeapBlock);

	if (rootParamIndex != shaderPass_dx12()->_pipelineRootParamList.parameters.size())
		throw Error_Undefined();

	return true;
}

void MaterialPass_Dx12::onSetShader() {
}

} // namespace

#endif //AX_RENDERER_DX12
