module;

export module AxShaderTool:GenResultInfo;
export import AxRender;

export namespace ax /*::AxRender*/ {

class GenResultInfo : public NonCopyable {
public:
	void run(StrView outFilename, StrView filename, RenderAPI api);

	void mergeStageInfo(ShaderStageInfo& outStageInfo, const ShaderStageInfo& inStageInfo);

private:
	template<class T, class FUNC>
	void _mergeParam(IArray<T>& dstArray, Span<T> srcSpan, FUNC func);

	template<class T>
	T* _findParam(MutSpan<T> span, StrView name) {
		auto result = span.find_([&](auto& e){ return e.name == name; });
		return result ? &result->value : nullptr;
	}

	ShaderResultInfo _resultInfo;
};

} // namespace