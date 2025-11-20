export module AxCore.RawArrayUtil;

import AxCore.BasicType;
import AxCore.MemoryUtil;
import <type_traits>;

#include "AxBase.h"

export namespace ax {

struct RawArrayUtil {
private:
	RawArrayUtil() = delete;
public:
	template< class T, class... Args> static void constructor( T* p, Int n, Args&&... args);
	template< class T > static void destructor(T* p, Int n);
	template< class T > static void moveConstructorAndDestructor( T* dst, T* src, Int n );
};

template <class T, class ... Args> AX_INLINE
void RawArrayUtil::constructor(T* p, Int n, Args&&... args) {
	if( n <= 0 ) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_constructor<T>(d, AX_FORWARD(args)...);
	}
}

template <class T> AX_INLINE
void RawArrayUtil::destructor(T* p, Int n) {
	if (std::is_trivially_destructible_v<T>) return;
	if (n <= 0) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_destructor(d);
	}
}

template <class T> AX_INLINE
void RawArrayUtil::moveConstructorAndDestructor(T* dst, T* src, Int n) {
	if( n <= 0 ) return;
	if (MemoryUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if (std::is_trivially_copy_assignable_v<T>) {
 		MemoryUtil::Copy(dst, src, n * ax_sizeof<T>);
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
