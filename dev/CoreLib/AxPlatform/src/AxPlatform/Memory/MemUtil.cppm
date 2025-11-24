module;
#include "AxPlatform-pch.h"

export module AxPlatform.MemoryUtil;

#include "AxPlatform-pch.h"
import AxPlatform.BasicType;

export namespace ax {

struct MemUtil {
	MemUtil() = delete;

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


	template<class T> AX_INLINE
	static Int sizeInBytes(const T* start, const T* end) {
		return reinterpret_cast<const char*>(end) - reinterpret_cast<const char*>(start);
	}

	template<class T> AX_INLINE
	static T* addOffsetInBytes(T* p, Int numBytes) { 
		return reinterpret_cast<T*>(reinterpret_cast<char*>(p) + numBytes); 
	}

	template<class T> AX_INLINE 
	static const T* addOffsetInBytes(const T* p, Int numBytes) {
		return reinterpret_cast<const T*>(reinterpret_cast<const char*>(p) + numBytes);
	}

	template<class T> struct memberOffset_wrap { static T v; };

	template< class Obj, class Member > AX_INLINE
	static Int memberOffset(Member Obj::*ptrToMember) {
#if 1
		Obj* obj = nullptr;
		char* m = reinterpret_cast<char*>(&(obj->*ptrToMember));
		char* c = nullptr;
		return m - c;
#else // try constexpr, but seems not work
		using W = memberOffset_wrap<Obj>;
		char arr[reinterpret_cast<char*>(&(W::v.*ptrToMember)) - reinterpret_cast<char*>(&W::v)];
		return static_cast<IntPtr>(sizeof(arr));
#endif
	}

	template< class Obj, class Member > AX_INLINE
	static Obj* _memberOwner( Member Obj::*ptrToMember, Member* member) {
		if (!member) return nullptr;
		auto o = reinterpret_cast<char*>(member) - memberOffset(ptrToMember);
		return reinterpret_cast<Obj*>(o);
	}

	template< class Obj, class Member > AX_INLINE
	static Obj* memberOwner( Member Obj::*ptrToMember, Member* member) {
		return _memberOwner(ptrToMember, member);
	}

	template< class Obj, class Member > AX_INLINE
	static const Obj* memberOwner( Member Obj::*ptrToMember, const Member* member) {
		return _memberOwner(ptrToMember, ax_const_cast(member));
	}
	
private:
	template <class T>
	static void _copyLoop(T* dst, const T* src, Int len);
};

template< class T > AX_INLINE
void MemUtil::_copyLoop(T* dst, const T* src, Int len) {
	auto* e = dst + len;
	for( ; dst < e; ++src, ++dst ) {
		*dst = *src;
	}
}

AX_INLINE
void MemUtil::rawCopy(void* dst, const void* src, Int len) {
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
bool MemUtil::isOverlapped(const A* a, Int a_size, const B* b, Int b_size) {
	const void* ea = a + a_size;
	const void* eb = b + b_size; 
	return (ea > b) && (eb > a);
}

template <class T, class ... Args> AX_INLINE
void MemUtil::constructor(T* p, Int n, Args&&... args) {
	if( n <= 0 ) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_constructor<T>(d, AX_FORWARD(args)...);
	}
}

template <class T> AX_INLINE
void MemUtil::destructor(T* p, Int n) {
	if (std::is_trivially_destructible_v<T>) return;
	if (n <= 0) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_destructor(d);
	}
}

template <class T> AX_INLINE
void MemUtil::copy(T* dst, const T* src, Int n) {
	if (n <= 0) return;
	if (MemUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if (std::is_trivially_copy_assignable_v<T>) {
		MemUtil::rawCopy(dst, src, n * ax_sizeof<T>);
	}else{
		auto s = src;
		auto e = src + n ;
		for( ; s<e; ++s, ++dst ) {
			*dst = *s;
		}
	}
}

template <class T> AX_INLINE
void MemUtil::moveConstructorAndDestructor(T* dst, T* src, Int n) {
	if( n <= 0 ) return;
	if (MemUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if (std::is_trivially_copy_assignable_v<T>) {
		MemUtil::rawCopy(dst, src, n * ax_sizeof<T>);
	}else{
		try {
			auto s = src;
			auto e = src + n ;
			for( ; s<e; ++s, ++dst ) {
				ax_call_constructor<T>(dst, std::move(*s));
				ax_call_destructor(s);
			}
		} catch	(...) {
			AX_ASSERT(false);
			// TODO: catch exception and rewind the move here before re-throw
			throw;
		}
	}
}

} // namespace
