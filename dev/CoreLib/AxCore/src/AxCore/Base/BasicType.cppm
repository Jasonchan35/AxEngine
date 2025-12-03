module;

#include "AxCore-pch.h"
export module AxCore.BasicType;
export import AxCore._PCH;

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

using f32  = float;
using f64  = double;
using f128 = long double;

using Byte  = u8;
using Int   = i64;
using UInt  = u64;
using Float = f64;

using CharA  = char;
using Char8  = char8_t;
using Char16 = char16_t;
using Char32 = char32_t;
using CharW  = wchar_t;
using Char   = CharA;
using CharU	 = Char32;

template<class T> constexpr bool Type_IsFundamental =  std::is_fundamental_v<T>;
template<class T> constexpr bool Type_IsInt         =  std::is_integral_v<T>;
template<class T> constexpr bool Type_IsSInt        =  std::is_integral_v<T> && std::is_signed_v<T>;
template<class T> constexpr bool Type_IsUInt        =  std::is_integral_v<T> && std::is_unsigned_v<T>;
template<class T> constexpr bool Type_IsFloat       =  std::is_floating_point_v<T>;
template<class T> constexpr bool Type_IsChar        =  std::is_same_v<std::remove_cv_t<T>, CharA>
                                                    || std::is_same_v<std::remove_cv_t<T>, CharW>
                                                    || std::is_same_v<std::remove_cv_t<T>, Char8>
                                                    || std::is_same_v<std::remove_cv_t<T>, Char16>
                                                    || std::is_same_v<std::remove_cv_t<T>, Char32>;

template<class T> constexpr bool Type_Is_f32        =  std::is_same_v<T, f32>;
template<class T> constexpr bool Type_Is_f64        =  std::is_same_v<T, f64>;

//--------
template<Int N>		struct	Type_Int_BySize_Struct;
template<>			struct	Type_Int_BySize_Struct<1> { using Type = i8 ; };
template<>			struct	Type_Int_BySize_Struct<2> { using Type = i16; };
template<>			struct	Type_Int_BySize_Struct<4> { using Type = i32; };
template<>			struct	Type_Int_BySize_Struct<8> { using Type = i64; };
template<Int N>		using	Type_Int_BySize = typename Type_Int_BySize_Struct<N>::Type;

template<Int N>		struct	Type_UInt_BySize_Struct;
template<>			struct	Type_UInt_BySize_Struct<1> { using Type = u8 ; };
template<>			struct	Type_UInt_BySize_Struct<2> { using Type = u16; };
template<>			struct	Type_UInt_BySize_Struct<4> { using Type = u32; };
template<>			struct	Type_UInt_BySize_Struct<8> { using Type = u64; };
template<Int N>		using	Type_UInt_BySize = typename Type_UInt_BySize_Struct<N>::Type;

template<Int N>		struct	Type_Char_BySize_Struct;
template<>			struct	Type_Char_BySize_Struct<1> { using Type = CharA ; };
template<>			struct	Type_Char_BySize_Struct<2> { using Type = Char16; };
template<>			struct	Type_Char_BySize_Struct<4> { using Type = Char32; };
template<Int N>		using	Type_Char_BySize =typename  Type_Char_BySize_Struct<N>::Type;

template<Int N>		struct	Type_Float_BySize_Struct;
template<>			struct	Type_Float_BySize_Struct<4 > { using Type = f32;  };
template<>			struct	Type_Float_BySize_Struct<8 > { using Type = f64;  };
template<>			struct	Type_Float_BySize_Struct<16> { using Type = f128; };
template<Int N>		using	Type_Float_BySize = typename Type_Float_BySize_Struct<N>::Type;

