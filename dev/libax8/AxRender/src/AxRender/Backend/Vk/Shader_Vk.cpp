module AxRender;

#if AX_RENDERER_VK

import :Shader_Vk;
import :Renderer_Vk;
import :RenderPass_Vk;
import :Vertex;
import :CommandBuffer_Vk;
import :RenderRequest_Vk;
import :RenderContext_Vk;
import :Material_Vk;

namespace ax /*::AxRender*/ {


void ShaderParamSpace_Vk::createLayout_vk() {
	AX_VkDescriptorSetLayoutBindings	bindings;

#if AX_RENDER_BINDLESS
	VkDescriptorSetLayoutCreateFlags layoutFlags  = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	VkDescriptorBindingFlags		 bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
#else
	VkDescriptorSetLayoutCreateFlags layoutFlags = 0;
	VkDescriptorBindingFlags		 bindingFlags = 0;
#endif

	auto addBinding = [&bindings, bindingFlags](ParamBase& p, VkDescriptorType type) {
		bindings.addBinding(type, p.bindPoint(), p.bindCount(), p.stageFlags(), bindingFlags);
	};
	for (auto& param : _constBuffers       ) { addBinding(param, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); }
	for (auto& param : _textureParams      ) { addBinding(param, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ); }
	for (auto& param : _samplerParams      ) { addBinding(param, VK_DESCRIPTOR_TYPE_SAMPLER       ); }
	for (auto& param : _storageBufferParams) { addBinding(param, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER); }

	auto* renderer = Renderer_Vk::s_instance();
	_layout_vk.create(renderer->device(), bindings, layoutFlags);
}

ShaderPass_Vk::ShaderPass_Vk(const CreateDesc& desc)
: Base(desc)
{	
	auto* renderer = Renderer_Vk::s_instance();
	auto& dev = renderer->device();

// create pipeline layout

	Array<VkDescriptorSetLayout, 8> layouts;

	const auto* commonPass = rttiCastCheck<ShaderPass_Vk>(getCommonPass());

	for (auto spaceType = ParamSpaceType::Default; spaceType < ParamSpaceType::_COUNT; ++spaceType) {
		if (shouldUseCommonParamSpace(spaceType)) {
			// get from commonPass
			if (auto* sp = commonPass->getParamSpace_<ShaderParamSpace_Vk>(spaceType)) {
				layouts.emplaceBack(sp->_layout_vk);
			}
		} else {
			// create own one
			if (auto* sp = this->getParamSpace_<ShaderParamSpace_Vk>(spaceType)) {
				sp->createLayout_vk();
				layouts.emplaceBack(sp->_layout_vk);
			}
		}
	}

	auto loadStage = [&](Stage& stage, ShaderStageFlags stageFlags) {
		if (!desc.info->getFuncName(stageFlags)) return;

		auto filename = Fmt("{}/Vk/Shader_Vk-{}-{}.bin", shader()->assetPath(), _name, stageFlags);
		FileMemMap bytecode(filename);
		stage.vkShaderModule.create(dev, bytecode);
	};

	_visitStages(loadStage);
	_pipelineLayout.create(dev, layouts);
}

auto ShaderPass_Vk::getOrAddPipeline(const Pipeline::PsoKey& key) -> Pipeline* {
	// TODO pick compatible key.renderPass instead

	for (auto& pipeline : _pipelineTable) {
		if (pipeline->key == key) {
			return pipeline.ptr();
		}
	}

	auto& outPipeline = _pipelineTable.emplaceNewObject(AX_ALLOC_REQ);
	outPipeline->key = key;

	auto& dev = Renderer_Vk::s_instance()->device();
	auto& rs  = _info->renderState;

//-----
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	pipelineCreateInfo.layout	  = _pipelineLayout;
	pipelineCreateInfo.renderPass = key.renderPass;

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
	inputAssemblyState.topology = AX_VkUtil::getVkPrimitiveTopology(key.primitiveType);

//-----
	VertexInputLayoutDesc_Vk vertexInputLayoutDesc;
	if (!_vsStage.vkShaderModule) { AX_ASSERT(false); return nullptr; }

	if (!vertexInputLayoutDesc.init(*_stageInfo, key.vertexLayout)) {
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
		dst.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		dst.stage	= AX_VkUtil::getVkShaderStageFlagBits(stageFlags);
	//	dst.flags	= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		dst.module	= stage.vkShaderModule;
		dst.pName	= entryFunc.c_str();
	};

	_visitStages(addStage);

	pipelineCreateInfo.stageCount	= AX_VkUtil::castUInt32(stageCreateInfos.size());
	pipelineCreateInfo.pStages		= stageCreateInfos.data();

	auto feature_v10 = dev.enabledFeatures().v10.features;

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
	AX_VkDevice_RequireFeature(feature_v10.alphaToOne, multisampleState.alphaToOneEnable, VK_FALSE);

//--------
	VkPipelineRasterizationStateCreateInfo	rasterizationState	= {};
	pipelineCreateInfo.pRasterizationState	= &rasterizationState;

	rasterizationState.sType	= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

	rasterizationState.depthClampEnable			= VK_FALSE;
	AX_VkDevice_RequireFeature(feature_v10.depthClamp, rasterizationState.depthClampEnable, VK_FALSE);

	rasterizationState.rasterizerDiscardEnable	= VK_FALSE;

//	rasterizationState.polygonMode	= key.debugWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterizationState.polygonMode	= VK_POLYGON_MODE_FILL;
	AX_VkDevice_RequireFeature(feature_v10.fillModeNonSolid, rasterizationState.polygonMode, VK_POLYGON_MODE_FILL);

	rasterizationState.cullMode					= AX_VkUtil::getVkCullMode(rs.cull);
	rasterizationState.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthBiasEnable			= VK_FALSE;
	rasterizationState.depthBiasConstantFactor	= 0;

	rasterizationState.depthBiasClamp			= 0;
	AX_VkDevice_RequireFeature(feature_v10.depthBiasClamp, rasterizationState.depthBiasClamp, 0.0f);

	rasterizationState.depthBiasSlopeFactor		= 0;

	rasterizationState.lineWidth				= 1.0f;
	AX_VkDevice_RequireFeature(feature_v10.wideLines, rasterizationState.lineWidth, 1.0f);

//-----
	VkPipelineColorBlendStateCreateInfo	colorBlendState	= {};
	pipelineCreateInfo.pColorBlendState		= &colorBlendState;

	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	Array<VkPipelineColorBlendAttachmentState, 16> colorBlendAttachmentStates;

	{
		auto& cb = colorBlendAttachmentStates.emplaceBack();
		if (rs.blend.isEnable()) {
			cb.blendEnable			= rs.blend.isEnable();
			cb.colorBlendOp			= AX_VkUtil::getVkBlendOp(rs.blend.rgb.op);
			cb.alphaBlendOp			= AX_VkUtil::getVkBlendOp(rs.blend.alpha.op);
			cb.srcColorBlendFactor	= AX_VkUtil::getVkBlendFactor(rs.blend.rgb.srcFactor);
			cb.dstColorBlendFactor	= AX_VkUtil::getVkBlendFactor(rs.blend.rgb.dstFactor);
			cb.srcAlphaBlendFactor	= AX_VkUtil::getVkBlendFactor(rs.blend.alpha.srcFactor);
			cb.dstAlphaBlendFactor	= AX_VkUtil::getVkBlendFactor(rs.blend.alpha.dstFactor);
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
		depthStencilState.depthWriteEnable	= rs.depthTest.writeMask;
		depthStencilState.depthTestEnable	= rs.depthTest.isEnable();
		depthStencilState.depthCompareOp	= AX_VkUtil::getVkDepthTestOp(rs.depthTest.op);
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		AX_VkDevice_RequireFeature(feature_v10.depthBounds, depthStencilState.depthBoundsTestEnable, VK_FALSE);
	}

//----- 
	outPipeline->pipelineCache.create(dev);
	outPipeline->pipeline.create(dev, outPipeline->pipelineCache, pipelineCreateInfo);

	return outPipeline;
}

bool ShaderPass_Vk::_bindPipeline(RenderRequest_Vk* req, Cmd_DrawCall& cmd) const {
	// TODO lookup compatible renderPass instead
	if (!req) { AX_ASSERT(false); return false; }
	
	auto* renderPass = req->currentRenderPass_vk();
	if (!renderPass) { AX_ASSERT(false); return false; }

	Pipeline::PsoKey psoKey;
	psoKey.vertexLayout  = cmd.vertexLayout;
	psoKey.primitiveType = cmd.primitiveType;
	psoKey.renderPass    = renderPass->_renderPass_vk;

	auto* pipeline = ax_const_cast(this)->getOrAddPipeline(psoKey);
	if (!pipeline) { AX_ASSERT(false); return false; }

	auto& graphCmdBuf = req->graphCmdBuf_vk();
	
	vkCmdBindPipeline(graphCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
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

		dst.binding = ax_enum_int(ShaderResourceBindPoint::VertexBuffer);
		dst.format	= AX_VkUtil::getVkDataType(src->dataType);
		dst.offset  = AX_VkUtil::castUInt32(src->offset);
		dst.location = loc;

		loc++;
	}

	if (attrDesc.size() > 0) {
		// bind vertex buffer
		auto& dst = bindingDesc.emplaceBack();
		dst.binding		= ax_enum_int(ShaderResourceBindPoint::VertexBuffer);
		dst.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		dst.stride		= AX_VkUtil::castUInt32(vertexLayout->stride);
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
