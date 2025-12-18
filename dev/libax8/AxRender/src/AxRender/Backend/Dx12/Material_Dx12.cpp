module AxRender;
import :Material_Dx12;
import :Texture_Dx12;

#if AX_RENDERER_DX12

namespace ax {


bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;

#if 0	
//	Array<VkDescriptorSet, 16>	bindDescSets;
	auto* renderer = Renderer_Backend::s_instance();

	for (auto& paramSpace_ : _materialParamSpaces) {
		if (!paramSpace_) continue;

		auto* paramSpace = rttiCastCheck<MaterialParamSpace_Dx12>(paramSpace_.ptr());
		if (!paramSpace) { AX_ASSERT(false); return false; }

		auto spaceType = ax_enum_int(paramSpace->paramSpaceType());
		if (spaceType >= ax_enum_int(ParamSpaceType::_COUNT)) {
			AX_ASSERT(false);
			return false;
		}

		auto& dst = bindDescSets.ensureSizeAndGetElement(spaceType);
		dst = paramSpace->getUpdatedDescriptorSet(req);
		if (!dst) { AX_ASSERT(false); return false; }
	}

	auto* commonMaterial = renderer->commonMaterial();
	if (!commonMaterial) { AX_ASSERT(false); return false; }

	auto addCommonBlock = [&](ParamSpaceType paramSpaceType) {
		auto* block = commonMaterial->getPassParamSpace_<MaterialParamSpace_Vk>(0, paramSpaceType);
		if (!block) throw Error_Undefined(Fmt("cannot get commonParamSpace {}", paramSpaceType));

		auto& dst = bindDescSets.ensureSizeAndGetElement(ax_enum_int(paramSpaceType));
		dst = block->getUpdatedDescriptorSet(req);
		if (!dst) throw Error_Undefined("cannot getUpdatedDescriptorSet");
	};

	addCommonBlock(ParamSpaceType::Global   );
	addCommonBlock(ParamSpaceType::PerFrame );
	// TODO: get from object
	addCommonBlock(ParamSpaceType::PerObject);

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
#endif
	return true;
}

} // namespace

#endif //AX_RENDERER_DX12
