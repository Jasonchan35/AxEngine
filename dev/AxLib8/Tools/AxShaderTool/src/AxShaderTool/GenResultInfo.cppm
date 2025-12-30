module;

export module AxShaderTool:GenResultInfo;
export import AxRender;

export namespace ax /*::AxRender*/ {

class GenResultInfo : public NonCopyable {
public:
	using ConstBuffer = ShaderStageInfo::ConstBuffer;
	using Variables   = ShaderStageInfo::Variable;
	
	void run(StrView outFilename, StrView filename, RenderAPI api);

	void mergeStageInfo(ShaderStageInfo& outStageInfo, const ShaderStageInfo& inStageInfo);

private:
	template<class T>
	void _mergeParam(IArray<T>& dstArray, Span<T> srcSpan);

	void _mergeVariables(IArray<Variables>& dstArray, Span<Variables> srcSpan);

	template<class T>
	T* _findParam(MutSpan<T> span, StrView name) {
		auto result = span.find_([&](auto& e){ return e.name == name; });
		return result ? &result->value : nullptr;
	}

	ShaderResultInfo _resultInfo;
};

} // namespace