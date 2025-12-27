module AxRender;
import :RenderResourceManager_Dx12;
import :Texture_Dx12;

namespace ax {

RenderResourceManager_Dx12::RenderResourceManager_Dx12(const CreateDesc& desc): Base(desc) {
}

void RenderResourceManager_Dx12::onUpdateDescriptors(RenderRequest_Backend* req_, Array<SPtr<Sampler_Backend>>& list) {
	
}

void RenderResourceManager_Dx12::onUpdateDescriptors(RenderRequest_Backend* req_, Array<SPtr<Texture2D_Backend>>& list) {
	auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
	
	for (auto& tex_ : list) {
		auto* tex = rttiCastCheck<Texture2D_Dx12>(tex_.ptr());
		if (!tex) continue;
		tex->_getUpdatedDescriptor(req);
	}
}

void RenderResourceManager_Dx12::onPostCreate() {
	Base::onPostCreate();
	
	auto& info = RenderSystem_Dx12::s_instance()->info();
	auto* dev = RenderSystem_Dx12::s_d3dDevice();

	auto& pool = descriptorHeapPools;
	auto& info_pass = info.renderPass;
	auto& info_req  = info.renderRequest;
	
	pool.ColorBuffer.create("ColorBuffer-pool", dev, info_pass.maxColorBufferCount * info_pass.maxCount * info_req.count);
	pool.DepthBuffer.create("DepthBuffer-pool", dev, info_pass.maxDepthBufferCount * info_pass.maxCount * info_req.count);

#if AX_RENDER_BINDLESS
	auto* paramSpace = commonShaderPass()->getParamSpace(BindSpace::Bindless);
	if (!paramSpace) throw Error_Undefined();

	auto* AxBindless_SamplerState = paramSpace->findSamplerParam(AX_NAMEID("AxBindless_SamplerState"));
	if (!AxBindless_SamplerState) throw Error_Undefined();
	
	auto* AxBindless_Texture2D = paramSpace->findTextureParam(AX_NAMEID("AxBindless_Texture2D"));
	if (!AxBindless_Texture2D) throw Error_Undefined();
	
	auto* AxBindless_Texture3D = paramSpace->findTextureParam(AX_NAMEID("AxBindless_Texture3D"));
	if (!AxBindless_Texture3D) throw Error_Undefined();

	Int renderReq_CBV_SRV_UAV_Count	= info_req.maxConstBufferCount
									+ info_req.maxTextureCount;

	Int resource_Texture2D_Count	= AxBindless_Texture2D->bindCount()
	Int resource_Texture3D_Count	= AxBindless_Texture3D->bindCount()

	Int resource_Sampler_Count      = AxBindless_SamplerState->bindCount();
	
#else
	Int renderReq_CBV_SRV_UAV_Count	= info_req.maxConstBufferCount
									+ info_req.maxTextureCount;

	Int resource_Texture2D_Count	= 100000;
	Int resource_Texture3D_Count	= 1000;
	Int resource_Sampler_Count      = 1000;
#endif

	Int resource_CBV_SRV_UAV_Count	= resource_Texture2D_Count
									+ resource_Texture3D_Count;
	
	pool.CBV_SRV_UAV.create("CBV_SRV_UAV-pool", dev, resource_CBV_SRV_UAV_Count + renderReq_CBV_SRV_UAV_Count * info_req.count);
	pool.Sampler.create(    "Sampler-pool"    , dev, resource_Sampler_Count     + info_req.maxSamplerCount    * info_req.count);


#if AX_RENDER_BINDLESS
	bindlessDescriptors.CBV_SRV_UAV = pool.CBV_SRV_UAV.currentHandle();
	bindlessDescriptors.Sampler     = pool.Sampler.currentHandle();
#endif
	resourceDescriptors.Sampler.create(  "resource.Sampler"  , pool.Sampler    , resource_Sampler_Count  );
	resourceDescriptors.Texture2D.create("resource.Texture2D", pool.CBV_SRV_UAV, resource_Texture2D_Count);
	resourceDescriptors.Texture3D.create("resource.Texture3D", pool.CBV_SRV_UAV, resource_Texture3D_Count);
}

} // namespace ax