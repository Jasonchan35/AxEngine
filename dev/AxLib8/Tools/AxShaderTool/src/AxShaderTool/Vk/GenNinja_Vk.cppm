module;

#if !AX_RENDER_VK
export module AxShaderTool:GenNinja_Vk;
#else

#include "spirv_reflect.h"

export module AxShaderTool:GenNinja_Vk;
export import AxRender;

export namespace ax /*::AxRender*/ {
class GenNinja_Vk : public NonCopyable {
public:
	void writeNinjaPass(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
};

} // namespace

#endif // #if AX_RENDER_VK
