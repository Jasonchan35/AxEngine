module;

#if AX_RENDERER_NULL

#include "spirv_reflect.h"

export module AxShaderTool:GenNinja_Null;
export import AxRender;

export namespace ax /*::AxRender*/ {

class GenNinja_Null : public NonCopyable {
public:
	void writeNinjaPass(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
};

} // namespace

#endif // #if AX_RENDERER_NULL
