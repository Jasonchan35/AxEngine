module AxRender;

#if AX_RENDER_BINDLESS && AX_RENDERER_VK
import :RenderRequest_Bindless_VK;
import :RenderRequest_VK;
import :Texture_VK;
import :Material_VK;

namespace ax /*::AxRender*/ {

template<class T>
RenderRequest_Bindless_VK::Table<T>::Table() {
	auto* renderer	  = Renderer_VK::s_instance();
	auto* globalSpace = renderer->commonShader()->getPassParamSpace(0, ParamSpaceType::Global);

	if constexpr (isSampler) {
		_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;

		auto* param = globalSpace->findSamplerParam(AX_NAMEID("AxBindless_SamplerState"));
		if (!param) throw Error_Undefined();
		if (param->dataType() != RenderDataType::SamplerState) throw Error_Undefined();
		
		_bindPoint = param->bindPoint();
		_slotLimit = param->bindCount();

	} else if constexpr (isTexture2D) {
		_descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		auto* param = globalSpace->findTextureParam(AX_NAMEID("AxBindless_Texture2D"));
		if (!param) throw Error_Undefined();
		if (param->dataType() != RenderDataType::Texture2D) throw Error_Undefined();

		_bindPoint = param->bindPoint();
		_slotLimit = param->bindCount();

	} else {
		throw Error_Undefined();
	}
}

template<class T>
void RenderRequest_Bindless_VK::Table<T>::update(RenderRequest_VK* req) {
	_temp.reset();
	_writeSets.clear();

	if (!req) return;

	auto* renderer = Renderer_VK::s_instance();

	auto* mtl = renderer->commonMaterial();
	if (!mtl) return;

	auto* mtlSpace = mtl->getPassParamSpace_<MaterialParamSpace_VK>(0, ParamSpaceType::Global);
	if (!mtlSpace) return;

	auto currentDescriptorSet = mtlSpace->getUpdatedDescriptorSet(req);
	auto* lastDescriptorSet   = mtlSpace->getLastDescriptorSet();

	{ // carry updated slots from N previous frames
		auto renderRequestCount = renderer->renderRequestCount();
		
		for (Int i = 1; i < renderRequestCount; i++) {
			auto otherReqIndex = (i + req->index()) % renderRequestCount;
			auto* otherReq = rttiCastCheck<RenderRequest_VK>(renderer->getRenderRequest(otherReqIndex));
			if (!otherReq) { AX_ASSERT(false); continue; }

			Table<T>* otherTable = nullptr;
			otherReq->_bindless.getTable(otherTable);
			if (!otherTable) { AX_ASSERT(false); continue; }

			for (auto& w : otherTable->_writeSets) {
				_temp.copySlots.append(w.dstArrayElement);
			}
		}

		if (_temp.copySlots.size()) {
			_temp.copySlots.sort([](auto& a, auto& b){ return a < b; });

			u32 lastSlotId = 0;
			_temp.copySets.ensureCapacity(_temp.copySlots.size());

			for (auto& slotId : _temp.copySlots) {
				if (slotId == lastSlotId) continue; // skip duplicated id

				if (slotId == lastSlotId + 1 && _temp.copySets.size() > 0) {
					// The id is continuous, just count + 1
					auto &c = _temp.copySets.back();
					++c.descriptorCount;
				} else {
					auto& ds = _temp.copySets.emplaceBack();
					ds.sType		   = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
					ds.srcSet		   = lastDescriptorSet;
					ds.dstSet		   = currentDescriptorSet;
					ds.srcArrayElement = slotId;
					ds.dstArrayElement = slotId;
					ds.srcBinding	   = AX_VkUtil::castUInt32(ax_enum_int(_bindPoint));
					ds.dstBinding	   = AX_VkUtil::castUInt32(ax_enum_int(_bindPoint));
					ds.descriptorCount = 1;
				}

				lastSlotId = slotId;
			}
		}
	}


	auto addWriteSet = [&](auto& object) -> VkWriteDescriptorSet& {
		auto& w = _writeSets.emplaceBack();
		w.sType			  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		w.pNext			  = nullptr;
		w.descriptorType  = _descriptorType;
		w.dstSet		  = currentDescriptorSet;
		w.dstBinding	  = AX_VkUtil::castUInt32(ax_enum_int(_bindPoint));
		w.dstArrayElement = ax_enum_int(object->resourceHandle.slotId());
		w.descriptorCount = 1;
		return w;
	};

	auto& srcList = req->resourcesToUpdateDescriptor;

	if constexpr (isSampler) {
		auto& samplersList = srcList.getList<Sampler_Backend>();
		_temp.imageInfos.ensureCapacity(samplersList.size()); // ensure the element pointer not change
		for (auto& sampler_ : samplersList) {
			auto* sampler = rttiCastCheck<Sampler_VK>(sampler_.ptr());
			if (!sampler) continue;

			auto& dst = _temp.imageInfos.emplaceBack();
			dst.imageView	= VK_NULL_HANDLE;
			dst.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			dst.sampler		= sampler->vkHandle();
			addWriteSet(sampler).pImageInfo = &dst;
		}

	} else if constexpr (isTexture2D) {
		auto& tex2dList = srcList.getList<Texture2D_Backend>();
		_temp.imageInfos.ensureCapacity(tex2dList.size()); // ensure the element pointer not change
		for (auto& tex_ : tex2dList) {
			auto* tex = rttiCastCheck<Texture2D_VK>(tex_.ptr());
			if (!tex) continue;

			auto& dst = _temp.imageInfos.emplaceBack();
			tex->_bindImage(req, dst);
			addWriteSet(tex).pImageInfo = &dst;
		}
	} else {
		AX_ASSERT(false);
	}

	if (_writeSets.size() <= 0 && _temp.copySets.size() <= 0) return;

	// vkUpdateDescriptorSets() will handle writeSets before copySets, but we need copySets before writeSets
	AX_vkUpdateDescriptorSets(renderer->device(), {}, _temp.copySets);
	AX_vkUpdateDescriptorSets(renderer->device(), _writeSets, {});
}

void RenderRequest_Bindless_VK::update(RenderRequest_VK * req) {
	_samplerTable.update(req);
	_texture2DTable.update(req);
}

/*---- The explicit instantiation ---*/ \
template class RenderRequest_Bindless_VK::Table<Sampler>;
template class RenderRequest_Bindless_VK::Table<Texture2D>;

} // namespace

#endif // AX_RENDERER_VK && AX_RENDER_BINDLESS