//-----------------------
template<class T>	struct	Type_Float_From_Struct;
template<>			struct	Type_Float_From_Struct<i8  > { using Type = f32;  };
template<>			struct	Type_Float_From_Struct<i16 > { using Type = f32;  };
template<>			struct	Type_Float_From_Struct<i32 > { using Type = f64;  };
template<>			struct	Type_Float_From_Struct<i64 > { using Type = f64;  };
template<>			struct	Type_Float_From_Struct<u8  > { using Type = f32;  };
template<>			struct	Type_Float_From_Struct<u16 > { using Type = f32;  };
template<>			struct	Type_Float_From_Struct<u32 > { using Type = f64;  };
template<>			struct	Type_Float_From_Struct<u64 > { using Type = f64;  };
template<>			struct	Type_Float_From_Struct<f32 > { using Type = f32;  };
template<>			struct	Type_Float_From_Struct<f64 > { using Type = f64;  };
template<>			struct	Type_Float_From_Struct<f128> { using Type = f128; };
template<class T>	using	Type_Float_From = typename Type_Float_From_Struct<T>::Type;

template<class T>	struct	Type_SInt_From_Struct;
template<>			struct	Type_SInt_From_Struct<i8 > { using Type = i8;  };
template<>			struct	Type_SInt_From_Struct<i16> { using Type = i16; };
template<>			struct	Type_SInt_From_Struct<i32> { using Type = i32; };
template<>			struct	Type_SInt_From_Struct<i64> { using Type = i64; };
template<>			struct	Type_SInt_From_Struct<u8 > { using Type = i8;  };
template<>			struct	Type_SInt_From_Struct<u16> { using Type = i16; };
template<>			struct	Type_SInt_From_Struct<u32> { using Type = i32; };
template<>			struct	Type_SInt_From_Struct<u64> { using Type = i64; };
template<class T>	using	Type_SInt_From = typename Type_SInt_From_Struct<T>::Type;

template<class T>	struct	Type_UInt_From_Struct;
template<>			struct	Type_UInt_From_Struct<i8 > { using Type = u8;  };
template<>			struct	Type_UInt_From_Struct<i16> { using Type = u16; };
template<>			struct	Type_UInt_From_Struct<i32> { using Type = u32; };
template<>			struct	Type_UInt_From_Struct<i64> { using Type = u64; };
template<>			struct	Type_UInt_From_Struct<u8 > { using Type = u8;  };
template<>			struct	Type_UInt_From_Struct<u16> { using Type = u16; };
template<>			struct	Type_UInt_From_Struct<u32> { using Type = u32; };
template<>			struct	Type_UInt_From_Struct<u64> { using Type = u64; };
template<class T>	using	Type_UInt_From = typename Type_UInt_From_Struct<T>::Type;

template<class T> AX_INLINE constexpr Int ax_strlen(const T* sz) {
	if (!sz) return 0;
	Int i = 0;
	for (; *sz; ++sz, ++i) { /* nothing */ }
	return i;
}

template<class T>
class MutStrLit_ {
	using This = MutStrLit_;
public:
	
	MutStrLit_() noexcept = default;

	AX_INLINE constexpr MutStrLit_(T* sz, Int size) noexcept : _data(sz), _size(size) {}
	
	template<Int N>
	AX_INLINE constexpr MutStrLit_(T (&sz)[N]) noexcept : _data(sz), _size(N > 0 ? N-1 : 0) {}
	
	AX_INLINE static constexpr MutStrLit_ s_from_c_str(T* sz) noexcept { return MutStrLit_(sz, sz ? ax_strlen(sz) : 0); }
	
	AX_INLINE constexpr const T* c_str() const noexcept { return _size ? _data : &_empty_c_str; }
	AX_INLINE constexpr T*  data() const noexcept { return _data; }
	AX_INLINE constexpr Int size() const noexcept { return _size; }

protected:
	static constexpr T _empty_c_str = 0;	
	T*  _data = nullptr;
	Int _size = 0;
};

template<class T> using StrLit_ = MutStrLit_<const T>;
using StrLit   = StrLit_<Char>;
using StrLitA  = StrLit_<CharA >;
using StrLitW  = StrLit_<CharW >;
using StrLit8  = StrLit_<Char8 >;
using StrLit16 = StrLit_<Char16>;
using StrLit32 = StrLit_<Char32>;

