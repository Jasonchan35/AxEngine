module;

#if !AX_RENDER_NULL
export module AxShaderTool:GenReflect_Null;
#else

export module AxShaderTool:GenReflect_Null;
export import :GenNinja_Null;

export namespace ax /*::AxRender*/ {

class GenReflect_Null : public NonCopyable {
public:
	void generate(StrView outFilename, StrView filename, RenderAPI api);
private:
};

} // namespace

#endif