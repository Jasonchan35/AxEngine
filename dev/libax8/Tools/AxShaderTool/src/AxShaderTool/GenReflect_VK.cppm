module;

export module AxShaderTool:GenReflect_VK;
export import AxRender;

#if AX_RENDERER_VK

#include "spirv_reflect.h"

export namespace ax::AxRender {

class GenReflect_VK_EX : public NonCopyable {
public:
	void generate(StrView outFilename, StrView filename, RenderApi api);
//	void _genEntryPoint		(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect) {}
	void _genVertexInputs	(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect);
	void _genBindings		(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect);

	void _genConstBuffer	(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);
	void _genSampler		(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);
	void _genTexture		(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* spirvReflect);


	void _genStorageBuffers	(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* spirvReflect) {}

private:
	template<class PARAM>
	void _genParamBase	(PARAM& dst, ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);

	void _tryGetParamBaseMemberValue(ShaderStageInfo::ParamBase& outParam, StrView sectionName, const JsonValue& json);

	void _throwIfError(SpvReflectResult v) {
		if (v != SPV_REFLECT_RESULT_SUCCESS) throw Error_Undefined();
	}
};

} // namespace

#endif