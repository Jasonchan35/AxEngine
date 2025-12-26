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
	
	descHeapPool_CBV_SRV_UAV.create(dev,renderRequest_CBV_SRV_UAV_Count    * reqCount);
	    descHeapPool_Sampler.create(dev,info.renderRequest.maxSamplerCount * reqCount);

	resourceDesc.Texture2D.create(dev, info.resource.maxTextureCount);
	  resourceDesc.Sampler.create(dev, info.resource.maxSamplerCount);
}

} // namespace ax