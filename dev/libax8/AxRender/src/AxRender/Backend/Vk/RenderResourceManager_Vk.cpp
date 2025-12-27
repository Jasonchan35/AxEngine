module AxRender;
import :RenderResourceManager_Vk;
import :RenderSystem_Vk;
import :RenderRequest_Vk;
import :Texture_Vk;

namespace ax {

#if AX_RENDER_BINDLESS

template<class T>
struct RenderResourceManager_Vk_onUpdateDescriptors {
	using BindPoint = ShaderParamBindPoint;
	
	using T_Backend = typename T::_TYPE_INFO_Base;

	static void run(RenderRequest_Backend*  req_,
	                Array<SPtr<T_Backend>>& list
	                // VkDescriptorType        descType,
	                // BindPoint               bindPoint,
	                // VkDescriptorSet&        descSet
	) {
		// auto* req = rttiCastCheck<RenderRequest_Vk>(req_);
		//
		// auto writer = req->_writeDescSetHelper.scopeStart();
		//
		// for (auto& obj_ : list) {
		// 	auto* obj = rttiCastCheck<T>(obj_.ptr());
		// 	if (!obj) continue;
		// 	auto info = obj->_getUpdatedDescriptor(req);
		// 	writer.addInfo(info);
		// }
	}
};

void RenderResourceManager_Vk::onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Sampler_Backend>>& list) {
	RenderResourceManager_Vk_onUpdateDescriptors<Sampler_Vk>::run(req, list);
}

void RenderResourceManager_Vk::onUpdateDescriptors(RenderRequest_Backend* req, Array<SPtr<Texture2D_Backend>>& list) {
	RenderResourceManager_Vk_onUpdateDescriptors<Texture2D_Vk>::run(req, list);
}
#endif

void RenderResourceManager_Vk::onPostCreate() {

#if AX_RENDER_BINDLESS
	auto* sys  = RenderSystem_Vk::s_instance();
	auto& dev  = sys->device();

	auto* paramSpace = commonShaderPass()->getParamSpace(BindSpace::Bindless);
	if (!paramSpace) throw Error_Undefined();

	auto* AxBindless_SamplerState = paramSpace->findSamplerParam(AX_NAMEID("AxBindless_SamplerState"));
	if (!AxBindless_SamplerState) throw Error_Undefined();
	
	auto* AxBindless_Texture2D = paramSpace->findTextureParam(AX_NAMEID("AxBindless_Texture2D"));
	if (!AxBindless_Texture2D) throw Error_Undefined();
	
	auto* AxBindless_Texture3D = paramSpace->findTextureParam(AX_NAMEID("AxBindless_Texture3D"));
	if (!AxBindless_Texture3D) throw Error_Undefined();

	Int total_Texture_Count	= AxBindless_Texture2D->bindCount()
							+ AxBindless_Texture3D->bindCount();

	AX_VkDescriptorPool_CreateDesc bindlessPoolDesc;
	bindlessPoolDesc.maxDestSetCount = 1;
	bindlessPoolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER       , AxBindless_SamplerState->bindCount());
	bindlessPoolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE , total_Texture_Count);
	// poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, );
	// poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, );
	bindlessPoolDesc.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	_bindlessDescriptorPool.create(dev, bindlessPoolDesc);
	
#endif
	
}

} // namespace
