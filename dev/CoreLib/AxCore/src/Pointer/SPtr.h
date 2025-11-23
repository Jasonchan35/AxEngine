#pragma once

#include "UPtr.h"
#include "AxCore/Thread/AtomicInt.h"

namespace ax {

class SPtrReferenable : public NonCopyable {
public:
	Int addRef()const		{ return ++_refCount; }
	Int releaseRef() const	{ return --_refCount; }
	Int refCount() const	{ return _refCount; }

	virtual ~SPtrReferenable() {
		AX_ASSERT(_refCount == 0);
	}

	virtual void onRefCountZero()	{ onSPtrDeleteThis(); }

protected:
	virtual void onSPtrDeleteThis() { ax_delete(this); }

private:
	mutable AxCore::AtomicInt		_refCount;
};

//! Intrusive Shared pointer, the reference count is embedded in object
//! therefore SPtr<T> and T* can be interchangable
//! unlike std::shared_ptr<T>, when create shared_ptr<T> from T*, 
//! may create different control block to hold the reference count
//! which may cause double delete of same object
template<class T>
class SPtr : public AxCore::PtrBase<T> {
	using Base = AxCore::PtrBase<T>;
public:
	AX_INLINE	SPtr() = default;
	AX_INLINE	SPtr(std::nullptr_t) noexcept {}
	AX_INLINE	SPtr(const SPtr&  p) noexcept { ref(p._p); }
	AX_INLINE	SPtr(      SPtr&& r) noexcept { _move(std::move(r)); }
	AX_INLINE	SPtr(T* p) noexcept { ref(p); }

	template<class... ARGS>
	AX_INLINE	SPtr(Tag::NewObject, const AllocRequest& req, ARGS&&... args) { newObject(req, AX_FORWARD(args)...); }

	template<class R>
	AX_INLINE	SPtr(SPtr<R> && r) noexcept { _move(std::move(r)); }

	template<class R>
	AX_INLINE	SPtr(SPtr<R> &r) noexcept { ref(r.ptr()); }

	AX_INLINE	~SPtr() { unref(); }

	T*	 ref	(T* p) noexcept;
	void unref	() noexcept;

	operator       T* () &       { return _p; }
	operator const T* () const & { return _p; }

	operator T* () && = delete;

	AX_INLINE	void operator=(const SPtr & r) noexcept	{ ref(r._p); }
	AX_INLINE	void operator=(SPtr && r) noexcept		{ _move(std::move(r)); }
	AX_INLINE	void operator=(std::nullptr_t) noexcept	{ unref(); }

	template<class R> AX_INLINE	void operator=(SPtr<R> &  r) { ref(r.ptr()); }
	template<class R> AX_INLINE	void operator=(SPtr<R> && r) noexcept { _move(std::move(r)); }

	template<class... ARGS>
	T*	newObject(const AllocRequest& req, ARGS&&...args) {
		return ref(new (req) T(AX_FORWARD(args)...));
	}

	template<class R>
	bool operator==(const SPtr<R>& r) const { return _p == r._p; }

	template<class R> bool operator==(R * const r) const { return _p == r; }
	template<class R> bool operator!=(R * const r) const { return _p != r; }

	template<class R>
	bool operator!=(const SPtr<R>& r) const { return _p != r._p; }


	static SPtr<T> s_ref(T* p) noexcept { return SPtr(p); }

protected:
	template<class R>
	AX_INLINE	void _move(SPtr<R> && r) noexcept;

private:

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
SPtr<T> SPtr_new(const AllocRequest& req, ARGS &&... args) {
	return SPtr_ref(new (req) T(AX_FORWARD(args)...));
}

//--------------------------------

template<class T> AX_INLINE
void SPtr<T>::unref() noexcept {
	if (_p) {
		if (_p->releaseRef() == 0) {
			ax_const_cast(_p)->onRefCountZero();
		}
		_p = nullptr;
	}
}

template<class T> AX_INLINE
T* SPtr<T>::ref(T * p) noexcept {
	static_assert(std::is_base_of_v<SPtrReferenable, T>);

	if (_p != p) {
		unref();
		_p = p;
		if (p) _p->addRef();
	}
	return p;
}

template<class T>
template<class R> AX_INLINE
void SPtr<T>::_move(SPtr<R> && r) noexcept {
	if (static_cast<void*>(&r) == this) return;
	ref(r.ptr());
	r.unref();
}


} // namespace