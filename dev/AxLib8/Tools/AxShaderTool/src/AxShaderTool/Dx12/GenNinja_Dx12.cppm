module;

#if AX_RENDERER_DX12

#include "spirv_reflect.h"

export module AxShaderTool:GenNinja_Dx12;
export import AxRender;

export namespace ax /*::AxRender*/ {

class GenNinja_Dx12 : public NonCopyable {
public:
	void writeNinjaPass(IString& outStr, IArray<String> & outJsonFileList, ShaderPassInfo& pass, StrView relSourceFilename);
};

} // namespace

#endif // #if AX_RENDERER_Dx12
