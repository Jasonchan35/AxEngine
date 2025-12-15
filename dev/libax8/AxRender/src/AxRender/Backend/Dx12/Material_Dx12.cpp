module AxRender;
import :Material_Dx12;
import :Texture_Dx12;

#if AX_RENDERER_DX12

namespace ax {


bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	// AX_ASSERT_TODO;
	// return false;

	return true;
}

} // namespace

#endif //AX_RENDERER_DX12
