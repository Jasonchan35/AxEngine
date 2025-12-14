module AxRender;

#if AX_RENDERER_VK

import :Shader_VK;
import :Renderer_VK;
import :RenderPass_VK;
import :Vertex;
import :CommandBuffer_VK;
import :RenderRequest_VK;
import :RenderContext_VK;
import :Material_VK;

namespace ax::AxRender {


VkDescriptorSetLayout ShaderParamSpace_VK::createDescriptorSetLayout() {
	AX_VkDescriptorSetLayoutBindings	bindings;

#if AX_RENDER_BINDLESS
	VkDescriptorSetLayoutCreateFlags layoutFlags  = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	VkDescriptorBindingFlags		 bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
#else
	VkDescriptorSetLayoutCreateFlags layoutFlags = 0;
	VkDescriptorBindingFlags		 bindingFlags = 0;
#endif

	for (auto& param : _constBuffers) {
		bindings.addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, param.bindPoint(), param.bindCount(), param.stageFlags(), bindingFlags);
	}

	for (auto& param : _textureParams) {
		bindings.addBinding(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, param.bindPoint(), param.bindCount(), param.stageFlags(), bindingFlags);
	}

	for (auto& param : _samplerParams) {
		bindings.addBinding(VK_DESCRIPTOR_TYPE_SAMPLER, param.bindPoint(), param.bindCount(), param.stageFlags(), bindingFlags);
	}

	for (auto& param : _storageBufferParams) {
		bindings.addBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, param.bindPoint(), param.bindCount(), param.stageFlags(), bindingFlags);
	}

	auto* renderer = Renderer_VK::s_instance();
	_descriptorSetLayout.create(renderer->device(), bindings, layoutFlags);

	return _descriptorSetLayout.handle();
}

ShaderPass_VK::ShaderPass_VK(const CreateDesc& desc)
: Base(desc)
{	
	auto* renderer = Renderer_VK::s_instance();
	auto& dev = renderer->device();

// create pipeline layout

	Array<VkDescriptorSetLayout, 8> layouts;

	auto addLayout = [&](const ShaderPass_VK* pass, BindSpace space) {
		auto* block = pass->getParamSpace_<ShaderParamSpace_VK>(space);
		if (!block) return;
		layouts.ensureSizeAndGetElement(ax_enum_int(space)) = block->descriptorSetLayout();
	};

	auto createLayout = [&](ShaderPass_VK* pass, BindSpace space) {
		auto* block = pass->getParamSpace_<ShaderParamSpace_VK>(space);
		if (!block) return;
		layouts.ensureSizeAndGetElement(ax_enum_int(space)) = block->createDescriptorSetLayout();
	};

	if (_shader->isGlobalCommonShader()) {
		for (auto space = BindSpace::Default; space < BindSpace::_COUNT; ++space) {
			createLayout(this, space);
		}

	} else {
	// Default Shader
		createLayout(this, BindSpace::Default);
	//-----
		auto* commonShader = renderer->commonShader();
		if (!commonShader) throw Error_Undefined();

		auto* commonPass = commonShader->getPass_<ShaderPass_VK>(0);
		if (!commonPass) throw Error_Undefined();

		addLayout(commonPass, BindSpace::Global);
		addLayout(commonPass, BindSpace::PerFrame);
		addLayout(commonPass, BindSpace::PerObject);
	}

	auto loadModule = [&](AX_VkShaderModule& outModule, ShaderStageFlags stageFlags) {
		if (!desc.info->getFuncName(stageFlags)) return;

		auto filename = Fmt("{}/VK/VK-{}-{}.bin", shader()->assetPath(), _name, stageFlags);
		FileMemMap bytecode(filename);
		outModule.create(dev, bytecode);
	};

	_visitModules(loadModule);

	_pipelineLayout.create(dev, layouts);
}

