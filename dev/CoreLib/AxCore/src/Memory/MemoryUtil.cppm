export module AxCore.MemoryUtil;

import <cstdlib>;
import <memory>;

#include "AxBase.h"
import AxCore.BasicType;

export namespace ax {

struct MemoryUtil {
	MemoryUtil() = delete;

	static const Int k_size_to_use_system_memcpy = 2048;

	static void rawCopy(void* dst, const void* src, Int len);
	
	template <class A, class B>
	static bool isOverlapped(const A* a, Int a_size, const B* b, Int b_size);

	template<class T>
	static void copy(T* dst, const T* src, Int n);
	
	template< class T, class... Args>
	static void constructor( T* p, Int n, Args&&... args);

	template< class T > static void destructor(T* p, Int n);
	template< class T > static void moveConstructorAndDestructor(T* dst, T* src, Int n);
	
private:
	template <class T>
	static void _copyLoop(T* dst, const T* src, Int len);
};

template< class T > AX_INLINE
void MemoryUtil::_copyLoop(T* dst, const T* src, Int len) {
	auto* e = dst + len;
	for( ; dst < e; ++src, ++dst ) {
		*dst = *src;
	}
}

AX_INLINE
void MemoryUtil::rawCopy(void* dst, const void* src, Int len) {
	if (len <= 0) { AX_ASSERT(false); return; }

	if (isOverlapped(static_cast<const char*>(dst), len, static_cast<const char*>(src), len)) {
		throw Error_BufferOverlapped();
	}

	if (len > k_size_to_use_system_memcpy) {
		::memcpy( dst, src, ax_safe_cast_size_t(len) );
		return;
	}
	using Block = u64;
	const Int w = ax_sizeof<Block>;
	auto n = len / w;
	auto r = len % w;
	auto j = n*w;
	_copyLoop( reinterpret_cast<Block*>(dst),  reinterpret_cast<const Block*>(src),  n);
	_copyLoop( reinterpret_cast<Byte*>(dst)+j, reinterpret_cast<const Byte*>(src)+j, r);
}

template <class A, class B> AX_INLINE
bool MemoryUtil::isOverlapped(const A* a, Int a_size, const B* b, Int b_size) {
	const void* ea = a + a_size;
	const void* eb = b + b_size; 
	return (ea > b) && (eb > a);
}

template <class T, class ... Args> AX_INLINE
void MemoryUtil::constructor(T* p, Int n, Args&&... args) {
	if( n <= 0 ) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_constructor<T>(d, AX_FORWARD(args)...);
	}
}

template <class T> AX_INLINE
void MemoryUtil::destructor(T* p, Int n) {
	if (std::is_trivially_destructible_v<T>) return;
	if (n <= 0) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_destructor(d);
	}
}

template <class T> AX_INLINE
void MemoryUtil::copy(T* dst, const T* src, Int n) {
	if constexpr (std::is_trivially_copy_assignable_v<T>) {
		MemoryUtil::rawCopy(dst, src, n * ax_sizeof<T>);
	}else{
		if (n <= 0) return;
		if (MemoryUtil::isOverlapped(dst, n, src, n)) {
			throw Error_BufferOverlapped();
		}
		
		auto s = src;
		auto e = src + n ;
		for( ; s<e; ++s, ++dst ) {
			*dst = *s;
		}
	}
}

template <class T> AX_INLINE
void MemoryUtil::moveConstructorAndDestructor(T* dst, T* src, Int n) {
	if( n <= 0 ) return;
	if (MemoryUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if (std::is_trivially_copy_assignable_v<T>) {
		MemoryUtil::rawCopy(dst, src, n * ax_sizeof<T>);
	}else{
		auto s = src;
		auto e = src + n ;
		for( ; s<e; ++s, ++dst ) {
			ax_call_constructor<T>(dst, std::move(*s));
			ax_call_destructor(s);
		}
	}
}

} // namespace
