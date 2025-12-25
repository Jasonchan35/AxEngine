module;

module AxRender;

#if AX_RENDERER_VK
import :Material_Vk;
import :RenderSystem_Vk;
import :RenderRequest_Vk;
import :Texture_Vk;
import :GpuBuffer_Vk;
import :RenderContext_Vk;

namespace ax /*::AxRender*/ {

void MaterialParamSpace_Vk::onUpdatePerFrameData(Int currentIndex, RenderRequest_Backend* req_, PerFrameData& frameData) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);

	req->_writeDescLinearAllocator.reset();
	//-----
	req->_writeDescriptorSets.clear();
	req->_writeDescriptorSets.ensureCapacity(64);
	//-----
	
	auto* mtlPass_vk = rttiCastCheck<MaterialPass_Vk>(_materialPass);
	
	auto& layout = shaderParamSpace_vk()->_descSetLayout_vk;
	frameData._descSet = req->_descriptorPool.allocDescriptorSet(layout);
	
#if AX_RENDER_DEBUG_NAME
	auto& dev = RenderSystem_Vk::s_instance()->device();
	dev.setObjectDebugName(frameData._descSet,
	                       Fmt("{}--DescSet[{}#{}]", mtlPass_vk->name(), bindSpace(), currentIndex));
#endif

	//------- update -----
	auto addWriteDesc = [&](VkDescriptorType        descType,
	                        BindPoint               bindPoint,
	                        VkDescriptorSet&        descSet,
	                        VkDescriptorBufferInfo* bufInfo,
	                        VkDescriptorImageInfo*  imageInfo
	) {
		auto& wds			= req->_writeDescriptorSets.emplaceBack();
		wds.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds.pNext			= nullptr;
		wds.dstSet			= descSet;
		wds.dstBinding		= AX_VkUtil::castUInt32(ax_enum_int(bindPoint));
		wds.dstArrayElement	= 0;
		wds.descriptorType	= descType;
		wds.descriptorCount	= 1;
		wds.pBufferInfo		= bufInfo;
		wds.pImageInfo		= imageInfo;

		if (!wds.dstSet) throw Error_Undefined();
	};

	for (auto& param : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Vk>(param.getUploadedGpuBuffer(req));
		if (!gpuBuf) throw Error_Undefined("cannot get const buffer");
		auto* dst  = req->_getWriteDescBufferInfo();
		dst->offset = 0;
		dst->range  = AX_VkUtil::castUInt32(param.dataSize());
		dst->buffer = gpuBuf->vkBufHandle();
		if (!dst->buffer) throw Error_Undefined();

		addWriteDesc(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, param.bindPoint(), frameData._descSet, dst, nullptr);
	}

#if !AX_RENDER_BINDLESS
	for (auto& param : _samplerParams) {
		auto* sampler = param.sampler();
		if (!sampler) {
			sampler = StockObjects::s_instance()->samplers.defaultValue.ptr();
		}

		auto* sampler_vk = rttiCastCheck<Sampler_Vk>(sampler);
		if (!sampler_vk) throw Error_Undefined();

		req->resourcesToKeep.add(sampler_vk);

		auto* dst   = req->_getWriteDescImageInfo();
		dst->sampler = ax_const_cast(sampler_vk)->vkHandle();
		if (!dst->sampler) throw Error_Undefined();

		addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLER, param.bindPoint(), frameData._descSet, nullptr, dst);
	}

	for (auto& param : _textureParams) {
		auto* tex = param.texture();
		if (!tex) {
			tex = StockObjects::s_instance()->texture2Ds.kError.ptr();
		}

		auto* dst = req->_getWriteDescImageInfo();
		switch (tex->type()) {
			case RenderDataType::Texture2D: {
				auto* tex2d = rttiCastCheck<Texture2D_Vk>(tex);
				if (!tex2d) throw Error_Undefined();
				ax_const_cast(tex2d)->_bindImage(req, *dst);
			}
			break;

			default: throw Error_Undefined();
		}

		addWriteDesc(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, param.bindPoint(), frameData._descSet, nullptr, dst);
	}
	
	for (auto& param : _storageBufferParams) {
		auto* storageBuffer = rttiCastCheck<StorageBuffer_Backend>(param.storageBuffer());
		if (!storageBuffer) throw Error_Undefined("cannot get storage buffer");

		auto* gpuBuffer = rttiCastCheck<GpuBuffer_Vk>(storageBuffer->gpuBuffer());
		if (!gpuBuffer) throw Error_Undefined("cannot get storage gpu buffer");

		auto* dst  = req->_getWriteDescBufferInfo();
		dst->offset = 0;
		dst->range  = AX_VkUtil::castUInt32(param.dataSize());
		dst->buffer = gpuBuffer->vkBufHandle();
		if (!dst->buffer) throw Error_Undefined();

		addWriteDesc(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, param.bindPoint(), frameData._descSet, dst, nullptr);
	}
#endif // #if !AX_RENDER_BINDLESS
	
	AX_vkUpdateDescriptorSets(dev, req->_writeDescriptorSets, {});

}

bool MaterialPass_Vk::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_vk();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

	Array<VkDescriptorSet, BindSpace_COUNT>  allDescSets;

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace_vk(bindSpace);
		if (!paramSpace) continue;

		auto& data = paramSpace->_perFrameDataSet.getUpdated(req);
		allDescSets.emplaceBack(data._descSet);
	}

	AX_vkCmdBindDescriptorSets(req->graphCmdBuf_vk(),
	                           shdPass->isCompute() ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS,
	                           shdPass->pipelineLayout(),
	                           0,
	                           allDescSets,
	                           {});

	return true;
}

MaterialPass_Vk::MaterialPass_Vk(const CreateDesc& desc)
: Base(desc)
{
}


} // namespace ax /*::AxRender*/
#endif // AX_RENDERER_VK