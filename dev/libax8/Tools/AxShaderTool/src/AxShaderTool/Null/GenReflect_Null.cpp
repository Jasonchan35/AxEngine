module;

#if !AX_RENDERER_NULL
module AxShaderTool;
#else

module AxShaderTool;
import :GenReflect_Null;

namespace ax /*::AxRender*/ {

void GenReflect_Null::generate(StrView outFilename, StrView filename, RenderAPI api) {
	ShaderStageInfo outInfo;
	JsonIO::writeFile(outFilename, outInfo, false, false);
}

} // namespace


#endif