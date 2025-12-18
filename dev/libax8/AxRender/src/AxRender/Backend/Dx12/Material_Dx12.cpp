module AxRender;
import :Material_Dx12;
import :Texture_Dx12;

#if AX_RENDERER_DX12

namespace ax {


bool MaterialPass_Dx12::onDrawcall(RenderRequest* req_, Cmd_DrawCall& cmd) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	if (!req) { AX_ASSERT(false); return false; }

	auto* shdPass = shaderPass_dx12();
	if (!shdPass) { AX_ASSERT(false); return false; }

	if (!shdPass->_bindPipeline(req, cmd)) return false;
	
	

	return true;
}

} // namespace

#endif //AX_RENDERER_DX12
