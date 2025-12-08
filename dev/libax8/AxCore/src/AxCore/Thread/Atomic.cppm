module;
#include "AxCore-pch.h"

export module AxCore.Atomic;

import AxCore.BasicType;

export namespace ax::Thread {

class AtomicFlag : public NonCopyable {
	using AtomicType = ::std::atomic_flag;
public:
	AX_INLINE bool testAndSet()		{ return _v.test_and_set(); }
	AX_INLINE void clear()			{ _v.clear(); }
private:
	AtomicType _v = ATOMIC_FLAG_INIT;
};

template< class T >
class AtomicInt_ : public NonCopyable {
	 using AtomicType = std::atomic<T>;
public:
	AX_INLINE AtomicInt_(T v = T()) : _v(v) {}

	AX_INLINE  void	set	( const T& v )  { _v.store(v); }
	AX_INLINE	T	get	() const		{ return _v.load(); }

	AX_INLINE	void	operator=	( const T& v )	{ set(v); }
	AX_INLINE			operator T	() const		{ return get(); }

	AX_INLINE	T		operator++	() 				{ return ++_v; }
	AX_INLINE	T		operator--	()				{ return --_v; }

private:
	AtomicType _v;
};

template<class T>
class AtomicPtr : public NonCopyable {
	using AtomicType = std::atomic<T*>;
public:
	AX_INLINE AtomicPtr(T* v = nullptr) : _v(v) {}

	AX_INLINE void     set(const T& v) { _v.store(v); }
	AX_INLINE T*       get() { return _v.load(); }
	AX_INLINE const T* get() const { return _v.load(); }

	AX_INLINE void operator=(const T& v) { set(v); }
	AX_INLINE      operator T() { return get(); }
	AX_INLINE      operator const T*() const { return get(); }

private:
	AtomicType _v;
};

using AtomicInt		= AtomicInt_<Int>;
using AtomicInt8	= AtomicInt_<i8>;
using AtomicInt16	= AtomicInt_<i16>;
using AtomicInt32	= AtomicInt_<i32>;
using AtomicInt64	= AtomicInt_<i64>;

using AtomicUInt	= AtomicInt_<UInt>;
using AtomicUInt8	= AtomicInt_<u8>;
using AtomicUInt16	= AtomicInt_<u16>;
using AtomicUInt32	= AtomicInt_<u32>;
using AtomicUInt64	= AtomicInt_<u64>;

} // namespace