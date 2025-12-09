module;

#if AX_RENDERER_VK

module AxRender;
import :Material_VK;
import :Renderer_VK;
import :RenderRequest_VK;
import :Texture_VK;
import :GpuBuffer_VK;
import :RenderContext_VK;

namespace ax::AxRender {

MaterialParamSpace_VK::MaterialParamSpace_VK(const CreateDesc& desc)
	: Base(desc) {}

VkDescriptorSet MaterialParamSpace_VK::getUpdatedDescriptorSet(RenderRequest_VK* req) {
	auto* shaderParamSpace = rttiCastCheck<ShaderParamSpace_VK>(_shaderParamSpace.ptr());
	if (!shaderParamSpace) return VK_NULL_HANDLE;

	auto seqId = req->renderSeqId();

	if (_lastUpdateRenderSeqId != seqId) {
		_lastUpdateRenderSeqId = seqId;
		_updateDescriptorSet(req, shaderParamSpace);
	}

	return _currentDescriptorSet();
}

void MaterialParamSpace_VK::_updateDescriptorSet(RenderRequest_VK* req, const ShaderParamSpace_VK* shaderParamSpace) {
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
		auto* gpuBuf = rttiCastCheck<GpuBuffer_VK>(param.getUploadedGpuBuffer(req));
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
		auto* gpuBuf = rttiCastCheck<GpuBuffer_VK>(param.gpuBuffer());
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

		auto* sampler_vk = rttiCastCheck<Sampler_VK>(sampler);
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
			case DataType::Texture2D: {
				auto* tex2d = rttiCastCheck<Texture2D_VK>(tex);
				if (!tex2d) throw Error_Undefined();
				tex2d->_bindImage(req, dst);
			}break;

			default: throw Error_Undefined();
		}

		addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, param.bindPoint(), nullptr, &dst);
	}
#endif

	auto* renderer = req->renderer();
	AX_vkUpdateDescriptorSets(renderer->device(), _updateWriteDescriptorSets, {});
}

void MaterialParamSpace_VK::_nextDescriptorSet(RenderRequest_VK* req, const ShaderParamSpace_VK* shaderParamSpace) {

	auto renderRequestCount = req->renderer()->info().renderRequestCount;

	if (!_descriptorPool) {
		Array<VkDescriptorPoolSize, 64>	poolSizes;

		auto helper = [&](Int descriptorCount, VkDescriptorType descType) -> void {
			if (descriptorCount <= 0) return;

			auto& dst = poolSizes.emplaceBack();
			dst.type = descType;
			dst.descriptorCount = AX_VkUtil::castUInt32(descriptorCount);
		};

		helper(constBuffers().size(),			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		helper(textureParams().size(),			VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		helper(samplerParams().size(),			VK_DESCRIPTOR_TYPE_SAMPLER);
		helper(storageBufferParams().size(),	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

		_descriptorSets.resize(renderRequestCount);

		VkDescriptorPoolCreateFlags poolFlags = 0;
#if AX_RENDER_BINDLESS
		poolFlags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
#endif

		auto* renderer = req->renderer();
		auto& dev = renderer->device();
		_descriptorPool.create(dev, poolSizes, renderRequestCount, poolFlags);

		for (Int i = 0; i < renderRequestCount; i++) {
			_descriptorSets[i] = _descriptorPool.allocDescriptorSet(shaderParamSpace->descipterSetLayout());
//#if AX_DEBUG_NAME
//			dev.setObjectDebugName(_descriptorSets[i], Fmt("DescSet[{}]-{}", i, bindSpace()));
//#endif
		}
	}

	_currentDescriptorSetsIndex = (_currentDescriptorSetsIndex + 1) % _descriptorSets.size();
}

VkDescriptorSet MaterialParamSpace_VK::getLastDescriptorSet() {
	Int n = _descriptorSets.size();
	Int last = (_currentDescriptorSetsIndex + n - 1) % n;
	return _descriptorSets[last];
}


bool MaterialPass_VK::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	if (!shaderPass()) { AX_ASSERT(false); return false; }

	auto* req = rttiCastCheck<RenderRequest_VK>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = rttiCastCheck<ShaderPass_VK>(shaderPass());
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	Array<VkDescriptorSet, 16>	bindDescSets;

	auto* renderer = Renderer_VK::s_instance();

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_VK>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto bindSpace = ax_enum_int(paramSpace->bindSpace());
		if (bindSpace >= ax_enum_int(BindSpace::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

		auto& dst = bindDescSets.ensureSizeAndGet(bindSpace);
		dst = paramSpace->getUpdatedDescriptorSet(req);
		if (!dst) { AX_ASSERT(false); return false; }
	}

	auto* commonMaterial = renderer->commonMaterial();
	if (!commonMaterial) { AX_ASSERT(false); return false; }

	auto addCommonBlock = [&](BindSpace bindSpace) {
		auto* block = commonMaterial->getPassParamSpace_<MaterialParamSpace_VK>(0, bindSpace);
		if (!block) throw Error_Undefined(Fmt("cannot get commonParamSpace {}", bindSpace));

		auto& dst = bindDescSets.ensureSizeAndGet(ax_enum_int(bindSpace));
		dst = block->getUpdatedDescriptorSet(req);
		if (!dst) throw Error_Undefined("cannot getUpdatedDescriptorSet");
	};

	addCommonBlock(BindSpace::Global   );
	addCommonBlock(BindSpace::PerFrame );
	// TODO: get from object
	addCommonBlock(BindSpace::PerObject);

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

MaterialPass_VK::MaterialPass_VK(const CreateDesc& desc)
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


} // namespace ax::AxRender
#endif // AX_RENDERER_VK