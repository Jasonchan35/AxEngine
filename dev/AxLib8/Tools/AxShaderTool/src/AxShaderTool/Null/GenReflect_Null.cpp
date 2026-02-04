module;

#if !AX_RENDER_NULL
module AxShaderTool;
#else

module AxShaderTool;
import :GenReflect_Null;

namespace ax /*::AxRender*/ {

void GenReflect_Null::generate(StrView outFilename, StrView filename, RenderAPI api) {
	ShaderStageInfo outInfo;

	auto& opt = CmdOptions::s_instance();
	JsonIO::writeFile(outFilename, outInfo, opt.writeFileOpt);
}

} // namespace


#endif