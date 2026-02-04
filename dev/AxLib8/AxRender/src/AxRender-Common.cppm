module;
export module AxRender:Common;
export import AxNativeUI;

export namespace ax {

#include "AxRender/HLSL/AX_HLSL_CPU_STRUCT.h"

struct AxRenderConfig {
	static constexpr Int kMaxBackBufferCount    = 3;
	static constexpr Int kMaxRenderRequestCount = 4;

	static constexpr bool bindless = static_cast<bool>(AX_RENDER_BINDLESS);
	static constexpr Opt<Milliseconds> kMaxRenderWaitTime() { return std::nullopt; /* Seconds(60); */ }
};

} // namespace

export namespace ax /*::AxRender*/ {
	AX_RenderObject_LIST(AX_RenderObject_ForwardDeclare, AX_EMPTY, AX_EMPTY);
	AX_RenderObject_BackendOnly_LIST(AX_RenderObject_ForwardDeclare, AX_EMPTY, AX_EMPTY);
}
