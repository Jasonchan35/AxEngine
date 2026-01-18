module;

#if !AX_RENDERER_VK
export module AxShaderTool:GenReflect_Vk;
#else

#include "spirv_reflect.h"

export module AxShaderTool:GenReflect_Vk;
export import :GenNinja_Vk;

export namespace ax /*::AxRender*/ {

class GenReflect_Vk : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindSpace = ShaderParamBindSpace;
	
	void generate(StrView outFilename, StrView filename, RenderAPI api);
//	void _genEntryPoint		(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect) {}
	void _genVertexInputs	(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect);
	void _genBindings		(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect);
	void _genPushConstants	(ShaderStageInfo& outInfo, const spv_reflect::ShaderModule& spirvReflect);

	void _genConstBuffer	(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);
	void _genSampler		(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);
	void _genTexture		(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);
	void _genStorageBuffer	(ShaderStageInfo& outInfo, const SpvReflectDescriptorBinding* binding);

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