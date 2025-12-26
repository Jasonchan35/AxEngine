module AxRender;
import :RenderResourceManager_Dx12;
import :Texture_Dx12;

namespace ax {

RenderResourceManager_Dx12::RenderResourceManager_Dx12(const CreateDesc& desc): Base(desc) {
	auto& info = RenderSystem_Dx12::s_instance()->info();
	auto* dev = RenderSystem_Dx12::s_d3dDevice();

	descHeapPool_ColorBuffer.create(dev, info.renderPass.maxColorBufferCount * info.renderPass.maxCount * info.renderRequest.count);
	descHeapPool_DepthBuffer.create(dev, info.renderPass.maxDepthBufferCount * info.renderPass.maxCount * info.renderRequest.count);

	Int reqCount = info.renderRequest.count;
	Int renderRequest_CBV_SRV_UAV_Count = info.renderRequest.maxConstBufferCount + info.renderRequest.maxTextureCount;
	Int resource_CBV_SRV_UAV_Count = info.resource.maxTextureCount;
	
	descHeapPool_CBV_SRV_UAV.create(dev, resource_CBV_SRV_UAV_Count    + renderRequest_CBV_SRV_UAV_Count    * reqCount);
	descHeapPool_Sampler.create(dev, info.resource.maxSamplerCount + info.renderRequest.maxSamplerCount * reqCount);

	descChunk_Texture2D.create(descHeapPool_CBV_SRV_UAV, info.resource.maxTextureCount);
	descChunk_Sampler.create(descHeapPool_Sampler    , info.resource.maxSamplerCount);

	descChunk_Texture2D.adjustUsedToSize();
	descChunk_Sampler.adjustUsedToSize();
}

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
		descChunk_Texture2D.setTexture(index, tex->_bindImage(req));
	}

}


} // namespace ax