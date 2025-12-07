module;
export module AxRender.PCH;
export import AxNativeUI;

export namespace ax {

struct AxRenderConfig {
	static constexpr Int kMaxBackBufferCount = 3;
	static constexpr Int kMaxRenderRequestCount = 8;
	static constexpr bool bindless = bool(AX_RENDER_BINDLESS);
};

} // namespace
