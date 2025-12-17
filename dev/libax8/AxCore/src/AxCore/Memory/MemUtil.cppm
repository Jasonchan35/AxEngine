module;


export module AxCore.MemUtil;
export import AxCore.SafeCast;

export namespace ax {

struct MemUtil {
	MemUtil() = delete;

	static constexpr const Int k_size_to_use_system_memcpy = 2048;

	static constexpr void rawCopy(void* dst, const void* src, Int n);
	
	template <class A, class B>
	static constexpr bool isOverlapped(const A* a, Int a_size, const B* b, Int b_size);

	template<class T>
	static constexpr void copy(T* dst, const T* src, Int n);
	
	template<class T, class... ARGS>
	static constexpr void constructor( T* p, Int n, ARGS&&... args);

	template<class T> static constexpr void destructor(T* p, Int n);

	template<class T> static constexpr void copyConstructor(T* dst, const T* src, Int n);
	template<class T> static constexpr void moveConstructor(T* dst, T* src, Int n);
	template<class T> static constexpr void moveConstructorAndDestructor(T* dst, T* src, Int n);

	template<class T, class R, class FUNC>
	static constexpr void copyConstructorFunc(T* dst, const R* src, Int n, FUNC func);
	
	template<class T> static constexpr bool equals(const T* dst, const T* src, Int n);

	template<class T> AX_INLINE
	static constexpr Int sizeInBytes(const T* start, const T* end) {
		return reinterpret_cast<const char*>(end) - reinterpret_cast<const char*>(start);
	}

	template<class T> AX_INLINE
	static constexpr T* addOffsetInBytes(T* p, Int numBytes) { 
		return reinterpret_cast<T*>(reinterpret_cast<char*>(p) + numBytes); 
	}

	template<class T> AX_INLINE 
	static constexpr const T* addOffsetInBytes(const T* p, Int numBytes) {
		return reinterpret_cast<const T*>(reinterpret_cast<const char*>(p) + numBytes);
	}

	template< class Obj, class Member > AX_INLINE
	static constexpr Int memberOffset(Member Obj::*ptrToMember) {
		Obj* obj = nullptr;
		char* m = reinterpret_cast<char*>(&(obj->*ptrToMember));
		char* c = nullptr;
		return m - c;
	}

	template< class OBJ, class MEMBER > AX_INLINE
	static constexpr OBJ* _memberOwner( MEMBER OBJ::*ptrToMember, MEMBER* member) {
		if (!member) return nullptr;
		auto o = reinterpret_cast<char*>(member) - memberOffset(ptrToMember);
		return reinterpret_cast<OBJ*>(o);
	}

	template< class OBJ, class EMBER > AX_INLINE
	static constexpr OBJ* memberOwner( EMBER OBJ::*ptrToMember, EMBER* member) {
		return _memberOwner(ptrToMember, member);
	}

	template< class OBJ, class MEMBER > AX_INLINE
	static constexpr const OBJ* memberOwner( MEMBER OBJ::*ptrToMember, const MEMBER* member) {
		return _memberOwner(ptrToMember, ax_const_cast(member));
	}
	
private:
	template <class T>
	static constexpr void _copyLoop(T* dst, const T* src, Int len);
};

template< class T > AX_INLINE constexpr
void MemUtil::_copyLoop(T* dst, const T* src, Int len) {
	auto* e = dst + len;
	for( ; dst < e; ++src, ++dst ) {
		*dst = *src;
	}
}

AX_INLINE constexpr
void MemUtil::rawCopy(void* dst, const void* src, Int n) {
	if (!std::is_constant_evaluated()) { AX_ASSERT(n >= 0); }
	if (n <= 0) return;

	if (isOverlapped(static_cast<const char*>(dst), n, static_cast<const char*>(src), n)) {
		throw Error_BufferOverlapped();
	}

	if (n > k_size_to_use_system_memcpy) {
		::memcpy( dst, src, ax_safe_cast_from(n) );
		return;
	}
	using Block = u64;
	constexpr Int w = AX_SIZEOF(Block);
	auto c = n / w;
	auto r = n % w;
	auto j = c * w;
	_copyLoop( static_cast<Block*>(dst),  static_cast<const Block*>(src),  c);
	_copyLoop( static_cast<Byte*>(dst)+j, static_cast<const Byte*>(src)+j, r);
}

template <class A, class B> AX_INLINE constexpr
bool MemUtil::isOverlapped(const A* a, Int a_size, const B* b, Int b_size) {
	const void* ea = a + a_size;
	const void* eb = b + b_size; 
	return (ea > b) && (eb > a);
}

template <class T, class ... ARGS> AX_INLINE constexpr
void MemUtil::constructor(T* p, Int n, ARGS&&... args) {
	if (!std::is_constant_evaluated()) { AX_ASSERT(n >= 0); }
	if (n <= 0) return;
	
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_constructor<T>(d, AX_FORWARD(args)...);
	}
}

