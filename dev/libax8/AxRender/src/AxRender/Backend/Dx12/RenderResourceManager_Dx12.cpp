module AxRender;
import :RenderResourceManager_Dx12;
import :Texture_Dx12;

namespace ax {

void RenderResourceManager_Dx12::onUpdateDescriptors(RenderRequest_Backend* req_, Array<SPtr<Sampler_Backend>>& list) {

}

void RenderResourceManager_Dx12::onUpdateDescriptors(RenderRequest_Backend*          req_,
                                                     Array<SPtr<Texture2D_Backend>>& list
) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);

	for (auto& tex_ : list) {
		auto* tex = rttiCastCheck<Texture2D_Dx12>(tex_.ptr());
		if (!tex) throw Error_Undefined();

		Int index = ax_enum_int(tex->resourceHandle.slotId());
		descAlloc_Texture2D.setTexture(index, tex->_bindImage(req));
	}

}


} // namespace ax