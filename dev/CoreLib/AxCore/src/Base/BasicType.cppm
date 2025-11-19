export module AxCore.BasicType;

#include "AX_DETECT_PLATFORM.h"
#include "AX_MACRO.h"

import <stdexcept>;
import <cstdint>;
import <type_traits>;
import <cassert>;
import <exception>;

export namespace ax {

class NonCopyable {
public:
	NonCopyable()                        = default;
	NonCopyable(NonCopyable&& r)         = default;
	NonCopyable(const NonCopyable& s)    = delete;
	void operator=(const NonCopyable& s) = delete;
};

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

using CharA  = char;
using Char8  = char8_t;
using Char16 = char16_t;
using Char32 = char32_t;
using CharW  = wchar_t;
using Char   = Char8; 

inline const Char8* ax_char_pointer(const CharA* p) {
	static_assert(sizeof(Char8) == sizeof(CharA));
	return reinterpret_cast<const Char8*>(p);
}

using Int  = i64;
using UInt = u64;
using Byte = u8;

template<class T> constexpr bool ax_type_is_char = std::is_same_v<std::remove_cv_t<T>, CharA>
												|| std::is_same_v<std::remove_cv_t<T>, CharW>
												|| std::is_same_v<std::remove_cv_t<T>, Char8>
												|| std::is_same_v<std::remove_cv_t<T>, Char16>
												|| std::is_same_v<std::remove_cv_t<T>, Char32>;

template<class T> concept CharType = ax_type_is_char<T>;

template<class T> requires std::is_const_v<T>
class InternalStrLit_ {
public:
	InternalStrLit_() = default;
	
	template<Int N>
	inline constexpr InternalStrLit_(T (&sz)[N])
	: _data(sz), _size(N > 0 ? N-1 : 0) {}
	
protected:
	T* _data = nullptr;
	Int _size = 0;
};

using InternalStrLit = InternalStrLit_<const CharA>;

struct SrcLoc {
	SrcLoc() = default;
	SrcLoc(InternalStrLit file, int line) : _file(file), _line(line) {}

	InternalStrLit file() const { return _file; }
	int line() const { return _line; }
	
protected:
	InternalStrLit _file;
	int _line  = 0;
};

export inline
void ax_assert(const SrcLoc & srcLoc, bool expr, InternalStrLit exprStr) {
	if (expr) return;
	assert(false);
} 

class Error : public std::exception {
public:
	Error() = default;
	Error(const SrcLoc& srcLoc) : _srcLoc(srcLoc) {}

protected:
	SrcLoc _srcLoc;
};

#define AX_SIMPLE_ERROR(ERROR_TYPE) \
	class ERROR_TYPE : public Error { \
	public: \
	ERROR_TYPE(const SrcLoc& srcLoc) : Error(srcLoc) {} \
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
		throw Error_ValueCast(AX_SRC_LOC);
	}
	return dst;
}

constexpr inline Int	ax_safe_cast_Int(size_t src) { return ax_safe_cast<Int>(src); }
constexpr inline size_t ax_safe_cast_size_t(Int src) { return ax_safe_cast<size_t>(src); }

template<class T, class... Args > AX_INLINE
T* ax_call_constructor(T* p, Args&&... args ) {
	return ::new(p) T(AX_FORWARD(args)...);
}

template<class T> AX_INLINE
void ax_call_destructor(T* p ) noexcept {
	p->~T();
}

} // namespace
