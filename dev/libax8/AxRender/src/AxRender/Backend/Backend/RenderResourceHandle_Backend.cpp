module;
module AxRender;
import :RenderResourceManager_Backend;
import :RenderRequest_Backend;
import :RenderContext_Backend;


namespace ax /*::AxRender*/ {

template<class T>
RenderResourceTable_Backend<T>::RenderResourceTable_Backend() {
	auto frameCount = RenderSystem::s_instance()->renderRequestCount();
	if (frameCount < 1) throw Error_Undefined();
	_frames.resize(frameCount);
	_slots.emplaceBack(); // slot 0 for fall back when error
}

template<class T>
auto RenderResourceTable_Backend<T>::s_get() -> MutexProtected<This>& {
	auto* mgr = RenderResourceManager_Backend::s_instance();
	return mgr->getTable<T>();
}

template<class T>
void RenderResourceTable_Backend<T>::add(T* obj) {
	if (!obj) return;

	if (auto& key = obj->resourceKey()) { _keyDict.add(key, obj); }

	auto& handle = obj->resourceHandle;
	if (handle) {
		AX_ASSERT(false); // added already ?
		return;
	}

	auto slotId = RenderResourceSlotId_None;
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
void RenderResourceTable_Backend<T>::markDirty(T* obj) {
	if (!obj) { AX_ASSERT(false); return; }
	auto& handle = obj->resourceHandle;
	if (!handle) { AX_ASSERT(false); return; }

	auto slotId = handle._slotId;
	AX_ASSERT(_slots[slotId] == obj);

	if (handle._dirty) return;
	handle._dirty = true;
	_dirtyObjects.emplaceBack(obj);
}

template<class T>
void RenderResourceTable_Backend<T>::remove(T* obj) {
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

	auto& slot = _slots[handle._slotId];
	AX_ASSERT(slot == obj);
	slot = nullptr;
	handle._dirty = false;

	auto& frame = currentFrame();
	frame.pendingFreeSlots.emplaceBack(handle._slotId);

	handle._slotId = RenderResourceSlotId_None;
}

template<class T>
void RenderResourceTable_Backend<T>::onFrameEnd(RenderRequest_Backend* req) {
	_currentFrameIndex = (_currentFrameIndex + 1) % _frames.size();
	auto& curFrame	   = currentFrame();

	_freeSlots.appendRange(curFrame.pendingFreeSlots);
	curFrame.pendingFreeSlots.clear();

	for (auto& e : _dirtyObjects) {
		if (!e) { AX_ASSERT(false); continue; }
		e->resourceHandle._dirty = false;
	}

	if constexpr (kNeedDescriptorUpdate) {
		RenderResourceManager_Backend::s_instance()->onUpdateDescriptors(req, _dirtyObjects);
	}
	_dirtyObjects.clear();
}

/*---- The explicit instantiation ---*/
template class RenderResourceTable_Backend<Shader_Backend>;
template class RenderResourceTable_Backend<Sampler_Backend>;
template class RenderResourceTable_Backend<Texture2D_Backend>;

} // namespace