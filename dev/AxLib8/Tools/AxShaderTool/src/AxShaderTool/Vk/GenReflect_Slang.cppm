module;

#if AX_RENDERER_VK

export module AxShaderTool:GenReflect_Slang;
export import :GenNinja_Vk;

export namespace ax /*::AxRender*/ {

class GenReflect_Slang : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindSpace = ShaderParamBindSpace;
	
	void generate(StrView outFilename, StrView filename, RenderAPI api);
	void _genVertexInputs	(ShaderStageInfo& outInfo, const JsonValue& json);
	void _genBindings		(ShaderStageInfo& outInfo, const JsonValue& json_root);

	struct SrcParam {
		const JsonObject* parameter   = nullptr;
		const JsonObject* binding     = nullptr;
		const JsonObject* type        = nullptr;
		const JsonObject* elementType = nullptr;
	};

	void _genGlobalParam		(ShaderStageInfo& outInfo, const SrcParam& srcParam);
	void _genConstBuffer		(ShaderStageInfo& outInfo, const SrcParam& srcParam);
	void _genSampler			(ShaderStageInfo& outInfo, const SrcParam& srcParam);
	void _genTexture			(ShaderStageInfo& outInfo, const SrcParam& srcParam, RenderDataType dataType);
	void _genStructuredBuffer	(ShaderStageInfo& outInfo, const SrcParam& srcParam);

private:
	ShaderStageInfo::ConstBuffer _globalConstBuffer;
	
	void _genParamBase (ShaderStageInfo::ParamBase & dstParam, const ShaderStageInfo& info, const SrcParam& srcParam);
	void _genVariables (ShaderStageInfo::BufferBase& dstBuf, const JsonArray& json_fields);
	
	ShaderStageFlags	_getShaderStageFlags(StrView s);
	VertexSemantic		_getShaderSemantic(StrView src);
	RenderDataType		_getRenderDataType(const JsonObject& json_type);
	ShaderVariableType	_getShaderVariableType(const JsonObject& json_type);
};

} // namespace

#else
	export module AxShaderTool:GenReflect_Slang;
#endif