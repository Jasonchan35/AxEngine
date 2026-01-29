module AxRender;

#if AX_RENDERER_VK

import :Shader_Vk;
import :RenderSystem_Vk;
import :RenderPass_Vk;
import :Vertex;
import :RenderCommandList_Vk;
import :RenderRequest_Vk;
import :RenderContext_Vk;
import :Material_Vk;

namespace ax /*::AxRender*/ {

ShaderPass_Vk::ShaderPass_Vk(const CreateDesc& desc)
: Base(desc)
{
//	AX_LOG("ShaderPass_Vk create {}", debugName());
	
	auto* renderSystem = RenderSystem_Vk::s_instance();
	auto& dev = renderSystem->device();

// create pipeline layout

#if AX_RENDER_BINDLESS
	VkDescriptorSetLayoutCreateFlags layoutFlags  = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	VkDescriptorBindingFlags		 bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
#else
	VkDescriptorSetLayoutCreateFlags layoutFlags  = 0;
	VkDescriptorBindingFlags		 bindingFlags = 0;
#endif

	Array<VkPushConstantRange, 8> pushConsts;
	u32 pushConstOffset = 0;
	
	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		if (bindSpace == BindSpace::RootConst) continue;
		auto* ownParamSpace = getOwnParamSpace_vk(bindSpace);
		if (!ownParamSpace) continue;

		AX_VkDescriptorSetLayoutBindings_<64> 	bindings;
		auto addBinding = [&](const ParamBase& p, VkDescriptorType type) {
//			AX_LOG("-- addBinding name={:26} bindPoint={:6}, bindCount={:6} flags={} type={:16} this={}",
//			           p.name(), p.bindPoint(), p.bindCount(), p.stageFlags(), type, ownParamSpace->debugName());
			bindings.addBinding(type, p.bindPoint(), p.bindCount(), p.stageFlags(), bindingFlags);
		};

		for (auto& param : ownParamSpace->_constBufferParams          ) { addBinding(param, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); }
		for (auto& param : ownParamSpace->_textureParams         ) { addBinding(param, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ); }
		for (auto& param : ownParamSpace->_samplerParams         ) { addBinding(param, VK_DESCRIPTOR_TYPE_SAMPLER       ); }
		for (auto& param : ownParamSpace->_structuredBufferParams) { addBinding(param, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER); }

		if (!ownParamSpace) continue;
//		if (bindings.bindings.size() <= 0) continue;
		
		ownParamSpace->_descSetLayout_vk.create(dev, bindings, layoutFlags);
//		AX_LOG("---- create Layout {} bindings={} {}",
//		       (void*)ownParamSpace->_descSetLayout_vk.handle(),
//		       bindings.bindings.size(), ownParamSpace->debugName());
		_ownDescSetCount++;
	}

	for (auto bindSpace : Range_(BindSpace::_COUNT)) {
		auto* paramSpace = getParamSpace_vk(bindSpace);
		if (!paramSpace) continue;

		if (bindSpace == BindSpace::RootConst) {
			for (auto& param : paramSpace->_constBufferParams) {
				auto& dst = pushConsts.emplaceBack();
				dst.offset = pushConstOffset;
				dst.size = ax_safe_cast_from(param.dataSize());
				dst.stageFlags = VK_SHADER_STAGE_ALL; // AX_VkUtil::getVkShaderStageFlagBits(param.stageFlags());
				pushConstOffset += dst.size;
			}
			continue;
		}
		
		auto* layout = paramSpace->_descSetLayout_vk.handle();
		if (!layout) continue;
		
//		AX_LOG("-- add Layout {} {}", (void*)layout, paramSpace->debugName());
		_allLayouts_vk.emplaceBack(layout);
	}

	auto loadStage = [&](Stage& stage, ShaderStageFlags stageFlags) {
		if (!desc.info->getFuncName(stageFlags)) return;

		auto filename = Fmt("{}/Vk/Shader_Vk-{}-{}.bin", shader()->assetPath(), _name, stageFlags);
		FileMemMap bytecode(filename);
		stage.vkShaderModule.create(dev, bytecode);
	};
	_visitStages(loadStage);
	_pipelineLayout.create(dev, _allLayouts_vk, pushConsts);
}

auto ShaderPass_Vk::getOrAddGraphicsPipeline(class RenderRequest_Vk* req, AxDrawCallDesc& cmd) -> Pipeline* {
	auto* renderPass = req->currentRenderPass_vk();
	if (!renderPass) { AX_ASSERT(false); return nullptr; }
	
	PsoKey psoKey;
	psoKey.vertexLayout  = cmd.vertexLayout;
	psoKey.primitiveType = cmd.primitiveType;
	psoKey.renderPass    = renderPass->_renderPass_vk;

	// TODO pick compatible key.renderPass instead
	// TODO lookup compatible renderPass instead
	
	for (auto& pipeline : _pipelineTable) {
		if (pipeline->key == psoKey) {
			return pipeline.ptr();
		}
	}

	auto& outPipeline = _pipelineTable.emplaceBack(_createGraphicsPipeline(req, cmd, psoKey));
	outPipeline->key = psoKey;
	
	auto shaderName = name().toString();
	req->_device_vk->setObjectDebugName(outPipeline->pipeline.handle(), shaderName);
	
	return outPipeline;
}

