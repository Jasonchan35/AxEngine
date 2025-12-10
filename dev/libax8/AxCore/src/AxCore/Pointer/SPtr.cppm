module;


export module AxCore.SPtr;
export import AxCore.UPtr;
import AxCore.Atomic;
import AxCore.SpinLock;

export namespace ax {
// +-----------------+     +----------+
// | SPtrReferenable |     | SPtr x N |
// |                 |<----| - p*     |
// | - refCount      |     +----------+
// +-----------------+     +-----------+     +----------+
// | WPtrReferenable |     | WPtrBlock |     | WPtr x N |
// | - WPtrBlock*    |---->|           |<----| - p*     |
// |                 |<----| - obj*    |     +----------+
// +-----------------+     +-----------+


template<class T> class SPtr;

struct SPtr_Config_ThreadSafe {
	using RefCountType = Thread::AtomicInt;
	using LockType     = Thread::SpinLock;
};

struct SPtr_Config_NonThreadSafe {
	using RefCountType = Int;
	using LockType     = Thread::NullSpinLock;
};

class SPtrReferenableBase {
	AX_NON_COPYABLE(SPtrReferenableBase)
protected:
	SPtrReferenableBase() = default;
};

template<class CONFIG> class WPtrReferenable_; 

template<class CONFIG>
class SPtrReferenable_ : public SPtrReferenableBase {
public:
	using _WPtrReferenable = WPtrReferenable_<CONFIG>;
	
	AX_INLINE Int _addSPtrRefCount() const		{ return ++m_SPtrRefCount; }
	AX_INLINE Int _releaseSPtrRefCount() const	{ return --m_SPtrRefCount; }
	AX_INLINE Int _getSPtrRefCount() const		{ return m_SPtrRefCount; }

	AX_INLINE ~SPtrReferenable_() { AX_ASSERT(m_SPtrRefCount == 0); }

private:
	mutable typename CONFIG::RefCountType	m_SPtrRefCount = 0;
};

using SPtrReferenable = SPtrReferenable_<SPtr_Config_ThreadSafe>;
using SPtrReferenable_NonThreadSafe = SPtrReferenable_<SPtr_Config_NonThreadSafe>;

template <class CONFIG>
struct WPtrBlock_ : public SPtrReferenable_<CONFIG> {
	using LockType = typename CONFIG::LockType;
	struct Data {
		SPtrReferenable* obj = nullptr;
	};
	Thread::LockProtected<LockType, Data> data;
};

template<class CONFIG>
class WPtrReferenable_ : public SPtrReferenable_<CONFIG> {
public:
	using WPtrBlock = WPtrBlock_<CONFIG>;
	~WPtrReferenable_() {
		if (auto* block = _weakPtrBlock.ptr()) {
			AX_ASSERT(block->data.scopedLock()->obj == nullptr);
		}
	}

