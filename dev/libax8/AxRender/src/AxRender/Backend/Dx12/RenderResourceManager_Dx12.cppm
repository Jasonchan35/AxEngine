module;

export module AxRender:RenderResourceManager_Dx12;
#if AX_RENDERER_DX12

export import :Dx12Util;
export import :RenderResourceManager_Backend;
export import :Renderer_Backend;

namespace ax {

class RenderResourceManager_Dx12 : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Dx12, RenderResourceManager_Backend)
public:
	RenderResourceManager_Dx12(const CreateDesc& desc) : Base(desc) {}

protected:
};

}

#endif // #if AX_RENDERER_DX12