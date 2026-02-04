module AxRender;
import :RenderObjectManager_Dx12;
import :Texture_Dx12;
import :GpuBuffer_Dx12;

namespace ax {

#if AX_RENDER_BINDLESS

template<class T, class T_BASE>
struct RenderObjectManager_Dx12_onUpdateDescriptors {
	static void run(RenderRequest_Backend* req_, Array<T_BASE*>& list) {
		auto* req = rttiCastCheck<RenderRequest_Dx12>(req_);
		for (auto& obj_ : list) {
			auto* obj = rttiCastCheck<T>(obj_);
			if (!obj) continue;
			obj->_getUpdatedDescriptor(req);
		}
	}
};

void RenderObjectManager_Dx12::onUpdateDescriptors(RenderRequest_Backend* req, Array<Sampler*>& list) {
	RenderObjectManager_Dx12_onUpdateDescriptors<Sampler_Dx12, Sampler>::run(req, list);
}

void RenderObjectManager_Dx12::onUpdateDescriptors(RenderRequest_Backend* req, Array<Texture2D*>& list) {
	RenderObjectManager_Dx12_onUpdateDescriptors<Texture2D_Dx12, Texture2D>::run(req, list);
}

#endif // #if AX_RENDER_BINDLESS

void RenderObjectManager_Dx12::onPostCreate() {
	_createDescriptors();

	_meshShaderIndirectDrawArgsBufferPool.create(AX_NEW,
	                                             GpuBufferType::IndirectArgument,
	                                             "pool-meshShaderIndirectDrawArgs",
	                                             100 * Math::MegaBytes,
	                                             4 * Math::MegaBytes);
}

void RenderObjectManager_Dx12::_createDescriptors() {
	auto& info = RenderSystem_Dx12::s_instance()->info();
	auto* dev = RenderSystem_Dx12::s_d3dDevice();
	
	auto& pool = descriptorHeapPools;
	auto& info_pass = info.renderPass;
	auto& info_req  = info.renderRequest;
	
	pool.ColorBuffer.create("ColorBuffer-pool", dev, info_pass.maxColorBufferCount * info_pass.maxCount * info_req.count);
	pool.DepthBuffer.create("DepthBuffer-pool", dev, info_pass.maxDepthBufferCount * info_pass.maxCount * info_req.count);

#if AX_RENDER_BINDLESS
	Int renderReq_CBV_SRV_UAV_Count	= info_req.maxConstBufferCount
									+ info_req.maxTextureCount;

	Int resource_Texture2D_Count	= bindless.AxBindless_Texture2D->bindCount();
	Int resource_Texture3D_Count	= bindless.AxBindless_Texture3D->bindCount();
	Int resource_Sampler_Count      = bindless.AxBindless_SamplerState->bindCount();
	
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