module;
module AxRender;
import :RenderObjectManager_Backend;
import :RenderRequest_Backend;
import :RenderContext_Backend;


namespace ax /*::AxRender*/ {

template<class T>
RenderObjectTable_Backend<T>::RenderObjectTable_Backend() {
	auto frameCount = RenderSystem::s_instance()->renderRequestCount();
	if (frameCount < 1) throw Error_Undefined();
	_frames.resize(frameCount);
	_slots.emplaceBack(); // slot 0 for fall back when error
}

template<class T>
auto RenderObjectTable_Backend<T>::s_get() -> MutexProtected<This>& {
	auto* mgr = RenderObjectManager_Backend::s_instance();
	return mgr->getTable<T>();
}

template<class T>
void RenderObjectTable_Backend<T>::add(T* obj) {
	if (!obj) return;

	if (auto& key = obj->resourceKey()) { _keyDict.add(key, obj); }

	auto& handle = obj->objectSlot;
	if (handle) {
		AX_ASSERT(false); // added already ?
		return;
	}

	auto slotId = RenderObjectSlotId_None;
	if (_freeSlots.size()) {
		slotId = _freeSlots.popBack();
	} else {
		slotId = ax_safe_cast_from(_slots.size());
		_slots.emplaceBack();
	}

	handle._slotId = slotId;
	_slots[slotId] = obj;

	markDirty(obj);
}

template<class T>
void RenderObjectTable_Backend<T>::markDirty(T* obj) {
	if (!obj) { AX_ASSERT(false); return; }
	auto& handle = obj->objectSlot;
	if (!handle) { AX_ASSERT(false); return; }

	auto slotId = handle._slotId;
	AX_ASSERT(_slots[slotId] == obj);

	if (handle._dirty) return;
	handle._dirty = true;
	_dirtyObjects.emplaceBack(obj);
}

template<class T>
void RenderObjectTable_Backend<T>::remove(T* obj) {
	if (!obj) return;

	if (auto& key = obj->resourceKey()) {
		AX_ASSERT(obj == *_keyDict.find(key));
		_keyDict.erase(key);
	}

	auto& handle = obj->objectSlot;
	if (!handle) {
		AX_ASSERT(false); // double remove ?
		return;
	}

	auto& slot = _slots[handle._slotId];
	AX_ASSERT(slot == obj);
	slot = nullptr;
	handle._dirty = false;

	auto& frame = currentFrame();
	frame.pendingFreeSlots.emplaceBack(handle._slotId);

	handle._slotId = RenderObjectSlotId_None;
}

template<class T>
void RenderObjectTable_Backend<T>::onFrameEnd(RenderRequest_Backend* req) {
	_currentFrameIndex = (_currentFrameIndex + 1) % _frames.size();
	auto& curFrame	   = currentFrame();

	_freeSlots.appendRange(curFrame.pendingFreeSlots);
	curFrame.pendingFreeSlots.clear();

	if (_dirtyObjects.size() <= 0) return;
	
	for (auto& e : _dirtyObjects) {
		if (!e) { AX_ASSERT(false); continue; }
		e->objectSlot._dirty = false;
	}

	if constexpr (kNeedDescriptorUpdate) {
	 	RenderObjectManager_Backend::s_instance()->onUpdateDescriptors(req, _dirtyObjects);
	}
	_dirtyObjects.clear();
}

/*---- The explicit instantiation ---*/
template class RenderObjectTable_Backend<Shader_Backend>;
template class RenderObjectTable_Backend<Sampler_Backend>;
template class RenderObjectTable_Backend<Texture2D_Backend>;

} // namespace