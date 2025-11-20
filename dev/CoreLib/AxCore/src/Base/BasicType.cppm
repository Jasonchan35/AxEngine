export module AxCore.BasicType;

#include "AX_DETECT_PLATFORM.h"
#include "AX_MACRO.h"

import <stdexcept>;
import <cstdint>;
import <type_traits>;
import <cassert>;
import <exception>;
import <source_location>;

export namespace ax {

class NonCopyable {
public:
	NonCopyable()                        = default;
	NonCopyable(NonCopyable&& r)         = default;
	NonCopyable(const NonCopyable& s)    = delete;
	void operator=(const NonCopyable& s) = delete;
};

template<class T> AX_INLINE constexpr T* ax_const_cast(const T* v) { return const_cast<T*>(v); }
template<class T> AX_INLINE constexpr T& ax_const_cast(const T& v) { return const_cast<T&>(v); }

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

constexpr i8   i8_max = std::numeric_limits<i8 >::max();
constexpr i16 i16_max = std::numeric_limits<i16>::max();
constexpr i32 i32_max = std::numeric_limits<i32>::max();
constexpr i64 i64_max = std::numeric_limits<i64>::max();

constexpr i8   i8_min = std::numeric_limits<i8 >::min();
constexpr i16 i16_min = std::numeric_limits<i16>::min();
constexpr i32 i32_min = std::numeric_limits<i32>::min();
constexpr i64 i64_min = std::numeric_limits<i64>::min();

constexpr u8   u8_max = std::numeric_limits<u8 >::max();
constexpr u16 u16_max = std::numeric_limits<u16>::max();
constexpr u32 u32_max = std::numeric_limits<u32>::max();
constexpr u64 u64_max = std::numeric_limits<u64>::max();

constexpr u8   u8_min = std::numeric_limits<u8 >::min();
constexpr u16 u16_min = std::numeric_limits<u16>::min();
constexpr u32 u32_min = std::numeric_limits<u32>::min();
constexpr u64 u64_min = std::numeric_limits<u64>::min();


using f32  = float;
using f64  = double;
using f128 = long double;

// using CharA  = char;
using Char8  = char8_t;
using Char16 = char16_t;
using Char32 = char32_t;
using CharW  = wchar_t;
using Char   = Char8; // char8_t is a distinct type from char, so don't have to worry mix with u8/i8

using Int  = i64;
using UInt = u64;
using Byte = u8;

template<class T> constexpr bool ax_type_is_char =	std::is_same_v<std::remove_cv_t<T>, char>
												 ||	std::is_same_v<std::remove_cv_t<T>, CharW>
												 || std::is_same_v<std::remove_cv_t<T>, Char8>
												 || std::is_same_v<std::remove_cv_t<T>, Char16>
												 || std::is_same_v<std::remove_cv_t<T>, Char32>;

template<class T> concept CharType = ax_type_is_char<T>;

template<class T, class R>
constexpr bool ax_same_size_char_type = (sizeof(T) == sizeof(R) && ax_type_is_char<R> && ax_type_is_char<T>);

template<class DST, class SRC> requires	ax_same_size_char_type<DST, SRC>
constexpr DST* ax_cast_char_type_pointer(SRC* s) { return reinterpret_cast<DST*>(s); }

template<class T> AX_INLINE constexpr Int ax_strlen(const T* sz) {
	if (!sz) return 0;
	Int i = 0;
	for (; *sz; ++sz, ++i) {}
	return i;
}

template<class T> requires std::is_const_v<T>
class StrLit_ {
	using This = StrLit_;
public:
	StrLit_() = default;

	template<class R> requires ax_same_size_char_type<T,R>
	AX_INLINE constexpr StrLit_(R* sz, Int size) : _data(ax_cast_char_type_pointer<T>(sz)), _size(size) {}
	
	template<class R, Int N> requires ax_same_size_char_type<T,R>
	AX_INLINE constexpr StrLit_(R (&sz)[N]) : _data(ax_cast_char_type_pointer<T>(sz)), _size(N > 0 ? N-1 : 0) {}
	
	template<class R> requires ax_same_size_char_type<T,R>
	AX_INLINE static constexpr StrLit_ s_from_c_str(R* sz) { return StrLit_(reinterpret_cast<T*>(sz), sz ? ax_strlen(sz) : 0); }
	
	AX_INLINE constexpr const T* c_str() const { return _size ? _data : &_empty_c_str; }

protected:
	static constexpr T _empty_c_str = 0;	
	
	T*  _data = nullptr;
	Int _size = 0;
};

using StrLit = StrLit_<const Char>;

class NoInit {};

struct SrcLoc {
	constexpr SrcLoc(NoInit) {}
	constexpr SrcLoc(const std::source_location & loc = std::source_location::current()) : _loc(loc) {};

	constexpr Int    column() const { return _loc.column(); }
	constexpr Int    line() const { return _loc.line(); }
	constexpr StrLit file() const { return StrLit::s_from_c_str(_loc.file_name()); }
	constexpr StrLit function() const { return StrLit::s_from_c_str(_loc.function_name()); }
	
protected:
	std::source_location _loc;
};

export inline
void ax_assert(bool expr, StrLit exprStr, const SrcLoc & srcLoc = SrcLoc()) {
	if (expr) return;
	assert(false);
} 

class Error : public std::exception {
public:
	Error() = default;
	Error(const SrcLoc& srcLoc) : _srcLoc(srcLoc) {}

	virtual char const* what() const override { return _what.c_str(); }
	
protected:
	SrcLoc _srcLoc;
	std::string _what;
};

#define AX_SIMPLE_ERROR(ERROR_TYPE) \
	class ERROR_TYPE : public Error { \
	public: \
	ERROR_TYPE(const SrcLoc& srcLoc = SrcLoc()) : Error(srcLoc) {} \
	}; \
//------
AX_SIMPLE_ERROR(Error_IndexOutOfRange)
AX_SIMPLE_ERROR(Error_InvalidSize)
AX_SIMPLE_ERROR(Error_BufferOverlapped)
AX_SIMPLE_ERROR(Error_ValueCast)
AX_SIMPLE_ERROR(Error_Allocator)

template<class DST, class SRC> AX_INLINE
constexpr bool ax_try_safe_cast(DST& dst, const SRC& src) noexcept {
	if (!std::in_range<DST>(src)) return false;
	
	dst = static_cast<DST>(src);
	return true;
}

template<class DST, class SRC> AX_INLINE
constexpr DST ax_safe_cast(const SRC& src) {
	DST dst;
	if (!ax_try_safe_cast(dst, src)) {
		throw Error_ValueCast(SrcLoc());
	}
	return dst;
}

constexpr inline Int	ax_safe_cast_Int(size_t src) { return ax_safe_cast<Int>(src); }
constexpr inline size_t ax_safe_cast_size_t(Int src) { return ax_safe_cast<size_t>(src); }

template<class T> constexpr Int ax_sizeof  = ax_safe_cast<Int>(sizeof(T)); 
template<class T> constexpr Int ax_alignof = ax_safe_cast<Int>(alignof(T));

template<class T, class... Args > AX_INLINE
T* ax_call_constructor(T* p, Args&&... args ) {
	return ::new(p) T(AX_FORWARD(args)...);
}

template<class T> AX_INLINE
void ax_call_destructor(T* p ) noexcept {
	p->~T();
}

} // namespace
