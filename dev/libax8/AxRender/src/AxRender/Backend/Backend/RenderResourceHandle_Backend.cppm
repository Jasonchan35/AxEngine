module;
export module AxRender:RenderResourceHandle_Backend;
export import :Texture;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {

template<class T> class RenderResourceTable_Backend;

enum class ResourceSlotId : u32 { None };
AX_ENUM_ALL_OPERATOR(ResourceSlotId)

template<class T>
class RenderResourceHandle_Backend : public NonCopyable {
public:
	using Table = RenderResourceTable_Backend<T>;
	using ResourceKey = typename T::ResourceKey;

	explicit operator bool() const { return _slotId != ResourceSlotId::None; }

	AX_INLINE ResourceSlotId slotId() const { return _slotId; }
	AX_INLINE T*	 owner() { return _owner; }

	RenderResourceHandle_Backend(T* owner) : _owner(owner) {
		Table::s_get().scopedLock()->add(_owner);
	}

	~RenderResourceHandle_Backend() {
		Table::s_get().scopedLock()->remove(_owner); 
	}

	void markDirty() {
		Table::s_get().scopedLock()->markDirty(_owner);
	}

friend class RenderResourceTable_Backend<T>;
protected:
	ResourceSlotId _slotId = ResourceSlotId::None;
	bool _dirty = false;
private:
	T*	_owner  = nullptr;
};

template<class T>
class RenderResourceTable_Backend : public NonCopyable {
	using This = RenderResourceTable_Backend;
public:
	using Handle = RenderResourceHandle_Backend<T>;
	using ResourceKey = typename T::ResourceKey;

	static constexpr bool kNeedDescriptorUpdate = std::is_base_of_v<Sampler    , T>
												| std::is_base_of_v<Texture2D  , T>
												| std::is_base_of_v<Texture3D  , T>
												| std::is_base_of_v<TextureCube, T>;

	void add(T* obj);
	void remove(T* obj);
	void markDirty(T* obj);

	T* findObject(const ResourceKey& key) {
		auto* p = _keyDict.find(key);
		return p ? *p : nullptr;
	}

	Int count() const { return _slots.size() - _freeSlots.size(); }

	RenderResourceTable_Backend();

	void onFrameEnd(class RenderRequest_Backend* req);

	static MutexProtected<This>&	s_get();

protected:
	Array<T*>				_slots;
	Array<ResourceSlotId>	_freeSlots;
	Array<SPtr<T>>			_dirtyObjects;

	Dict<ResourceKey, T*>	_keyDict;

	struct Frame : public NonCopyable {
		Array<ResourceSlotId> pendingFreeSlots;
	};
	Frame& currentFrame() { return _frames[_currentFrameIndex]; }

	Array<Frame>	_frames;
	Int _currentFrameIndex = 0;
};

} // namespace