namespace Tag {
	class NewObject_{};			inline constexpr NewObject_ 		NewObject = {};
	class NoInit_{};			inline constexpr NoInit_			NoInit = {};
	class All_{};				inline constexpr All_				All = {};
} // namespace Tag

struct SrcLoc {
	constexpr SrcLoc(Tag::NoInit_) noexcept {}
	constexpr SrcLoc(const std::source_location & loc = std::source_location::current()) noexcept : _loc(loc) {};
	
	constexpr Int    column() const noexcept { return _loc.column(); }
	constexpr Int    line() const noexcept { return _loc.line(); }
	constexpr StrLit file() const noexcept { return StrLit::s_from_c_str(_loc.file_name()); }
	constexpr StrLit function() const noexcept { return StrLit::s_from_c_str(_loc.function_name()); }
	
protected:
	std::source_location _loc;
};

enum class StrCase : u8 {
	Ignore,
	Sensitive,
};

template<class A, class B> using Type_KeepConst = std::conditional_t< std::is_const_v<A>, const B, B>;


template<class T> requires Type_IsChar<T>
AX_INLINE bool ax_char_ignore_case_equals(const T& a, const T& b) { return std::tolower(a) == std::tolower(b); }

enum class CmpResult : u8 {
	Equal,
	Greater,
	Lesser,
};

AX_INLINE constexpr CmpResult CmpResult_fromInt(Int v) {
	return v == 0 ? CmpResult::Equal
	      : v > 0 ? CmpResult::Greater
	              : CmpResult::Lesser; 
}

AX_INLINE constexpr bool CmpResult_isEqual         (CmpResult a) { return a == CmpResult::Equal;   }
AX_INLINE constexpr bool CmpResult_isLesser        (CmpResult a) { return a == CmpResult::Lesser;  }
AX_INLINE constexpr bool CmpResult_isLesserOrEqual (CmpResult a) { return a == CmpResult::Lesser  || a == CmpResult::Equal; }
AX_INLINE constexpr bool CmpResult_isGreater       (CmpResult a) { return a == CmpResult::Greater; }
AX_INLINE constexpr bool CmpResult_isGreaterOrEqual(CmpResult a) { return a == CmpResult::Greater || a == CmpResult::Equal; }

inline void ax_assert(bool expr, StrLit exprStr, const SrcLoc & srcLoc = SrcLoc()) {
	if (expr) return;
	assert(false);
}

template<class A, class B> using Pair = std::pair<A, B>;
template<class A, class B> constexpr auto Pair_make(A && a, B && b) { return Pair(AX_FORWARD(a), AX_FORWARD(b)); }

template<void (*FUNC)()>
class ScopeFunc0 : public NonCopyable {
public:
	AX_NODISCARD ScopeFunc0() : _valid(1) {}
	ScopeFunc0(ScopeFunc0 && r) { std::swap(_valid, r._valid); }
	~ScopeFunc0() { if (_valid) (*FUNC)(); }
private:
	u8 _valid = false;
};

template<class PARAM0, void (*FUNC)(PARAM0)>
class ScopeFunc1 : public NonCopyable {
public:
	AX_NODISCARD ScopeFunc1(PARAM0 && param0) : _valid(1), _param0(param0) {}
	ScopeFunc1(ScopeFunc1 && r) { std::swap(_valid, r._valid); std::swap(_param0, r._param0); }
	~ScopeFunc1() { if (_valid) (*FUNC)(std::move(_param0)); }
private:
	u8		_valid = false;
	PARAM0	_param0 = {};
};

template<class OBJ, void (OBJ::*FUNC)()>
class ScopeObjFunc0 : public NonCopyable {
public:
	AX_NODISCARD ScopeObjFunc0(OBJ* obj) : _obj(obj) {}
	ScopeObjFunc0(ScopeObjFunc0 && r) { std::swap(_obj, r._obj); }
	~ScopeObjFunc0() { if (_obj) (_obj->*FUNC)(); }
private:
	OBJ*	_obj = nullptr;
};

