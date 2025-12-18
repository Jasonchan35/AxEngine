module;

module AxRender;

#if AX_RENDERER_VK
import :Material_Vk;
import :Renderer_Vk;
import :RenderRequest_Vk;
import :Texture_Vk;
import :GpuBuffer_Vk;
import :RenderContext_Vk;

namespace ax /*::AxRender*/ {

MaterialParamSpace_Vk::MaterialParamSpace_Vk(const CreateDesc& desc)
	: Base(desc) {}

VkDescriptorSet MaterialParamSpace_Vk::getUpdatedDescriptorSet(RenderRequest_Vk* req) {
	auto* shaderParamSpace = rttiCastCheck<ShaderParamSpace_Vk>(_shaderParamSpace.ptr());
	if (!shaderParamSpace) return VK_NULL_HANDLE;

	auto seqId = req->renderSeqId();

	if (_lastUpdateRenderSeqId != seqId) {
		_lastUpdateRenderSeqId = seqId;
		_updateDescriptorSet(req, shaderParamSpace);
	}

	return _currentDescriptorSet();
}

void MaterialParamSpace_Vk::_updateDescriptorSet(RenderRequest_Vk* req, const ShaderParamSpace_Vk* shaderParamSpace) {
	_nextDescriptorSet(req, shaderParamSpace);
	auto curSet = _currentDescriptorSet();

//---
	_updateUniformBufferInfos.clear();
	_updateStorageBufferInfos.clear();
	_updateTextureInfos.clear();
	_updateSamplerInfos.clear();
	_updateWriteDescriptorSets.clear();

	auto addWriteDesc = [&](
		VkDescriptorType descType, 
		BindPoint dstBinding, 
		VkDescriptorBufferInfo* bufInfo, 
		VkDescriptorImageInfo* imageInfo
	) {
		auto& w				= _updateWriteDescriptorSets.emplaceBack();
		w.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		w.pNext				= nullptr;
		w.dstSet			= curSet;
		w.dstBinding		= AX_VkUtil::castUInt32(ax_enum_int(dstBinding));
		w.dstArrayElement	= 0;
		w.descriptorType	= descType;
		w.descriptorCount	= 1;
		w.pBufferInfo		= bufInfo;
		w.pImageInfo		= imageInfo;
	};

	for (auto& param : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Vk>(param.getUploadedGpuBuffer(req));
		if (!gpuBuf) throw Error_Undefined("cannot get const buffer");

		auto& dst = _updateUniformBufferInfos.emplaceBack();
		dst.offset  = 0;
		dst.range	= AX_VkUtil::castUInt32(param.dataSize());
		dst.buffer	= gpuBuf->vkBufHandle();
		if (!dst.buffer) throw Error_Undefined();

		addWriteDesc(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, param.bindPoint(), &dst, nullptr);
	}

#if !AX_RENDER_BINDLESS

	for (auto& param : _storageBufferParams) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Vk>(param.gpuBuffer());
		if (!gpuBuf) throw Error_Undefined("cannot get storage buffer");

		auto& dst = _updateStorageBufferInfos.emplaceBack();
		dst.offset  = 0;
		dst.range	= AX_VkUtil::castUInt32(param.dataSize());
		dst.buffer	= gpuBuf->vkBufHandle();
		if (!dst.buffer) throw Error_Undefined();
		
		addWriteDesc(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, param.bindPoint(), &dst, nullptr);
	}

	for (auto& param : _samplerParams) {
		auto* sampler = param.sampler();
		if (!sampler) {
			sampler = StockObjects::s_instance()->samplers.defaultValue.ptr();
		}

		auto* sampler_vk = rttiCastCheck<Sampler_Vk>(sampler);
		if (!sampler_vk) throw Error_Undefined();

		req->resourcesToKeep.add(sampler_vk);

		auto& dst = _updateSamplerInfos.emplaceBack();
		dst.sampler = sampler_vk->vkHandle();
		addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLER, param.bindPoint(), nullptr, &dst);
	}

	for (auto& param : _textureParams) {
		auto* tex = param.texture();
		if (!tex) {
			tex = StockObjects::s_instance()->texture2Ds.kError.ptr();
		}
		
		auto& dst = _updateTextureInfos.emplaceBack();
		switch (tex->type()) {
			case RenderDataType::Texture2D: {
				auto* tex2d = rttiCastCheck<Texture2D_Vk>(tex);
				if (!tex2d) throw Error_Undefined();
				tex2d->_bindImage(req, dst);
			}break;

			default: throw Error_Undefined();
		}

		addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, param.bindPoint(), nullptr, &dst);
	}
#endif

	auto* renderer = req->renderer_vk();
	AX_vkUpdateDescriptorSets(renderer->device(), _updateWriteDescriptorSets, {});
}

