module;

#include "AxCore-pch.h"
export module AxCore.BasicType;

export import AxCore.pch;

export namespace ax {

class NonCopyable {
public:
	constexpr NonCopyable()                        = default;
	constexpr NonCopyable(NonCopyable&& r)         = default;
	constexpr NonCopyable(const NonCopyable& s)    = delete;
	constexpr void operator=(const NonCopyable& s) = delete;
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

using Int  = i64;
using UInt = u64;
using Byte = u8;

using f32  = float;
using f64  = double;
using f128 = long double;

using CharA  = char;
using Char8  = char8_t;
using Char16 = char16_t;
using Char32 = char32_t;
using CharW  = wchar_t;
using Char   = CharA;
using CharU	 = Char32;

template<class T> constexpr bool ax_type_is_char =	std::is_same_v<std::remove_cv_t<T>, CharA>
												 ||	std::is_same_v<std::remove_cv_t<T>, CharW>
												 || std::is_same_v<std::remove_cv_t<T>, Char8>
												 || std::is_same_v<std::remove_cv_t<T>, Char16>
												 || std::is_same_v<std::remove_cv_t<T>, Char32>;

template<class T> concept CharType = ax_type_is_char<T>;

template<class T> AX_INLINE constexpr Int ax_strlen(const T* sz) {
	if (!sz) return 0;
	Int i = 0;
	for (; *sz; ++sz, ++i) { /* nothing */ }
	return i;
}

template<class T> requires std::is_const_v<T>
class StrLit_ {
	using This = StrLit_;
public:
	StrLit_() = default;

	AX_INLINE constexpr StrLit_(T* sz, Int size) : _data(sz), _size(size) {}
	
	template<Int N>
	AX_INLINE constexpr StrLit_(T (&sz)[N]) : _data(sz), _size(N > 0 ? N-1 : 0) {}
	
	AX_INLINE static constexpr StrLit_ s_from_c_str(T* sz) { return StrLit_(sz, sz ? ax_strlen(sz) : 0); }
	
	AX_INLINE constexpr const T* c_str() const { return _size ? _data : &_empty_c_str; }

protected:
	static constexpr T _empty_c_str = 0;	
	
	T*  _data = nullptr;
	Int _size = 0;
};

using StrLit = StrLit_<const Char>;

namespace Tag {
	class NewObject {};
	class NoInit {};
} // Tag

struct SrcLoc {
	constexpr SrcLoc(Tag::NoInit) {}
	constexpr SrcLoc(const std::source_location & loc = std::source_location::current()) : _loc(loc) {};

	constexpr Int    column() const { return _loc.column(); }
	constexpr Int    line() const { return _loc.line(); }
	constexpr StrLit file() const { return StrLit::s_from_c_str(_loc.file_name()); }
	constexpr StrLit function() const { return StrLit::s_from_c_str(_loc.function_name()); }
	
protected:
	std::source_location _loc;
};

inline void ax_assert(bool expr, StrLit exprStr, const SrcLoc & srcLoc = SrcLoc()) {
	if (expr) return;
	assert(false);
}

template<class T>
struct WithName {
	WithName(T& value_, const char* name_) : value(value_), name(name_) {}
	T& value;
	const char* name;
};

class Error : public std::exception {
public:
	Error() = default;
	Error(const SrcLoc& srcLoc) : _srcLoc(srcLoc) {}

	virtual char const* what() const override { return _what.c_str(); }
	
protected:
	SrcLoc _srcLoc;
	std::string _what;
};

AX_SIMPLE_ERROR(Error_IndexOutOfRange)
AX_SIMPLE_ERROR(Error_InvalidSize)
AX_SIMPLE_ERROR(Error_BufferOverlapped)
AX_SIMPLE_ERROR(Error_ValueCast)
AX_SIMPLE_ERROR(Error_Allocator)
AX_SIMPLE_ERROR(Error_Format)
AX_SIMPLE_ERROR(Error_Utf)

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

template<Int N>	struct CharW_Native_;
template<>	struct CharW_Native_<ax_sizeof<Char16>> { using Type = Char16; };
template<>	struct CharW_Native_<ax_sizeof<Char32>> { using Type = Char32; };
using CharW_Native = typename CharW_Native_<ax_sizeof<CharW>>::Type;

AX_INLINE			CharW_Native	CharW_to_Native(      CharW   v) { return static_cast<           CharW_Native  >(v); }
AX_INLINE			CharW_Native&	CharW_to_Native(      CharW&  v) { return reinterpret_cast<      CharW_Native& >(v); }
AX_INLINE			CharW_Native*	CharW_to_Native(      CharW*  v) { return reinterpret_cast<      CharW_Native* >(v); }
AX_INLINE			CharW_Native**	CharW_to_Native(      CharW** v) { return reinterpret_cast<      CharW_Native**>(v); }
AX_INLINE	const	CharW_Native&	CharW_to_Native(const CharW&  v) { return reinterpret_cast<const CharW_Native& >(v); }
AX_INLINE	const	CharW_Native*	CharW_to_Native(const CharW*  v) { return reinterpret_cast<const CharW_Native* >(v); }
AX_INLINE	const	CharW_Native**	CharW_to_Native(const CharW** v) { return reinterpret_cast<const CharW_Native**>(v); }


// this cast can over come function pointer to void*
template<class DST, class SRC> AX_INLINE
constexpr DST ax_bit_cast(const SRC& src) {
	// std::bit_cast needs c++20, so use union work around
	union Wrap {
		constexpr Wrap(const SRC& src_) : src(src_) {}
		DST dst;
		SRC src;
	};
	static_assert(sizeof(DST) == sizeof(SRC));
	return Wrap(src).dst;
}

template<class T> using Opt = std::optional<T>;

template<Int... ints> using IntSequence = std::integer_sequence<Int, ints...>;
template<Int N> using IntSequence_make = std::make_integer_sequence<Int, N>;


template<class T>
class PtrBase {
public:
	PtrBase(T* p = nullptr) : _p(p) {}
	~PtrBase() { _p = nullptr; }

	//	AX_INLINE 		T* ptr() && = delete;

	AX_INLINE 		T* ptr()				{ return  _p; }
	AX_INLINE const	T* ptr() const			{ return  _p; }

	AX_INLINE 		T* get()				{ return  _p; }
	AX_INLINE const	T* get() const			{ return  _p; }

	AX_INLINE 		T* operator->()			{ return  _p; }
	AX_INLINE const	T* operator->() const	{ return  _p; }

	AX_INLINE 		T& operator* ()			{ return *_p; }
	AX_INLINE const	T& operator* () const	{ return *_p; }

	AX_INLINE explicit operator bool() const	{ return _p != nullptr; }

protected:
	T* _p;
};

template<class T> struct NumLimit;

template<class T> requires std::is_integral_v<T> || std::is_floating_point_v<T> || CharType<T>
struct NumLimit<T> {
	AX_INLINE static constexpr T kDefaultValue	() { return 0; }
	AX_INLINE static constexpr T kLowest		() { return  std::numeric_limits<T>::lowest();    }
	AX_INLINE static constexpr T kMin			() { return  std::numeric_limits<T>::min();		  }
	AX_INLINE static constexpr T kMax			() { return  std::numeric_limits<T>::max();		  }
	AX_INLINE static constexpr T kInfinity		() { return  std::numeric_limits<T>::infinity();  }
	AX_INLINE static constexpr T kNegInfinity	() { return -std::numeric_limits<T>::infinity();  }
	AX_INLINE static constexpr T kNaN			() { return  std::numeric_limits<T>::quiet_NaN(); }
};


} // namespace