template<class OBJ, class PARAM0, void (OBJ::*FUNC)(PARAM0)>
class ScopeObjFunc1 : public NonCopyable {
public:
	AX_NODISCARD ScopeObjFunc1(OBJ* obj, PARAM0 && param0) : _obj(obj), _param0(param0) {}
	ScopeObjFunc1(ScopeObjFunc1 && r) { std::swap(_obj, r._obj); std::swap(_param0, r._param0); }
	~ScopeObjFunc1() { if (_obj) (_obj->*FUNC)(_param0); }
private:
	OBJ*	_obj = nullptr;
	PARAM0	_param0 = {};
};

template<class T>
class ScopeValue {
public:
	AX_NODISCARD ScopeValue(T* p) { save(p); }
	AX_NODISCARD ScopeValue(T* p, const T& newValue) { save(p); if(p) *p = newValue; }
	AX_NODISCARD ScopeValue(ScopeValue && r) { std::swap(_p, r._p); std::swap(_backup, r._backup); }
	~ScopeValue() { restore(); }

	void save(T* newPtr) {
		if (newPtr == _p) return;
		restore();
		_p = newPtr;
		if (newPtr) { _backup = *newPtr; }
	}

	void restore() { if (_p) { *_p = _backup; _p = nullptr; } }
	void detach() { _p = nullptr; }

private:
	T  _backup;
	T* _p = nullptr;
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
AX_SIMPLE_ERROR(Error_ParseString)

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

consteval Int ax_const_eval_Int(size_t v) { return ax_safe_cast_Int(v); } 

template<class T, class... Args > AX_INLINE
T* ax_call_constructor(T* p, Args&&... args ) {
	return ::new(p) T(AX_FORWARD(args)...);
}

template<class T> AX_INLINE
void ax_call_destructor(T* p ) noexcept {
	p->~T();
}

template<Int N>	struct CharW_Native_;
template<>	struct CharW_Native_<AX_SIZEOF(Char16)> { using Type = Char16; };
template<>	struct CharW_Native_<AX_SIZEOF(Char32)> { using Type = Char32; };
using CharW_Native = typename CharW_Native_<AX_SIZEOF(CharW)>::Type;

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

template<class A,   class  B> inline constexpr bool Type_IsSame   = std::is_same_v<A, B>;
template<class BASE, class T> inline constexpr bool Type_IsBaseOf = std::is_base_of_v<BASE, T>;

template<class T> struct NumLimit_Struct {
	using Type = typename T::_NumLimit;
};

template<class T>
struct NumLimit_FundamentalType {
	static constexpr bool isExactType    =  std::numeric_limits<T>::is_exact;
	static constexpr bool hasInfinity    =  std::numeric_limits<T>::has_infinity;
	static constexpr T    infinity       =  std::numeric_limits<T>::infinity();
	static constexpr T    negInfinity    = -std::numeric_limits<T>::infinity();
	static constexpr T    lowest         =  std::numeric_limits<T>::lowest();
	static constexpr T    min            =  std::numeric_limits<T>::min();
	static constexpr T    max            =  std::numeric_limits<T>::max();
	static constexpr T    epsilon        =  std::numeric_limits<T>::epsilon();
	static constexpr T    NaN            =  std::numeric_limits<T>::quiet_NaN();
};

template<class T> requires Type_IsFundamental<T>
struct NumLimit_Struct<T> { using Type = NumLimit_FundamentalType<T>; };

template<class T>
using NumLimit = typename NumLimit_Struct<T>::Type;

inline constexpr f32   f32_epsilon       = NumLimit<f32>::epsilon;
inline constexpr f64   f64_epsilon       = NumLimit<f64>::epsilon;
inline constexpr Float Float_epsilon     = NumLimit<Float>::epsilon;

inline constexpr f32   f32_NaN           = NumLimit<f32>::NaN;
inline constexpr f64   f64_NaN           = NumLimit<f64>::NaN;
inline constexpr Float Float_NaN         = NumLimit<Float>::NaN;

inline constexpr f32   f32_infinity      = NumLimit<f32>::infinity;
inline constexpr f64   f64_infinity      = NumLimit<f64>::infinity;
inline constexpr Float Float_infinity    = NumLimit<Float>::infinity;

inline constexpr f32   f32_negInfinity   = NumLimit<f32>::negInfinity;
inline constexpr f64   f64_negInfinity   = NumLimit<f64>::negInfinity;
inline constexpr Float Float_negInfinity = NumLimit<Float>::negInfinity;

// for internal use, i.e. unit test cannot have high level logger functions
inline void __ax_internal_log(const char* msg) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "libax", msg);
#else
	std::wcout << msg << std::endl;
