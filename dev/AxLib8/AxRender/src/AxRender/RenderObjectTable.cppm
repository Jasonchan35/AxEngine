module;
export module AxRender:RenderObjectTable;
export import :RenderSystem;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

template<class T> class RenderObjectTable;

using RenderObjectSlotId = u32;
RenderObjectSlotId RenderObjectSlotId_None = u32_max;

template<class T>
class RenderObjectSlot : public NonCopyable {
public:
	using Table = RenderObjectTable<T>;
	using ResourceKey = typename T::ResourceKey;

	explicit operator bool() const { return _slotId != RenderObjectSlotId_None; }

	AX_INLINE RenderObjectSlotId slotId() const { return _slotId; }
	AX_INLINE T*	 owner() { return _owner; }

	RenderObjectSlot(T* owner, bool isFallbackDefault = false);
	~RenderObjectSlot();
	void markDirty();

	friend class RenderObjectTable<T>;
protected:
	RenderObjectSlotId _slotId = RenderObjectSlotId_None;
	bool _dirty = false;
private:
	T*	_owner  = nullptr;
};

class IRenderObjectTable : public RttiObject {
	AX_RTTI_INFO(IRenderObjectTable, RttiObject)
public:
	virtual void onFrameEnd(class RenderRequest* req) {}
};

MutexProtected<UPtr<IRenderObjectTable>>& RenderObjectManager_Backend_getTable(Rtti* rtti);

template<class T>
class RenderObjectTable : public IRenderObjectTable {
	AX_RTTI_INFO(RenderObjectTable, IRenderObjectTable)
public:
	using Handle = RenderObjectSlot<T>;
	using ResourceKey = typename T::ResourceKey;
	static constexpr bool kHasResourceKey = !Type_IsSame<ResourceKey, TagNoInit_T>;

	void add(T* obj, bool isFallbackDefault);
	void remove(T* obj);
	void markDirty(T* obj);

	T* findObject(const ResourceKey& key) {
		auto* p = _keyDict.find(key);
		return p ? *p : nullptr;
	}

	Int count() const { return _slots.size() - _freeSlots.size(); }

	RenderObjectTable();

	virtual void onFrameEnd(class RenderRequest* req) override;

	GpuBufferPool* gpuBufferPool() { return _gpuBufferPool; }
	
protected:
	friend class RenderObjectManager_Backend;
	Array<T*>                 _slots;
	Array<RenderObjectSlotId> _freeSlots;
	Array<T*>                 _dirtyObjects;
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

void RenderObjectTable_init(IRenderObjectTable* table, GpuBufferPool* pool);

template<class T> inline
typename MutexProtected<RenderObjectTable<T>>::ScopedLock RenderObjectTable_getLocked() {
	auto lock   = RenderObjectManager_Backend_getTable(rttiOf<T>()).scopedLock();
	using Table = RenderObjectTable<T>; 
	Table* table = rttiCastCheck<Table>(lock->ptr());
	if (!table) {
		auto uptr = UPtr_new<Table>(AX_NEW);
		table = uptr.ptr();
		RenderObjectTable_init(table, table->gpuBufferPool());
		*lock.data() = std::move(uptr);
	}
	
	auto* data  = rttiCastCheck<RenderObjectTable<T>>(lock->ptr());
	auto* mutex = lock.detach();
	return typename MutexProtected<RenderObjectTable<T>>::ScopedLock(*mutex, data);
}

template<class T>
RenderObjectTable<T>::RenderObjectTable() {
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
RenderObjectSlot<T>::RenderObjectSlot(T* owner, bool isFallbackDefault): _owner(owner) {
	auto lock = RenderObjectTable_getLocked<T>();
	lock->add(_owner, isFallbackDefault);
}

template<class T>
RenderObjectSlot<T>::~RenderObjectSlot() {
	auto lock = RenderObjectTable_getLocked<T>();
	lock->remove(_owner); 
}

template<class T>
void RenderObjectSlot<T>::markDirty() {
	auto lock = RenderObjectTable_getLocked<T>();
	lock->markDirty(_owner);
}

template<class T>
void RenderObjectTable<T>::add(T* obj, bool isFallbackDefault) {
	if (!obj) return;

	if constexpr (kHasResourceKey) {
		if (auto& key = obj->resourceKey()) { _keyDict.add(key, obj); }
	}

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
void RenderObjectTable<T>::markDirty(T* obj) {
	if (!obj) { AX_ASSERT(false); return; }
	auto& handle = obj->objectSlot;
	auto slotId = handle._slotId;
	AX_ASSERT(_slots[slotId] == obj);

	if (handle._dirty) return;
	handle._dirty = true;
	_dirtyObjects.emplaceBack(obj);
}

template<class T>
void RenderObjectTable<T>::remove(T* obj) {
	if (!obj) return;

	if constexpr (kHasResourceKey) {
		if (auto& key = obj->resourceKey()) {
			AX_ASSERT(obj == *_keyDict.find(key));
			_keyDict.erase(key);
		}
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
void RenderObjectTable<T>::onFrameEnd(RenderRequest* req) {
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