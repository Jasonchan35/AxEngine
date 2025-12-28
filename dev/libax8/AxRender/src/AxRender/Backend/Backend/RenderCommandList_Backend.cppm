module;
export module AxRender:RenderCommandList_Backend;
export import :RenderSystem_Backend;
export import :RenderCommandList;

export namespace ax /*::AxRender*/ {

class RenderCommandList_Backend : public RenderCommandList {
	AX_RTTI_INFO(RenderCommandList_Backend, RenderCommandList)
};

} // namespace