auto ShaderPass_Vk::_createGraphicsPipeline(RenderRequest_Vk* req, AxDrawCallDesc& cmd, PsoKey& psoKey)-> UPtr<Pipeline> {
	auto outPipeline = UPtr_new<Pipeline>(AX_NEW);

	auto& dev = RenderSystem_Vk::s_instance()->device();
	auto& renderState = _info->renderState;

//-----
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	pipelineCreateInfo.layout	  = _pipelineLayout;
	pipelineCreateInfo.renderPass = psoKey.renderPass;

//-----
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	pipelineCreateInfo.pDynamicState = &dynamicState;

	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

	VkDynamicState	dynamicStateEnables_[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	auto dynamicStateEnables = Span(dynamicStateEnables_);

	dynamicState.dynamicStateCount	= AX_VkUtil::castUInt32(dynamicStateEnables.size());
	dynamicState.pDynamicStates		= dynamicStateEnables.data();

//-----
	VkPipelineInputAssemblyStateCreateInfo	inputAssemblyState	= {};
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;

	inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyState.topology = AX_VkUtil::getVkPrimitiveTopology(psoKey.primitiveType);

//-----
	VertexInputLayoutDesc_Vk vertexInputLayoutDesc;
	if (!_vertexStage.vkShaderModule && !_meshStage.vkShaderModule) {
		throw Error_Undefined("missing vertex or mesh stage in graphic pipeline");
	}

	if (!vertexInputLayoutDesc.init(*_stageInfo, psoKey.vertexLayout)) {
		AX_ASSERT(false);
		return nullptr;
	}
	pipelineCreateInfo.pVertexInputState = &vertexInputLayoutDesc.vertexInputState;

//-----
	VkPipelineViewportStateCreateInfo viewportState = {};
	pipelineCreateInfo.pViewportState = &viewportState;

	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

	VkViewport	viewport = {};
	VkRect2D	scissorRect = {};

	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissorRect;

	Array<VkPipelineShaderStageCreateInfo, 32>	stageCreateInfos;
	auto addStage = [&](Stage& stage, ShaderStageFlags stageFlags) -> void {
		auto& entryFunc = info()->getFuncName(stageFlags);
		if (!entryFunc) return;

		auto& dst	= stageCreateInfos.emplaceBack();
		dst = {};
		dst.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		dst.stage	= AX_VkUtil::getVkShaderStageFlagBits(stageFlags);
	//	dst.flags	= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		dst.module	= stage.vkShaderModule;
		dst.pName	= entryFunc.c_str();
	};

	_visitStages(addStage);

	pipelineCreateInfo.stageCount	= AX_VkUtil::castUInt32(stageCreateInfos.size());
	pipelineCreateInfo.pStages		= stageCreateInfos.data();

	auto features_base = dev.enabledFeatures().base.features;

#define AX_VkDevice_RequireFeature(feature, state, value) \
	if (!Math::exactlyEqual(state, value) && !feature) { \
		AX_ASSERT_MSG(false, "require: " #feature " for " #state " != " #value); \
		state = value; \
	} \
//-----
	
//-----
	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	pipelineCreateInfo.pMultisampleState	= &multisampleState;
	
	multisampleState.sType	= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleState.alphaToOneEnable = VK_FALSE;
	AX_VkDevice_RequireFeature(features_base.alphaToOne, multisampleState.alphaToOneEnable, VK_FALSE);

//--------
	VkPipelineRasterizationStateCreateInfo	rasterizationState	= {};
	pipelineCreateInfo.pRasterizationState	= &rasterizationState;

	rasterizationState.sType	= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

	rasterizationState.depthClampEnable			= VK_FALSE;
	AX_VkDevice_RequireFeature(features_base.depthClamp, rasterizationState.depthClampEnable, VK_FALSE);

	rasterizationState.rasterizerDiscardEnable	= VK_FALSE;

//	rasterizationState.polygonMode	= key.debugWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterizationState.polygonMode	= VK_POLYGON_MODE_FILL;
	AX_VkDevice_RequireFeature(features_base.fillModeNonSolid, rasterizationState.polygonMode, VK_POLYGON_MODE_FILL);

	rasterizationState.cullMode					= AX_VkUtil::getVkCullMode(renderState.cull);
	rasterizationState.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthBiasEnable			= VK_FALSE;
	rasterizationState.depthBiasConstantFactor	= 0;

	rasterizationState.depthBiasClamp			= 0;
	AX_VkDevice_RequireFeature(features_base.depthBiasClamp, rasterizationState.depthBiasClamp, 0.0f);

	rasterizationState.depthBiasSlopeFactor		= 0;

	rasterizationState.lineWidth				= 1.0f;
	AX_VkDevice_RequireFeature(features_base.wideLines, rasterizationState.lineWidth, 1.0f);

//-----
	VkPipelineColorBlendStateCreateInfo	colorBlendState	= {};
	pipelineCreateInfo.pColorBlendState		= &colorBlendState;

	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	Array<VkPipelineColorBlendAttachmentState, 16> colorBlendAttachmentStates;

	{
		auto& cb = colorBlendAttachmentStates.emplaceBack();
		cb = {};
		if (renderState.blend.isEnable()) {
			cb.blendEnable			= renderState.blend.isEnable();
			cb.colorBlendOp			= AX_VkUtil::getVkBlendOp(renderState.blend.rgb.op);
			cb.alphaBlendOp			= AX_VkUtil::getVkBlendOp(renderState.blend.alpha.op);
			cb.srcColorBlendFactor	= AX_VkUtil::getVkBlendFactor(renderState.blend.rgb.srcFactor);
			cb.dstColorBlendFactor	= AX_VkUtil::getVkBlendFactor(renderState.blend.rgb.dstFactor);
			cb.srcAlphaBlendFactor	= AX_VkUtil::getVkBlendFactor(renderState.blend.alpha.srcFactor);
			cb.dstAlphaBlendFactor	= AX_VkUtil::getVkBlendFactor(renderState.blend.alpha.dstFactor);
		} else {
			cb.blendEnable = false;
		}
		cb.colorWriteMask	= VK_COLOR_COMPONENT_R_BIT
							| VK_COLOR_COMPONENT_G_BIT
							| VK_COLOR_COMPONENT_B_BIT
							| VK_COLOR_COMPONENT_A_BIT;
	}

	colorBlendState.attachmentCount = AX_VkUtil::castUInt32(colorBlendAttachmentStates.size());
	colorBlendState.pAttachments = colorBlendAttachmentStates.data();

//---------
	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;

	depthStencilState.sType	= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	{
		depthStencilState.depthWriteEnable	= renderState.depthTest.writeMask;
		depthStencilState.depthTestEnable	= renderState.depthTest.isEnable();
		depthStencilState.depthCompareOp	= AX_VkUtil::getVkDepthTestOp(renderState.depthTest.op);
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		AX_VkDevice_RequireFeature(features_base.depthBounds, depthStencilState.depthBoundsTestEnable, VK_FALSE);
	}

//----- 
	outPipeline->pipelineCache.create(dev);
	outPipeline->pipeline.create(dev, outPipeline->pipelineCache, pipelineCreateInfo);

	return outPipeline;
}

bool ShaderPass_Vk::_bindPipeline(RenderRequest_Vk* req, AxDrawCallDesc& cmd) const {
	if (!req) { AX_ASSERT(false); return false; }

	auto* pipeline = ax_const_cast(this)->getOrAddGraphicsPipeline(req, cmd);
	if (!pipeline) { AX_ASSERT(false); return false; }
	
	auto& graphCmdList = req->graphCmdList_vk();
	vkCmdBindPipeline(graphCmdList, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
	return true;
}

bool VertexInputLayoutDesc_Vk::init(const ShaderStageInfo& info, VertexLayout vertexLayout) {
	if (!vertexLayout) {
		vertexLayout = Vertex_None::s_layout();
	}

	u32 loc = 0;
	for (auto& input : info.inputs) {
		auto* src = vertexLayout->find(input.semantic);
		if (!src) {
			AX_LOG("VertexLayout semantic '{}' not found", input.semantic);
			AX_ASSERT(false);
			return false; 
		}

		auto& dst = attrDesc.emplaceBack();
		dst = {};
		dst.binding = ax_enum_int(ShaderParamBindPoint::BindVertexBuffer);
		dst.format	= AX_VkUtil::getVkDataType(src->dataType);
		dst.offset  = AX_VkUtil::castUInt32(src->offset);
		dst.location = loc;

		loc++;
	}

	if (attrDesc.size() > 0) {
		// bind vertex buffer
		auto& dst = bindingDesc.emplaceBack();
		dst = {};
		dst.binding		= ax_enum_int(ShaderParamBindPoint::BindVertexBuffer);
		dst.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		dst.stride		= AX_VkUtil::castUInt32(vertexLayout->strideInBytes);
	}

	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.vertexBindingDescriptionCount		= AX_VkUtil::castUInt32(bindingDesc.size());
	vertexInputState.pVertexBindingDescriptions			= bindingDesc.data();
	vertexInputState.vertexAttributeDescriptionCount	= AX_VkUtil::castUInt32(attrDesc.size());
	vertexInputState.pVertexAttributeDescriptions		= attrDesc.data();

	return true;
}

} // namespace
#endif // AX_RENDERER_VK
