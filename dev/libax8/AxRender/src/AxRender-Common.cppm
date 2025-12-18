module;
export module AxRender:Common;
export import AxNativeUI;

export namespace ax {

struct AxRenderConfig {
	static constexpr Int kMaxBackBufferCount = 3;
	static constexpr Int kMaxRenderRequestCount = 8;
	static constexpr bool bindless = static_cast<bool>(AX_RENDER_BINDLESS);
	static constexpr Opt<Milliseconds> kMaxRenderWaitTime = Milliseconds(15 * 1000);
};

} // namespace

export namespace ax /*::AxRender*/ {
	AX_RenderObject_LIST(AX_RenderObject_ForwardDeclare, AX_EMPTY, AX_EMPTY);
}