void MaterialParamSpace_Vk::_nextDescriptorSet(RenderRequest_Vk* req, const ShaderParamSpace_Vk* shaderParamSpace) {

	auto renderRequestCount = req->renderer()->info().renderRequestCount;

	if (!_descriptorPool) {
		Array<VkDescriptorPoolSize, 64>	poolSizes;

		auto helper = [&](Int descriptorCount, VkDescriptorType descType) -> void {
			if (descriptorCount <= 0) return;

			auto& dst = poolSizes.emplaceBack();
			dst.type = descType;
			dst.descriptorCount = AX_VkUtil::castUInt32(descriptorCount);
		};

		helper(constBuffers_totalBindCount(),			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		helper(textureParams_totalBindCount(),			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		helper(samplerParams_totalBindCount(),			VK_DESCRIPTOR_TYPE_SAMPLER);
		helper(storageBufferParams_totalBindCount(),	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

		_descriptorSets.resize(renderRequestCount);

		VkDescriptorPoolCreateFlags poolFlags = 0;
#if AX_RENDER_BINDLESS
		poolFlags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
#endif

		auto* renderer = req->renderer_vk();
		auto& dev = renderer->device();
		_descriptorPool.create(dev, poolSizes, renderRequestCount, poolFlags);

		for (Int i = 0; i < renderRequestCount; i++) {
			_descriptorSets[i] = _descriptorPool.allocDescriptorSet(shaderParamSpace->_layout_vk);
//#if AX_DEBUG_NAME
//			dev.setObjectDebugName(_descriptorSets[i], Fmt("DescSet[{}]-{}", i, paramSpaceType()));
//#endif
		}
	}

	_currentDescriptorSetsIndex = (_currentDescriptorSetsIndex + 1) % _descriptorSets.size();
}

VkDescriptorSet MaterialParamSpace_Vk::getLastDescriptorSet() {
	Int n = _descriptorSets.size();
	Int last = (_currentDescriptorSetsIndex + n - 1) % n;
	return _descriptorSets[last];
}


bool MaterialPass_Vk::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_vk();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	Array<VkDescriptorSet, 16>	bindDescSets;

	auto* renderer = Renderer_Vk::s_instance();

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Vk>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto paramSpaceType = ax_enum_int(paramSpace->paramSpaceType());
		if (paramSpaceType >= ax_enum_int(SpaceType::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

		auto& dst = bindDescSets.ensureSizeAndGetElement(paramSpaceType);
		dst = paramSpace->getUpdatedDescriptorSet(req);
		if (!dst) { AX_ASSERT(false); return false; }
	}

	auto* commonMaterial = renderer->commonMaterial();
	if (!commonMaterial) { AX_ASSERT(false); return false; }

	auto addCommonBlock = [&](SpaceType paramSpaceType) {
		auto* block = commonMaterial->getPassParamSpace_<MaterialParamSpace_Vk>(0, paramSpaceType);
		if (!block) throw Error_Undefined(Fmt("cannot get commonParamSpace {}", paramSpaceType));

		auto& dst = bindDescSets.ensureSizeAndGetElement(ax_enum_int(paramSpaceType));
		dst = block->getUpdatedDescriptorSet(req);
		if (!dst) throw Error_Undefined("cannot getUpdatedDescriptorSet");
	};

	addCommonBlock(SpaceType::Global   );
	addCommonBlock(SpaceType::PerFrame );
	// TODO: get from object
	addCommonBlock(SpaceType::PerObject);

	if (bindDescSets.size() <= 0) {
		AX_ASSERT(false);
		return false;
	}

	if (bindDescSets.find(VK_NULL_HANDLE)) {
		AX_ASSERT(false); // cannot contains null DescriptorSet
		return false;
	}

	auto& graphCmdBuf = req->graphCmdBuf_vk();
	AX_vkCmdBindDescriptorSets(	graphCmdBuf,
								shdPass->isCompute() ? VK_PIPELINE_BIND_POINT_COMPUTE :  VK_PIPELINE_BIND_POINT_GRAPHICS, 
								shdPass->pipelineLayout(), 
								0, bindDescSets, {});

	return true;
}

MaterialPass_Vk::MaterialPass_Vk(const CreateDesc& desc)
: Base(desc)
{
	auto shaderBlocks = shaderPass()->shaderParamSpaces();

	_materialParamSpaces.resize(shaderBlocks.size());

	for (Int i = 0; i < shaderBlocks.size(); i++) {
		if (auto& src = shaderBlocks[i]) {
			auto p = src->newMaterialParamSpace(AX_ALLOC_REQ);
			_materialParamSpaces[i] = rttiCastCheck<MaterialParamSpace_Backend>(p.ptr());
		}
	}
}


} // namespace ax /*::AxRender*/
#endif // AX_RENDERER_VK