#endif
}

// for internal use, i.e. unit test cannot have high level logger functions
inline void __ax_internal_logError(const char* msg) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_ERROR, "libax", msg);
#else
	std::wcerr << msg;
#endif
}

void __ax_internal_forceCrash() {
	std::cout << "ax_force_crash\n";
	*reinterpret_cast<int*>(1) = 0;
}

void __ax_internal_assert(const char* title, const char* expr, const SrcLoc& loc, const char* msg) {
	const int bufLen = 32 * 1024;
	char buf[bufLen + 1];
	snprintf(buf, bufLen,
		"\n%s\n"
		"  Expr: %s\n"
		"  Func: %s\n"
		"Source: %s:%lld\n"
		"-------------\n"
		"%s\n",
		title,
			expr,
			loc.function().c_str(),
			loc.file().c_str(), loc.line(),
			msg);
	buf[bufLen] = 0; //snprintf might not end with zero if exists bufLen limit

#if AX_OS_WINDOWS & _DEBUG
	if (1 == _CrtDbgReport(_CRT_ASSERT, loc.file().c_str(), static_cast<int>(loc.line()), "bax", "%s", buf)) {
		_CrtDbgBreak();
	}
#else
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_ERROR, "libax", buf);
#else
	std::wcerr << msg;
#endif
	AX_ASSERT(false);
#endif
}

template<class T> using ax_enum_int_t = std::underlying_type_t<T>;
template<class T> AX_NODISCARD constexpr auto ax_enum_int(const T & v) { return static_cast<ax_enum_int_t<T>>(v); }

template<class T> requires Type_IsChar<T>
AX_INLINE constexpr bool ax_char_is_hex(T ch) {
	if( ch >= '0' && ch <='9' ) return true;
	if( ch >= 'A' && ch <='F' ) return true;
	if( ch >= 'a' && ch <='f' ) return true;
	return false;
}

template<class T> requires Type_IsChar<T>
AX_INLINE constexpr Opt<u8> ax_char_hex_to_u8(T ch) {
	if( ch >= '0' && ch <= '9' ) return static_cast<u8>(ch - '0');
	if( ch >= 'a' && ch <= 'f' ) return static_cast<u8>(ch - 'a' + 10);
	if( ch >= 'A' && ch <= 'F' ) return static_cast<u8>(ch - 'A' + 10);
	return std::nullopt;
}

template<class T> requires Type_IsChar<T>
AX_INLINE constexpr Pair<T,T> ax_char_u8_to_upper_hex_pair(u8 ch) {
	constexpr char hex[] = "0123456789ABCDEF";
	return Pair<T,T>(static_cast<T>(hex[(ch >> 4) & 0xF]),
					 static_cast<T>(hex[ ch       & 0xF]));
}

struct DebuggerNatvisHex {
	// UpperCase
	struct Low4  { uint8_t c; };
	struct High4 { uint8_t c; };

	// LowerCase
	struct low4  { uint8_t c; };
	struct high4 { uint8_t c; };
};

inline bool ax_is_debugger_present() {
#if AX_OS_WINDOWS
	return ::IsDebuggerPresent();
#else
	return false;
#endif
}


} // namespace
