module;
module AxRender.ResourceHandle_Backend;
import AxRender.ResourceManager_Backend;
import AxRender.RenderRequest_Backend;
import AxRender.RenderContext_Backend;


namespace ax::AxRender {

template<class T>
ResourceTable_Backend<T>::ResourceTable_Backend() {
	auto frameCount = Renderer::s_instance()->renderRequestCount();
	if (frameCount < 1) throw Error_Undefined(AX_SRC_LOC);
	_frames.resize(frameCount);
	_slots.emplaceBack(); // slot 0 for fall back when error
}

template<class T> inline
MutexProtected<ResourceTable_Backend<T>>* ResourceTable_Backend<T>::s_get() {
	MutexProtected<This>* p = nullptr;
	ResourceManager_Backend::s_instance()->getTable(p);
	return p;
}

template<class T>
void ResourceTable_Backend<T>::add(T* obj) {
	if (!obj) return;

	if (auto& key = obj->resourceKey()) { _keyDict.add(key, obj); }

	auto& handle = obj->resourceHandle;
	if (handle) {
		AX_ASSERT(false); // added already ?
		return;
	}

	auto slotId = ResourceSlotId::None;
	if (_freeSlots.size()) {
		slotId = _freeSlots.popBack();
	} else {
		ax_safe_assign(slotId, _slots.size());
		_slots.emplaceBack();
	}

	handle._slotId = slotId;
	_slots[ax_enum_int(slotId)] = obj;

	markDirty(obj);
}

template<class T>
void ResourceTable_Backend<T>::markDirty(T* obj) {
	if (!obj) { AX_ASSERT(false); return; }
	auto& handle = obj->resourceHandle;
	if (!handle) { AX_ASSERT(false); return; }

	auto slotId = handle._slotId;
	AX_ASSERT(_slots[ax_enum_int(slotId)] == obj);

	if (handle._dirty) return;
	handle._dirty = true;
	_dirtyObjects.emplaceBack(obj);
}

template<class T>
void ResourceTable_Backend<T>::remove(T* obj) {
	if (!obj) return;

	if (auto& key = obj->resourceKey()) {
		AX_ASSERT(obj == *_keyDict.find(key));
		_keyDict.erase(key);
	}

	auto& handle = obj->resourceHandle;
	if (!handle) {
		AX_ASSERT(false); // double remove ?
		return;
	}

	auto& slot = _slots[ax_enum_int(handle._slotId)];
	AX_ASSERT(slot == obj);
	slot = nullptr;
	handle._dirty = false;

	auto& frame = currentFrame();
	frame.pendingFreeSlots.emplaceBack(handle._slotId);

	handle._slotId = ResourceSlotId::None;
}

template<class T>
void ResourceTable_Backend<T>::onFrameEnd(RenderRequest_Backend* req) {
	_currentFrameIndex = (_currentFrameIndex + 1) % _frames.size();
	auto& curFrame	   = currentFrame();

	_freeSlots.appendRange(curFrame.pendingFreeSlots);
	curFrame.pendingFreeSlots.clear();

	for (auto& e : _dirtyObjects) {
		if (!e) { AX_ASSERT(false); continue; }
		e->resourceHandle._dirty = false;

		if constexpr (kNeedDescriptorUpdate) {
			req->resourcesToUpdateDescriptor.add(std::move(e));
		}
	}
	_dirtyObjects.clear();
}

/*---- The explicit instantiation ---*/
template class ResourceTable_Backend<Shader_Backend>;
template class ResourceTable_Backend<Sampler_Backend>;
template class ResourceTable_Backend<Texture2D_Backend>;
template class ResourceTable_Backend<Texture3D_Backend>;

} // namespace