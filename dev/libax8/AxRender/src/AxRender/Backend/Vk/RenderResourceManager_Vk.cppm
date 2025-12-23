module;

export module AxRender:RenderResourceManager_Vk;
#if AX_RENDERER_VK
export import :RenderResourceManager_Backend;
export import :Renderer_Backend;

namespace ax {

class RenderResourceManager_Vk : public RenderResourceManager_Backend {
	AX_RTTI_INFO(RenderResourceManager_Vk, RenderResourceManager_Backend)
public:
	RenderResourceManager_Vk(const CreateDesc& desc) : Base(desc) {}
};

}

#endif // #if AX_RENDERER_DX12