module;
export module AxRender:RenderObjectSlot_Backend;
export import :Texture;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

template<class T> class RenderObjectTable_Backend;

using RenderObjectSlotId = u32;
RenderObjectSlotId RenderObjectSlotId_None = u32_max;

template<class T>
class RenderObjectSlot_Backend : public NonCopyable {
public:
	using Table = RenderObjectTable_Backend<T>;
	using ResourceKey = typename T::ResourceKey;

	explicit operator bool() const { return _slotId != RenderObjectSlotId_None; }

	AX_INLINE RenderObjectSlotId slotId() const { return _slotId; }
	AX_INLINE T*	 owner() { return _owner; }

	RenderObjectSlot_Backend(T* owner, bool isFallbackDefault);
	~RenderObjectSlot_Backend();
	void markDirty();

	friend class RenderObjectTable_Backend<T>;
protected:
	RenderObjectSlotId _slotId = RenderObjectSlotId_None;
	bool _dirty = false;
private:
	T*	_owner  = nullptr;
};

class IRenderObjectTable_Backend : public RttiObject {
	AX_RTTI_INFO(IRenderObjectTable_Backend, RttiObject)
public:
	virtual void onFrameEnd(class RenderRequest* req) {}
};

MutexProtected<UPtr<IRenderObjectTable_Backend>>& RenderObjectManager_Backend_getTable(Rtti* rtti);

template<class T>
class RenderObjectTable_Backend : public IRenderObjectTable_Backend {
	AX_RTTI_INFO(RenderObjectTable_Backend, IRenderObjectTable_Backend)
public:
	using Handle = RenderObjectSlot_Backend<T>;
	using ResourceKey = typename T::ResourceKey;

	void add(T* obj, bool isFallbackDefault);
	void remove(T* obj);
	void markDirty(T* obj);

	T* findObject(const ResourceKey& key) {
		auto* p = _keyDict.find(key);
		return p ? *p : nullptr;
	}

	Int count() const { return _slots.size() - _freeSlots.size(); }

	RenderObjectTable_Backend();

	virtual void onFrameEnd(class RenderRequest* req) override;

protected:
	friend class RenderObjectManager_Backend;
	Array<T*>                 _slots;
	Array<RenderObjectSlotId> _freeSlots;
	Array<SPtr<T>>            _dirtyObjects;
	Dict<ResourceKey, T*>     _keyDict;
	
	using GpuData = typename T::GpuData;
	static constexpr bool kHasGpuData = !Type_IsSame<GpuData, nullptr_t>;
	
	StructuredGpuBufferPool_<GpuData>	_gpuBufferPool;
	StructuredGpuBuffer_<GpuData>		_gpuBuffer;

	struct Frame : public NonCopyable {
		Array<RenderObjectSlotId> pendingFreeSlots;
	};
	Frame& currentFrame() { return _frames[_currentFrameIndex]; }

	Array<Frame>	_frames;
	Int _currentFrameIndex = 0;
};

template<class T> inline
typename MutexProtected<RenderObjectTable_Backend<T>>::ScopedLock RenderObjectTable_getLocked() {
	auto lock   = RenderObjectManager_Backend_getTable(rttiOf<T>()).scopedLock();
	auto* data  = rttiCastCheck<RenderObjectTable_Backend<T>>(lock->ptr());
	auto* mutex = lock.detach();
	return typename MutexProtected<RenderObjectTable_Backend<T>>::ScopedLock(*mutex, data);
}

template<class T>
RenderObjectTable_Backend<T>::RenderObjectTable_Backend() {
	auto frameCount = RenderSystem::s_instance()->renderRequestCount();
	if (frameCount < 1) throw Error_Undefined();
	_frames.resize(frameCount);
	_slots.emplaceBack(nullptr); // slot 0 for fallback when error
	
	if constexpr (kHasGpuData) {
		// TODO - get maxSize and pageSize from config file
		Int maxSize  = T::s_gpuBufferMaxSize();
		Int pageSize = T::s_gpuBufferPageSize();
		auto bufName = T::s_gpuBufferName();
		_gpuBufferPool.create(AX_NEW, bufName, maxSize, pageSize);
		_gpuBuffer.create(AX_NEW, bufName, _gpuBufferPool);
	}
}

template<class T>
RenderObjectSlot_Backend<T>::RenderObjectSlot_Backend(T* owner, bool isFallbackDefault): _owner(owner) {
	auto lock = RenderObjectTable_getLocked<T>();
	lock->add(_owner, isFallbackDefault);
}

template<class T>
RenderObjectSlot_Backend<T>::~RenderObjectSlot_Backend() {
	auto lock = RenderObjectTable_getLocked<T>();
	lock->remove(_owner); 
}

template<class T>
void RenderObjectSlot_Backend<T>::markDirty() {
	auto lock = RenderObjectTable_getLocked<T>();
	lock->markDirty(_owner);
}

template<class T>
void RenderObjectTable_Backend<T>::add(T* obj, bool isFallbackDefault) {
	if (!obj) return;

	if (auto& key = obj->resourceKey()) { _keyDict.add(key, obj); }

	auto& handle = obj->objectSlot;
	if (handle) {
		AX_ASSERT(false); // added already ?
		return;
	}

	auto slotId = RenderObjectSlotId_None;
	if (isFallbackDefault) {
		slotId = 0;
		if (_slots[slotId]) {
			AX_ASSERT(false); // added already ?
			return;
		}
		_slots[slotId] = obj;

	} else {
		if (_freeSlots.size()) {
			slotId = _freeSlots.popBack();
		} else {
			slotId = ax_safe_cast_from(_slots.size());
			_slots.emplaceBack(obj);
		}
	}

	handle._slotId = slotId;
	markDirty(obj);
}

template<class T>
void RenderObjectTable_Backend<T>::markDirty(T* obj) {
	if (!obj) { AX_ASSERT(false); return; }
	auto& handle = obj->objectSlot;
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
void RenderObjectTable_Backend<T>::onFrameEnd(RenderRequest* req) {
	_currentFrameIndex = (_currentFrameIndex + 1) % _frames.size();
	auto& curFrame	   = currentFrame();

	_freeSlots.appendRange(curFrame.pendingFreeSlots);
	curFrame.pendingFreeSlots.clear();

	if (_dirtyObjects.size() <= 0) return;
	
	for (auto& obj : _dirtyObjects) {
		if (!obj) { AX_ASSERT(false); continue; }
		obj->objectSlot._dirty = false;
		
		if constexpr (kHasGpuData) {
			if (auto* data = obj->onGetGpuData(req)) {
				_gpuBuffer.setValue(obj->objectSlot.slotId(), *data);
				_gpuBuffer.buffer->getUploadedGpuBuffer(req);
			}
		}
	}

	_dirtyObjects.clear();
}


} // namespace