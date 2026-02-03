module AxRender;
import :RenderObjectManager_Vk;
import :RenderSystem_Vk;
import :RenderRequest_Vk;
import :Texture_Vk;
import :Material_Vk;

namespace ax {

#if AX_RENDER_BINDLESS

template<class T>
struct RenderObjectManager_Vk_onUpdateDescriptors {
	using T_Backend = typename T::_TYPE_INFO_Base;

	using BindPoint = ShaderParamBindPoint;

	static void run(RenderObjectManager_Vk* mgr,
	                RenderRequest_Backend*  req_,
	                Array<T_Backend*>&      list,
	                VkDescriptorType        descType,
	                BindPoint               bindPoint
	) {
		auto* req       = rttiCastCheck<RenderRequest_Vk>(req_);
		auto  helper    = req->_writeDescSetHelper.scopeStart();
		auto  destSet   = mgr->_bindlessDescriptorSet;

//		AX_LOG("onUpdateDescriptors descType={} size={} -------", descType, list.size());
		
		for (auto& obj_ : list) {
			auto* obj = rttiCastCheck<T>(obj_);
			if (!obj) continue;
			auto slotId = obj->objectSlot.slotId();
			auto info   = obj->_getUpdatedDescriptorInfo(req);
			helper.addInfo(descType, bindPoint, destSet, slotId, info);
			
//			AX_LOG("- updateDescriptor descType={:25} slotId={:4} debugName=[{}]", descType, slotId, obj->debugName());
		}

		helper.updateToDevice(req->_device_vk->handle());
	}
};

void RenderObjectManager_Vk::onUpdateDescriptors(RenderRequest_Backend* req, Array<Sampler_Backend*>& list) {
	auto  bindPoint = bindless.AxBindless_SamplerState->bindPoint();
	auto  descType  = VK_DESCRIPTOR_TYPE_SAMPLER;
	RenderObjectManager_Vk_onUpdateDescriptors<Sampler_Vk>::run(this, req, list, descType, bindPoint);
}

void RenderObjectManager_Vk::onUpdateDescriptors(RenderRequest_Backend* req, Array<Texture2D_Backend*>& list) {
	auto  bindPoint = bindless.AxBindless_Texture2D->bindPoint();
	auto  descType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	RenderObjectManager_Vk_onUpdateDescriptors<Texture2D_Vk>::run(this, req, list, descType, bindPoint);
}
#endif

void RenderObjectManager_Vk::onPostCreate() {
	auto* sys  = RenderSystem_Vk::s_instance();
	auto& dev  = sys->device();

#if AX_RENDER_BINDLESS

	auto* commonShaderPass = ShaderPass_Backend::s_globalCommonShaderPass();

	auto* bindlessParamSpace = rttiCastCheck<ShaderParamSpace_Vk>(commonShaderPass->getParamSpace(BindSpace::Bindless));
	if (!bindlessParamSpace) throw Error_Undefined();

	Int total_Texture_Count	= bindless.AxBindless_Texture2D->bindCount()
							+ bindless.AxBindless_Texture3D->bindCount();

	AX_VkDescriptorPool_CreateDesc bindlessPoolDesc;
	bindlessPoolDesc.maxDestSetCount = 1;
	bindlessPoolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER       , bindless.AxBindless_SamplerState->bindCount());
	bindlessPoolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE , total_Texture_Count);
	// poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, );
	// poolDesc.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, );
	bindlessPoolDesc.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	_bindlessDescriptorPool.create(dev, bindlessPoolDesc);
	_bindlessDescriptorSet = _bindlessDescriptorPool.allocDescriptorSet(bindlessParamSpace->_descSetLayout_vk);

	dev.setObjectDebugName(_bindlessDescriptorSet, "_bindlessDescriptorSet");
#endif
	
}

} // namespace
