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

auto MaterialPass_Vk::getUpdatedFrameData(RenderRequest_Vk* req) -> PerFrameData& {
	auto seqId = req->renderSeqId();

	if (_lastUpdateRenderSeqId != seqId) {
		_lastUpdateRenderSeqId = seqId;
		_nextFrameData(req);
	}

	return _currentFrameData();
}

void MaterialPass_Vk::_nextFrameData(RenderRequest_Vk* req) {
	if (!_descPool) {
		_createDescPool();
	}
	
	_currentFrameDataIndex = (_currentFrameDataIndex + 1) % _perFrameData.size();
	auto& curFrameData = _currentFrameData();
	curFrameData.update(this, req);
}

void MaterialPass_Vk::_createDescPool() {
	bool isStaticMaterial = false; // TODO
	auto maxFrameDataCount = isStaticMaterial ? 1 : Renderer::s_instance()->info().renderRequestCount;
	
	Array<VkDescriptorPoolSize, 8>	poolSizes;

	auto addPoolSize = [&](Int descriptorCount, VkDescriptorType descType) -> void {
		if (descriptorCount <= 0) return;
		auto& dst           = poolSizes.emplaceBack();
		dst.type            = descType;
		dst.descriptorCount = AX_VkUtil::castUInt32(descriptorCount * maxFrameDataCount);
	};

	addPoolSize(_shaderPass->constBuffers_totalBindCount(),			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	addPoolSize(_shaderPass->textureParams_totalBindCount(),		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	addPoolSize(_shaderPass->samplerParams_totalBindCount(),		VK_DESCRIPTOR_TYPE_SAMPLER);
	addPoolSize(_shaderPass->storageBufferParams_totalBindCount(),	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

	VkDescriptorPoolCreateFlags poolFlags = 0;
#if AX_RENDER_BINDLESS
	poolFlags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
#endif
	
	auto& dev = Renderer_Vk::s_instance()->device();
	auto& layouts = shaderPass_vk()->_spaceDescSetLayout;
	_descPool.create(dev, poolSizes, layouts.size() * maxFrameDataCount, poolFlags);
}

void MaterialPass_Vk::PerFrameData::create(MaterialPass_Vk* pass, RenderRequest_Vk* req) {
	_created = true;
	if (!pass->_descPool) {
		pass->_createDescPool();
	}
	
	auto& layouts = pass->shaderPass_vk()->_spaceDescSetLayout;
		
	for (Int i = 0; i < BindSpace_COUNT; ++i) {
		if (!layouts[i]) continue;
		_descSets[i] = pass->_descPool.allocDescriptorSet(layouts[i]);

#if AX_RENDER_DEBUG_NAME
		auto& dev = Renderer_Vk::s_instance()->device();
		dev.setObjectDebugName(_descSets[i], Fmt("{}-DescSet[{}][{}]", pass->_name, pass->_currentFrameDataIndex, i));
#endif
	}
}

void MaterialPass_Vk::PerFrameData::update(MaterialPass_Vk* pass, RenderRequest_Vk* req) {
	if (!_created) create(pass, req);

	auto* shaderPass = pass->_shaderPass;
	
	req->_writeDescriptor_BufferInfos.clear();
	req->_writeDescriptor_BufferInfos.ensureCapacity(
		shaderPass->constBuffers_totalBindCount() + shaderPass->storageBufferParams_totalBindCount());
	auto checkCapacity_BufferInfos = ScopedArrayCapacityCheck(req->_writeDescriptor_BufferInfos);

	//------------
	req->_writeDescriptor_ImageInfos.clear();
	req->_writeDescriptor_ImageInfos.ensureCapacity(
		shaderPass->textureParams_totalBindCount() + shaderPass->samplerParams_totalBindCount());
	auto checkCapacity_ImageInfos = ScopedArrayCapacityCheck(req->_writeDescriptor_ImageInfos);

	//------------
	req->_writeDescriptorSets.clear();
	req->_writeDescriptorSets.ensureCapacity(
		req->_writeDescriptor_BufferInfos.capacity() + req->_writeDescriptor_ImageInfos.capacity());
	
	auto addWriteDesc = [&](VkDescriptorType        descType,
	                        BindPoint               bindPoint,
	                        BindSpace               bindSpace,
	                        VkDescriptorBufferInfo* bufInfo,
	                        VkDescriptorImageInfo*  imageInfo
	) {
		auto& wds			= req->_writeDescriptorSets.emplaceBack();
		wds.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds.pNext			= nullptr;
		wds.dstSet			= _descSets[ax_enum_int(bindSpace)];
		wds.dstBinding		= AX_VkUtil::castUInt32(ax_enum_int(bindPoint));
		wds.dstArrayElement	= 0;
		wds.descriptorType	= descType;
		wds.descriptorCount	= 1;
		wds.pBufferInfo		= bufInfo;
		wds.pImageInfo		= imageInfo;

		if (!wds.dstSet) throw Error_Undefined();
	};

	for (Int i = 0; i < BindSpace_COUNT; ++i) {
		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Vk>(pass->_materialParamSpaces[i].ptr());
		if (!paramSpace) continue;
		auto bindSpace = static_cast<BindSpace>(i);
		
		for (auto& param : paramSpace->_constBuffers) {
			auto* gpuBuf = rttiCastCheck<GpuBuffer_Vk>(param.getUploadedGpuBuffer(req));
			if (!gpuBuf) throw Error_Undefined("cannot get const buffer");
			auto& dst  = req->_writeDescriptor_BufferInfos.emplaceBack();
			dst.offset = 0;
			dst.range  = AX_VkUtil::castUInt32(param.dataSize());
			dst.buffer = gpuBuf->vkBufHandle();
			if (!dst.buffer) throw Error_Undefined();

			addWriteDesc(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, param.bindPoint(), bindSpace, &dst, nullptr);
		}

#if !AX_RENDER_BINDLESS
		for (auto& param : paramSpace->_samplerParams) {
			auto* sampler = param.sampler();
			if (!sampler) {
				sampler = StockObjects::s_instance()->samplers.defaultValue.ptr();
			}

			auto* sampler_vk = rttiCastCheck<Sampler_Vk>(sampler);
			if (!sampler_vk) throw Error_Undefined();

			req->resourcesToKeep.add(sampler_vk);

			auto& dst   = req->_writeDescriptor_ImageInfos.emplaceBack();
			dst.sampler = sampler_vk->vkHandle();
			if (!dst.sampler) throw Error_Undefined();

			addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLER, param.bindPoint(), bindSpace, nullptr, &dst);
		}

		for (auto& param : paramSpace->_textureParams) {
			auto* tex = param.texture();
			if (!tex) {
				tex = StockObjects::s_instance()->texture2Ds.kError.ptr();
			}

			auto& dst = req->_writeDescriptor_ImageInfos.emplaceBack();
			switch (tex->type()) {
				case RenderDataType::Texture2D: {
					auto* tex2d = rttiCastCheck<Texture2D_Vk>(tex);
					if (!tex2d) throw Error_Undefined();
					tex2d->_bindImage(req, dst);
				}
				break;

				default: throw Error_Undefined();
			}

			addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, param.bindPoint(), bindSpace, nullptr, &dst);
		}
		
		for (auto& param : paramSpace->_storageBufferParams) {
			auto* storageBuffer = rttiCastCheck<StorageBuffer_Backend>(param.storageBuffer());
			if (!storageBuffer) throw Error_Undefined("cannot get storage buffer");

			auto* gpuBuffer = rttiCastCheck<GpuBuffer_Vk>(storageBuffer->gpuBuffer());
			if (!gpuBuffer) throw Error_Undefined("cannot get storage gpu buffer");

			auto& dst  = req->_writeDescriptor_BufferInfos.emplaceBack();
			dst.offset = 0;
			dst.range  = AX_VkUtil::castUInt32(param.dataSize());
			dst.buffer = gpuBuffer->vkBufHandle();
			if (!dst.buffer) throw Error_Undefined();

			addWriteDesc(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, param.bindPoint(), bindSpace, &dst, nullptr);
		}
#endif // #if !AX_RENDER_BINDLESS
	}
	auto* renderer = req->renderer_vk();
	AX_vkUpdateDescriptorSets(renderer->device(), req->_writeDescriptorSets, {});
}

bool MaterialPass_Vk::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_vk();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Vk>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto bindSpace = ax_enum_int(paramSpace->bindSpace());
		if (bindSpace < 0 || bindSpace >= BindSpace_COUNT) {
			AX_ASSERT(false);
			return false;
		}
	}

	Array<VkDescriptorSet, BindSpace_COUNT> destSet;
	for (auto& s : getUpdatedFrameData(req)._descSets) {
		if (!s) continue; // destSet cannot contains null
		destSet.emplaceBack(s);
	} 

	AX_vkCmdBindDescriptorSets(req->graphCmdBuf_vk(),
	                           shdPass->isCompute() ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS,
	                           shdPass->pipelineLayout(),
	                           0,
	                           destSet,
	                           {});

	return true;
}

MaterialPass_Vk::MaterialPass_Vk(const CreateDesc& desc)
: Base(desc)
{
	_perFrameData.resize(Renderer::s_instance()->info().renderRequestCount);
}


} // namespace ax /*::AxRender*/
#endif // AX_RENDERER_VK