	template<class R> friend class WPtr;
	template<class R> friend class SPtr_Internal;
protected:
	SPtr<WPtrBlock>		_weakPtrBlock;
};

using WPtrReferenable = WPtrReferenable_<SPtr_Config_ThreadSafe>;

} namespace ax { // no export

// add this layer to support SPtr<forward declare class>
template<class T>
class SPtr_Internal : public NonCopyable {
public:
	SPtr_Internal() = delete;
	
	static constexpr T* ref(SPtr<T>* s, T* p) {
		static_assert(std::is_base_of_v<SPtrReferenableBase, T>);
		if (s->_p != p) {
			unref(s);
			s->_p = p;
			if (p) s->_p->_addSPtrRefCount();
		}
		return p;
	}

	static constexpr bool hasWPtrReferenable = std::is_base_of_v<typename T::_WPtrReferenable, T>;
	
	static constexpr void unref(SPtr<T>* s) {
		static_assert(std::is_base_of_v<SPtrReferenableBase, T>);
		auto* obj = s->_p;
		if (!obj) return;
		auto refCount = _releaseRefCount(s);
		AX_ASSERT(refCount >= 0);
		if (refCount == 0) {
			auto* p = ax_const_cast(s->_p);
			ax_delete(p);
		}
		s->_p = nullptr;
	}
	
private:
	static AX_INLINE constexpr Int _releaseRefCount(SPtr<T>* s) {
		auto* obj = s->_p;
		if constexpr (hasWPtrReferenable) {
			if (auto* wb = obj->_weakPtrBlock.ptr()) {
				// lock before release ref count
				auto wbData   = ax_const_cast(wb)->data.scopedLock();
				//------				
				auto refCount = obj->_releaseSPtrRefCount();
				if (refCount == 0) { 
					AX_ASSERT(wbData->obj == obj);
					wbData->obj = nullptr;
				}
				return refCount;
			}
		}

		auto refCount = obj->_releaseSPtrRefCount();
		return refCount;
	}
};

} export namespace ax {

//! Intrusive Shared pointer, the reference count is embedded in object
//! therefore SPtr<T> and T* can be interchangable
//! unlike std::shared_ptr<T>, when create shared_ptr<T> from T*, 
//! may create different control block to hold the reference count
//! which may cause double delete of same object
template<class T>
class SPtr : public PtrBase<T> {
	using Base = PtrBase<T>;
public:
	AX_INLINE	SPtr() = default;
	AX_INLINE	SPtr(std::nullptr_t) noexcept {}
	AX_INLINE	SPtr(const SPtr&  p) noexcept { ref(p._p); }
	AX_INLINE	SPtr(      SPtr&& r) noexcept { _move(std::move(r)); }
	AX_INLINE	SPtr(T* p) noexcept { ref(p); }

	template<class... ARGS>
	AX_INLINE	SPtr(AxTag::NewObject_, const MemAllocRequest& req, ARGS&&... args) { newObject(req, AX_FORWARD(args)...); }

	template<class R>
	AX_INLINE	SPtr(SPtr<R> && r) noexcept { _move(std::move(r)); }

	template<class R>
	AX_INLINE	SPtr(SPtr<R> &r) noexcept { ref(r.ptr()); }

	AX_INLINE	~SPtr() { unref(); }

	AX_INLINE T*	 ref	(T* p) noexcept;
	AX_INLINE void unref	() noexcept;

	AX_INLINE operator       T* () &       { return _p; }
	AX_INLINE operator const T* () const & { return _p; }

//	operator T* () && = delete;

	AX_INLINE	void operator=(std::nullptr_t) noexcept	{ unref(); }

	AX_INLINE	void operator=(T* p) noexcept { ref(p); }
	template<class R> AX_INLINE	void operator=(SPtr<R> &  r) { ref(r.ptr()); }
	template<class R> AX_INLINE	void operator=(SPtr<R> && r) noexcept { _move(std::move(r)); }

	template<class... ARGS> AX_INLINE
	T*	newObject(const MemAllocRequest& req, ARGS&&...args) { return ref(new (req) T(AX_FORWARD(args)...)); }

	template<class R> AX_NODISCARD AX_INLINE bool operator==(const SPtr<R>& r) const { return _p == r._p; }
	template<class R> AX_NODISCARD AX_INLINE bool operator==(R * const r) const { return _p == r; }
	template<class R> AX_NODISCARD AX_INLINE bool operator!=(R * const r) const { return _p != r; }
	template<class R> AX_NODISCARD AX_INLINE bool operator!=(const SPtr<R>& r) const { return _p != r._p; }
	
	AX_NODISCARD AX_INLINE static SPtr<T> s_ref(T* p) noexcept { return SPtr(p); }
	AX_NODISCARD AX_INLINE static SPtr<T> s_ref_DontAddRefCount(T* p) noexcept { SPtr<T> o; o._p = p; return o; }
	
	friend  class SPtr_Internal<T>;
	template<class R> friend class SPtr;
protected:
	template<class R>
	AX_INLINE	void _move(SPtr<R> && r) noexcept;
	using Base::_p;
};

template<class T> constexpr bool Type_IsSPtr = false; 
template<class T> constexpr bool Type_IsSPtr<SPtr<T>> = true; 

template<class R, class T> AX_NODISCARD AX_INLINE constexpr bool operator==(R* const a, const SPtr<T>& b) { return b == a; }
template<class R, class T> AX_NODISCARD AX_INLINE constexpr bool operator!=(R* const a, const SPtr<T>& b) { return b != a; }

template<class T> AX_NODISCARD AX_INLINE constexpr bool operator==(const std::nullptr_t&, const SPtr<T>& b) { return b == nullptr; }
template<class T> AX_NODISCARD AX_INLINE constexpr bool operator!=(const std::nullptr_t&, const SPtr<T>& b) { return b != nullptr; }

template<class T> inline
SPtr<T> SPtr_ref(T* p) noexcept { return SPtr<T>::s_ref(p); }

template<class T> inline
SPtr<T> SPtr_fromUPtr(UPtr<T> && p) noexcept { return SPtr<T>::s_ref(p.detach()); }

template<class T, class... ARGS> AX_INLINE
SPtr<T> SPtr_new(const MemAllocRequest& req, ARGS &&... args) {
	return SPtr_ref(new (req) T(AX_FORWARD(args)...));
}

//--------------------------------

template<class T> AX_INLINE
void SPtr<T>::unref() noexcept {
	SPtr_Internal<T>::unref(this);
}

template<class T> AX_INLINE
T* SPtr<T>::ref(T * p) noexcept {
	static_assert(std::is_base_of_v<SPtrReferenableBase, T>);
	return SPtr_Internal<T>::ref(this, p);
}

template<class T>
template<class R> AX_INLINE
void SPtr<T>::_move(SPtr<R> && r) noexcept {
	if (static_cast<void*>(&r) == this) return;
	auto* rp = static_cast<T*>(r.ptr());
	if (rp == _p) return;
	unref();
	_p = rp;
	r._p = nullptr;
}

} // namespace

