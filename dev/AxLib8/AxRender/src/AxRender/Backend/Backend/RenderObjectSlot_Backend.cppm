module;
export module AxRender:RenderObjectSlot_Backend;
export import :Texture;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

template<class T> class RenderObjectTable_Backend;

using RenderObjectSlotId = u32;
RenderObjectSlotId RenderObjectSlotId_None = 0;

template<class T>
class RenderObjectSlot_Backend : public NonCopyable {
public:
	using Table = RenderObjectTable_Backend<T>;
	using ResourceKey = typename T::ResourceKey;

	explicit operator bool() const { return _slotId != RenderObjectSlotId_None; }

	AX_INLINE RenderObjectSlotId slotId() const { return _slotId; }
	AX_INLINE T*	 owner() { return _owner; }

	RenderObjectSlot_Backend(T* owner, bool isFallbackDefault) : _owner(owner) {
		Table::s_get().scopedLock()->add(_owner, isFallbackDefault);
	}

	~RenderObjectSlot_Backend() {
		Table::s_get().scopedLock()->remove(_owner); 
	}

	void markDirty() {
		Table::s_get().scopedLock()->markDirty(_owner);
	}

friend class RenderObjectTable_Backend<T>;
protected:
	RenderObjectSlotId _slotId = RenderObjectSlotId_None;
	bool _dirty = false;
private:
	T*	_owner  = nullptr;
};

template<class T>
class RenderObjectTable_Backend : public NonCopyable {
	using This = RenderObjectTable_Backend;
public:
	using Handle = RenderObjectSlot_Backend<T>;
	using ResourceKey = typename T::ResourceKey;

	static constexpr bool kNeedDescriptorUpdate = std::is_base_of_v<Sampler    , T>
												| std::is_base_of_v<Texture2D  , T>;

	void add(T* obj, bool isFallbackDefault);
	void remove(T* obj);
	void markDirty(T* obj);

	T* findObject(const ResourceKey& key) {
		auto* p = _keyDict.find(key);
		return p ? *p : nullptr;
	}

	Int count() const { return _slots.size() - _freeSlots.size(); }

	RenderObjectTable_Backend();

	void onFrameEnd(class RenderRequest_Backend* req);

	static MutexProtected<This>&	s_get();

protected:
	Array<T*>                 _slots;
	Array<RenderObjectSlotId> _freeSlots;
	Array<SPtr<T>>            _dirtyObjects;
	Dict<ResourceKey, T*>     _keyDict;

	struct Frame : public NonCopyable {
		Array<RenderObjectSlotId> pendingFreeSlots;
	};
	Frame& currentFrame() { return _frames[_currentFrameIndex]; }

	Array<Frame>	_frames;
	Int _currentFrameIndex = 0;
};

} // namespace