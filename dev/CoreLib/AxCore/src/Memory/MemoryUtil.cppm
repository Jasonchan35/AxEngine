export module AxCore.MemoryUtil;

import <cstdlib>;
import <memory>;

#include "AxBase.h"
import AxCore.BasicType;

export namespace ax {


struct MemoryUtil {
	MemoryUtil() = delete;

	static const Int k_size_to_use_system_memcpy = 2048;

	template <class A, class B>
	static bool isOverlapped(const A* a, Int a_size, const B* b, Int b_size);
	static void Copy(void* dst, const void* src, Int len);

private:
	template <class T>
	static void _copyLoop(T* dst, const T* src, Int len);
};

template< class T > AX_INLINE
void MemoryUtil::_copyLoop( T* dst, const T* src, Int len ) {
	auto* e = dst + len;
	for( ; dst < e; ++src, ++dst ) {
		*dst = *src;
	}
}


AX_INLINE
void MemoryUtil::Copy(void* dst, const void* src, Int len) {
	if( len <= 0 ) return;

	if (isOverlapped(static_cast<const char*>(dst), len, static_cast<const char*>(src), len)) {
		throw Error_BufferOverlapped(AX_SRC_LOC);
	}

	if (len > k_size_to_use_system_memcpy) {
		::memcpy( dst, src, ax_safe_cast_size_t(len) );
		return;
	}
	using Block = u64;
	const Int w = AX_SIZE_OF(Block);
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

} // namespace
