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

void MaterialParamSpace_Vk::onUpdatePerFrameData(Int                    currentIndex,
                                                 RenderRequest_Backend* req_,
                                                 PerFrameData&          frameData
) {
	auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
	
#if AX_RENDER_BINDLESS
	if (_shaderParamSpace->bindSpace() == BindSpace::Bindless) {
		frameData._descSet = req->_bindlessDescriptorSet;
		return;
	}
#endif

	auto writeDescSetHelper = req->_writeDescSetHelper.scopeStart();

	auto& layout = shaderParamSpace_vk()->_descSetLayout_vk;
	frameData._descSet = req->_descriptorPool.allocDescriptorSet(layout);
	
#if AX_RENDER_DEBUG_NAME
	auto* mtlPass_vk = rttiCastCheck<MaterialPass_Vk>(_materialPass);
	auto& dev = RenderSystem_Vk::s_instance()->device();
	dev.setObjectDebugName(frameData._descSet,
	                       Fmt("{}--DescSet[{}#{}]", mtlPass_vk->name(), bindSpace(), currentIndex));
#endif

	//------- update -----
	for (auto& param : _constBuffers) {
		auto* gpuBuf = rttiCastCheck<GpuBuffer_Vk>(param.getUploadedGpuBuffer(req));
		if (!gpuBuf) throw Error_Undefined("cannot get const buffer");
		auto info = gpuBuf->_getUpdatedDescriptorInfo(req);
		writeDescSetHelper.addInfo(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, param.bindPoint(), frameData._descSet, 0, info);
	}

#if !AX_RENDER_BINDLESS
	for (auto& param : _samplerParams) {
		auto* sampler = param.sampler();
		if (!sampler) {
			sampler = StockObjects::s_instance()->samplers.defaultValue.ptr();
		}

		auto* sampler_vk = rttiCastCheck<Sampler_Vk>(sampler);
		if (!sampler_vk) throw Error_Undefined();

		auto info = sampler_vk->_getUpdatedDescriptorInfo(req);
		writeDescSetHelper.addInfo(VK_DESCRIPTOR_TYPE_SAMPLER, param.bindPoint(), frameData._descSet, 0, info);
	}

	for (auto& param : _textureParams) {
		auto* tex = param.texture();
		if (!tex) {
			tex = StockObjects::s_instance()->texture2Ds.kError.ptr();
		}

		switch (tex->type()) {
			case RenderDataType::Texture2D: {
				auto* tex2d = rttiCastCheck<Texture2D_Vk>(tex);
				if (!tex2d) throw Error_Undefined();
				
				auto info = ax_const_cast(tex2d)->_getUpdatedDescriptorInfo(req);
				writeDescSetHelper.addInfo(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				                           param.bindPoint(),
				                           frameData._descSet,
				                           0,
				                           info);
				break;
			}

			default: throw Error_Undefined();
		}
	}
#endif // #if !AX_RENDER_BINDLESS

	writeDescSetHelper.updateToDevice(dev);
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

	AX_vkCmdBindDescriptorSets(req->graphCmdList_vk(),
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