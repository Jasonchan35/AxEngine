module;
#include "AxPlatform-pch.h"

export module AxPlatform.SPtr;

export import AxPlatform.UPtr;
import AxPlatform.Atomic;
import AxPlatform.SpinLock;

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

class SPtrReferenableBase : public NonCopyable {
protected:
	SPtrReferenableBase() = default;
};

template<bool USE_ATOMIC_INT> class WPtrReferenable_; 

template<bool USE_ATOMIC_INT>
class SPtrReferenable_ : public SPtrReferenableBase {
public:
	using _WPtrReferenable = WPtrReferenable_<USE_ATOMIC_INT>;
	
	using SPtrRefCounter = std::conditional_t<USE_ATOMIC_INT, Thread::AtomicInt, Int>; 
	
	AX_INLINE Int _addSPtrRef() const		{ return ++m_SPtrRefCount; }
	AX_INLINE Int _releaseSPtrRef() const	{ return --m_SPtrRefCount; }
	AX_INLINE Int _getSPtrRefCount() const	{ return m_SPtrRefCount; }

	AX_INLINE ~SPtrReferenable_() { AX_ASSERT(m_SPtrRefCount == 0); }

private:
	mutable SPtrRefCounter	m_SPtrRefCount;
};

using SPtrReferenable = SPtrReferenable_<true>;

template <bool USE_ATOMIC_PTR>
struct WPtrBlock_ : public SPtrReferenable_<USE_ATOMIC_PTR> {
	using LockType = std::conditional_t<USE_ATOMIC_PTR, Thread::SpinLock, Thread::NullSpinLock>;
	
	struct Data {
		SPtrReferenable* obj = nullptr;
	};
	Thread::LockProtected<LockType, Data> data;
};

template<bool USE_ATOMIC_INT>
class WPtrReferenable_ : public SPtrReferenable_<USE_ATOMIC_INT> {
public:
	using WPtrBlock = WPtrBlock_<USE_ATOMIC_INT>;
	~WPtrReferenable_() {
		if (auto* block = _weakPtrBlock.ptr()) {
			AX_ASSERT(block->data.scopedLock()->obj == nullptr);
		}
	}
	SPtr<WPtrBlock>		_weakPtrBlock;
};

using WPtrReferenable = WPtrReferenable_<true>;

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
			if (p) s->_p->_addSPtrRef();
		}
		return p;
	}

	static constexpr bool kHasWPtrBlock = requires (T & o) { o._weakPtrBlock; };
	
	static constexpr void unref(SPtr<T>* s) {
		static_assert(std::is_base_of_v<SPtrReferenableBase, T>);
		auto* obj = s->_p;
		if (!obj) return;

		if constexpr (kHasWPtrBlock) {
			if (auto* wb = obj->_weakPtrBlock.ptr()) {
				// lock before release ref count and delete object
				auto wbData = wb->data.scopedLock();
				
				bool isDeleted = _doRelease(s);
				if (isDeleted) {
					AX_ASSERT(wbData->obj == obj);
					wbData->obj = nullptr;
				}
				return;
			}
		}

		_doRelease(s);
	}
	
private:
	static AX_INLINE constexpr bool _doRelease(SPtr<T>* s) {
		if (s->_p->_releaseSPtrRef() == 0) {
			ax_delete(s->_p);
			s->_p = nullptr;
			return true;
		}
		return false;
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
	AX_INLINE	SPtr(Tag::NewObject_, const MemAllocRequest& req, ARGS&&... args) { newObject(req, AX_FORWARD(args)...); }

	template<class R>
	AX_INLINE	SPtr(SPtr<R> && r) noexcept { _move(std::move(r)); }

	template<class R>
	AX_INLINE	SPtr(SPtr<R> &r) noexcept { ref(r.ptr()); }

	AX_INLINE	~SPtr() { unref(); }

	AX_INLINE T*	 ref	(T* p) noexcept;
	AX_INLINE void unref	() noexcept;

	AX_INLINE operator       T* () &       { return _p; }
	AX_INLINE operator const T* () const & { return _p; }

	operator T* () && = delete;

	AX_INLINE	void operator=(std::nullptr_t) noexcept	{ unref(); }
	template<class R> AX_INLINE	void operator=(SPtr<R> &  r) { ref(r.ptr()); }
	template<class R> AX_INLINE	void operator=(SPtr<R> && r) noexcept { _move(std::move(r)); }

	template<class... ARGS> AX_INLINE
	T*	newObject(const MemAllocRequest& req, ARGS&&...args) { return ref(new (req) T(AX_FORWARD(args)...)); }

	template<class R> AX_INLINE bool operator==(const SPtr<R>& r) const { return _p == r._p; }

	template<class R> AX_INLINE bool operator==(R * const r) const { return _p == r; }
	template<class R> AX_INLINE bool operator!=(R * const r) const { return _p != r; }

	template<class R> AX_INLINE bool operator!=(const SPtr<R>& r) const { return _p != r._p; }
	
	AX_INLINE static SPtr<T> s_ref(T* p) noexcept { return SPtr(p); }

	friend  class SPtr_Internal<T>;
protected:
	template<class R>
	AX_INLINE	void _move(SPtr<R> && r) noexcept;
	using Base::_p;
};

template<class R, class T> AX_INLINE bool operator==(R* const a, const SPtr<T>& b) { return b == a; }
template<class R, class T> AX_INLINE bool operator!=(R* const a, const SPtr<T>& b) { return b != a; }

template<class T> AX_INLINE bool operator==(const std::nullptr_t&, const SPtr<T>& b) { return b == nullptr; }
template<class T> AX_INLINE bool operator!=(const std::nullptr_t&, const SPtr<T>& b) { return b != nullptr; }

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
	ref(r.ptr());
	r.unref();
}


} // namespace