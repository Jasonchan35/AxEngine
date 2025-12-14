module;

export module AxRender:RenderTarget_Dx12;
import :Dx12Util;
export import :RenderTarget_Backend;

#if AX_RENDERER_DX12

import :Renderer_Backend;
import :Shader_Backend;

namespace ax {

class RenderTargetColorBuffer_Dx12 : public RenderTargetColorBuffer_Backend {
	AX_RTTI_INFO(RenderTargetColorBuffer_Dx12, RenderTargetColorBuffer_Backend)
public:
	RenderTargetColorBuffer_Dx12(const CreateDesc& desc) : Base(desc) {}
};

class RenderTargetDepthBuffer_Dx12 : public RenderTargetDepthBuffer_Backend {
	AX_RTTI_INFO(RenderTargetDepthBuffer_Dx12, RenderTargetDepthBuffer_Backend)
public:
	RenderTargetDepthBuffer_Dx12(const CreateDesc& desc) : Base(desc) {}
};


} // namespace

#endif // #if AX_RENDERER_DX12