template <class T> AX_INLINE constexpr
void MemUtil::destructor(T* p, Int n) {
	AX_ASSERT(n >= 0);
	if (n <= 0) return;
	
	if constexpr (Type_IsTriviallyDestructible<T>) return;
	T* d = p;
	T* e = p + n;
	for( ; d<e; ++d ) {
		ax_call_destructor(d);
	}
}

template <class T> AX_INLINE constexpr
void MemUtil::copy(T* dst, const T* src, Int n) {
	if (!std::is_constant_evaluated()) { AX_ASSERT(n >= 0); }
	if (n <= 0) return;
	
	if (MemUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if constexpr (Type_IsTriviallyCopyAssignable<T>) {
		MemUtil::rawCopy(dst, src, n * AX_SIZEOF(T));
	}else{
		auto s = src;
		auto e = src + n ;
		for( ; s<e; ++s, ++dst ) {
			*dst = *s;
		}
	}
}

template <class T>
constexpr void MemUtil::copyConstructor(T* dst, const T* src, Int n) {
	if (!std::is_constant_evaluated()) { AX_ASSERT(n >= 0); }
	if (n <= 0) return;
	
	if (MemUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if constexpr (Type_IsTriviallyCopyAssignable<T>) {
		MemUtil::rawCopy(dst, src, n * AX_SIZEOF(T));
	}else{
		auto s = src;
		auto e = src + n ;
		try {
			for (; s<e; ++s, ++dst) {
				ax_call_constructor<T>(dst, *s);
			}
		} catch	(...) {
			AX_ASSERT(false);
			throw;
		}
	}	
}

template <class T>
constexpr void MemUtil::moveConstructor(T* dst, T* src, Int n) {
	if( n <= 0 ) return;
	if (isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if constexpr (Type_IsTriviallyCopyAssignable<T>) {
		MemUtil::rawCopy(dst, src, n);
	}else{
		auto s = src;
		auto e = src + n ;
		for( ; s<e; ++s, ++dst ) {
			ax_call_constructor(dst, std::move(*s));
		}
	}
}

template <class T> constexpr
void MemUtil::moveConstructorAndDestructor(T* dst, T* src, Int n) {
	if (!std::is_constant_evaluated()) { AX_ASSERT(n >= 0); }
	if (n <= 0) return;
	
	if (MemUtil::isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	if constexpr (Type_IsTriviallyCopyAssignable<T>) {
		MemUtil::rawCopy(dst, src, n * AX_SIZEOF(T));
	}else{
		try {
			auto* s = src;
			auto* e = src + n ;
			auto* d = dst;
			for( ; s<e; ++s, ++d ) {
				ax_call_constructor<T>(d, std::move(*s));
				ax_call_destructor(s);
			}
		} catch	(...) {
			AX_ASSERT(false);
			// TODO: catch exception and rewind the move here before re-throw
			throw;
		}
	}
}

template <class T, class R, class FUNC>
constexpr void MemUtil::copyConstructorFunc(T* dst, const R* src, Int n, FUNC func) {
	if( n <= 0 ) return;
	if (isOverlapped(dst, n, src, n)) {
		throw Error_BufferOverlapped();
	}

	auto s = src;
	auto e = src + n ;
	for( ; s<e; ++s, ++dst ) {
		ax_call_constructor(dst, func(*s));
	}
}

template <class T>
constexpr bool MemUtil::equals(const T* dst, const T* src, Int n) {
	if (!std::is_constant_evaluated()) { AX_ASSERT(n >= 0); }
	if (n <= 0) return true;
	
	auto* s = src;
	auto* e = src + n;
	auto* d = dst;
	for (; s<e; ++s, ++d) {
		if (*s != *d) return false;
	}
	return true;
}

} // namespace
