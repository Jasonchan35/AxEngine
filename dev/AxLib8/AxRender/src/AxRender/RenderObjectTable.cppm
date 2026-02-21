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
	using Object = T;
	using Table = RenderObjectTable<T>;
	using ResourceKey = typename T::ResourceKey;

	explicit operator bool() const { return _slotId != RenderObjectSlotId_None; }

	AX_INLINE RenderObjectSlotId slotId() const { return _slotId; }
	AX_INLINE T*	 owner() { return _owner; }

	RenderObjectSlot(T* owner);
	~RenderObjectSlot();
	void markDirty();
	
	void _internalResetDirty() { _dirty = false; }
protected:
	friend class RenderObjectTable<T>;
	RenderObjectSlotId _slotId = RenderObjectSlotId_None;
	bool _dirty = false;
private:
	T*	_owner  = nullptr;
};

class RenderObjectTableBase : public RttiObject {
	AX_RTTI_INFO(RenderObjectTableBase, RttiObject)
public:
	virtual void onFrameEnd(class RenderRequest* req) {}
	Rtti* objRtti() const { return _objRtti; }
	
	virtual GpuBufferPool* onGetGpuBufferPool() = 0;
protected:
	Rtti* _objRtti = nullptr;
};

RenderObjectTableBase* RenderObjectManager_Backend_getTable(Rtti* rtti);

template<class T>
class RenderObjectTable : public RenderObjectTableBase {
	AX_RTTI_INFO(RenderObjectTable, RenderObjectTableBase)
public:
	static_assert(std::is_same_v<T, typename decltype(T::objectSlot)::Object>);
	
	static This* s_instance();

	using Handle = RenderObjectSlot<T>;
	using ResourceKey = typename T::ResourceKey;
	static constexpr bool kHasResourceKey = !Type_IsSame<ResourceKey, nullptr_t>;

	void add(T* obj);
	void remove(T* obj);
	void markDirty(T* obj);

	T* findObject(const ResourceKey& key) {
		auto* p = _keyDict.find(key);
		return p ? *p : nullptr;
	}

	Int count() const { return _slots.size() - _freeSlots.size(); }

	RenderObjectTable();

	virtual void onFrameEnd(class RenderRequest* req) override;

	virtual GpuBufferPool* onGetGpuBufferPool() override { return _gpuBufferPool; }
	
	MutSpan<T*>	slots() { return _slots; }
	
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

void RenderObjectManager_Backend_addTable(RenderObjectTableBase* table);

template<class T> inline
RenderObjectTable<T>* RenderObjectTable<T>::s_instance() {
	auto* tableBase = RenderObjectManager_Backend_getTable(rttiOf<T>());
	if (!tableBase) {
		auto newTable = SPtr_new<RenderObjectTable<T>>(AX_NEW);
		RenderObjectManager_Backend_addTable(newTable);
		tableBase = newTable.ptr();
	}
	return rttiCastCheck<RenderObjectTable<T>>(tableBase);
}

template<class T>
RenderObjectTable<T>::RenderObjectTable() {
	_objRtti = rttiOf<T>();
	auto frameCount = RenderSystem::s_instance()->renderRequestCount();
	if (frameCount < 1) throw Error_Undefined();
	_frames.resize(frameCount);
	
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
RenderObjectSlot<T>::RenderObjectSlot(T* owner): _owner(owner) {
	auto* table = Table::s_instance();
	table->add(_owner);
}

template<class T>
RenderObjectSlot<T>::~RenderObjectSlot() {
	auto* table = Table::s_instance();
	table->remove(_owner); 
}

template<class T>
void RenderObjectSlot<T>::markDirty() {
	auto* table = Table::s_instance();
	table->markDirty(_owner);
}

template<class T>
void RenderObjectTable<T>::add(T* obj) {
	if (!obj) return;

	if constexpr (kHasResourceKey) {
		if (auto* key = obj->resourceKey()) {
			AX_ASSERT(_keyDict.find(*key) == nullptr); // already added
			_keyDict.add(*key, obj);
		}
	}

	auto& slot = obj->objectSlot;
	if (slot) {
		AX_ASSERT(false); // added already ?
		return;
	}

	if (_freeSlots.size()) {
		slot._slotId = _freeSlots.popBack();
	} else {
		slot._slotId = ax_safe_cast_from(_slots.size());
		_slots.emplaceBack(obj);
	}

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
		if (auto* key = obj->resourceKey()) {
			AX_ASSERT(obj == *_keyDict.find(*key));
			_keyDict.erase(*key);
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
		obj->objectSlot._internalResetDirty();
		
		if constexpr (kHasGpuData) {
			if (auto* data = obj->onGetGpuData(req)) {
				_gpuBuffer.setValue(obj->objectSlot.slotId(), *data);
				_gpuBuffer.getUploadedGpuBuffer(req);
			}
		}
	}

	_dirtyObjects.clear();
	
	if (_gpuBufferPool) {
		_gpuBufferPool->onGpuUpdatePages(req);
	}
	
}


} // namespace