ShaderPipeline_VK* ShaderPass_VK::getOrAddPipeline(const Pipeline::Key& key) {
	// TODO pick compatible key.renderPass instead

	for (auto& pipeline : _pipelineTable) {
		if (pipeline->key == key) {
			return pipeline.ptr();
		}
	}

	auto& outPipeline = _pipelineTable.emplaceNew(AX_ALLOC_REQ);
	outPipeline->key = key;

	auto& dev = Renderer_VK::s_instance()->device();
	auto& renderState = _info->renderState;

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
	VertexInputLayoutDesc_VK vertexInputLayoutDesc;
	if (!_vsModule) { AX_ASSERT(false); return nullptr; }

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
	auto addStage = [&](AX_VkShaderModule& mod, ShaderStageFlags stageFlags) -> void {
		auto& entryFunc = info()->getFuncName(stageFlags);
		if (!entryFunc) return;

		auto& dst	= stageCreateInfos.emplaceBack();
		dst.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		dst.stage	= AX_VkUtil::getVkShaderStageFlagBits(stageFlags);
	//	dst.flags	= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		dst.module	= mod;
		dst.pName	= entryFunc.c_str();
	};

	_visitModules(addStage);

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

	rasterizationState.polygonMode	= key.debugWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	AX_VkDevice_RequireFeature(feature_v10.fillModeNonSolid, rasterizationState.polygonMode, VK_POLYGON_MODE_FILL);

	rasterizationState.cullMode					= AX_VkUtil::getVkCullMode(renderState.cull);
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
		auto& blend = renderState.blend;
		auto& cb = colorBlendAttachmentStates.emplaceBack();
		if (blend.isEnable()) {
			cb.blendEnable			= blend.isEnable();
			cb.colorBlendOp			= AX_VkUtil::getVkBlendOp(blend.rgb.op);
			cb.alphaBlendOp			= AX_VkUtil::getVkBlendOp(blend.alpha.op);
			cb.srcColorBlendFactor	= AX_VkUtil::getVkBlendFactor(blend.rgb.srcFactor);
			cb.dstColorBlendFactor	= AX_VkUtil::getVkBlendFactor(blend.rgb.dstFactor);
			cb.srcAlphaBlendFactor	= AX_VkUtil::getVkBlendFactor(blend.alpha.srcFactor);
			cb.dstAlphaBlendFactor	= AX_VkUtil::getVkBlendFactor(blend.alpha.dstFactor);
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
		auto& depthTest = renderState.depthTest;
		depthStencilState.depthWriteEnable	= depthTest.writeMask;
		depthStencilState.depthTestEnable	= depthTest.isEnable();
		depthStencilState.depthCompareOp	= AX_VkUtil::getVkDepthTestOp(depthTest.op);
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		AX_VkDevice_RequireFeature(feature_v10.depthBounds, depthStencilState.depthBoundsTestEnable, VK_FALSE);
	}

//----- 
	outPipeline->pipelineCache.create(dev);
	outPipeline->pipeline.create(dev, outPipeline->pipelineCache, pipelineCreateInfo);

	return outPipeline;
}

bool ShaderPass_VK::_bindPipeline(RenderRequest_VK* req_, Cmd_DrawCall& cmd) const {
	// TODO lookup compatible renderPass instead
	auto* req = rttiCastCheck<RenderRequest_VK>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* renderPass = req->currentRenderPass_vk();
	if (!renderPass) { AX_ASSERT(false); return false; }

	Pipeline::Key key;
	key.vertexLayout	= cmd.vertexLayout;
	key.primitiveType	= cmd.primitiveType;
	key.renderPass		= renderPass->_renderPass;

	auto* pipeline = ax_const_cast(this)->getOrAddPipeline(key);
	if (!pipeline) { AX_ASSERT(false); return false; }

	auto& graphCmdBuf = req->graphCmdBuf_vk();
	
	vkCmdBindPipeline(graphCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
	return true;
}

bool VertexInputLayoutDesc_VK::init(const ShaderStageInfo& info, VertexLayout vertexLayout) {
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

		dst.binding = ax_enum_int(BindPoint::VertexBuffer);
		dst.format	= AX_VkUtil::getVkDataType(src->dataType);
		dst.offset  = AX_VkUtil::castUInt32(src->offset);
		dst.location = loc;

		loc++;
	}

	if (attrDesc.size() > 0) {
		// bind vertex buffer
		auto& dst = bindingDesc.emplaceBack();
		dst.binding		= ax_enum_int(BindPoint::VertexBuffer);
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
