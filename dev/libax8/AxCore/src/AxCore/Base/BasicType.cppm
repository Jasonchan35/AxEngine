module;


export module AxCore.BasicType;
export import AxCore.Common;

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

template<class T> using Type_EnumInt = std::underlying_type_t<T>;
template<class T> AX_NODISCARD constexpr auto ax_enum_int(const T & v) { return static_cast<Type_EnumInt<T>>(v); }

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

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

constexpr Int Int_min = std::numeric_limits<Int>::min();
constexpr Int Int_max = std::numeric_limits<Int>::max();

constexpr UInt UInt_min = std::numeric_limits<UInt>::min();
constexpr UInt UInt_max = std::numeric_limits<UInt>::max();


namespace  AxTag {
	class NewObject_{};		constexpr NewObject_	NewObject	= {};
	class NoInit_{};		constexpr NoInit_		NoInit		= {};
	class Zero_{};			constexpr Zero_			Zero		= {};
} // AxTag

template <class T>
	struct No_rvalue_ {
	T value;

	template<class IN_OBJ>
	constexpr No_rvalue_(IN_OBJ&& obj) requires std::is_lvalue_reference_v<IN_OBJ&&>
	: value(obj) {}

	constexpr No_rvalue_(T && v) : value(v) {}
		
	constexpr operator       T&()       { return value; }
	constexpr operator const T&() const { return value; }

	AX_INLINE 		T* operator->()			{ return &value; }
	AX_INLINE const	T* operator->() const	{ return &value; }

	AX_INLINE 		T& operator* ()			{ return value; }
	AX_INLINE const	T& operator* () const	{ return value; }

	AX_INLINE explicit operator bool() const	{ return bool(value);  }
};

template<class A,   class  B> inline constexpr bool Type_IsSame   = std::is_same_v<A, B>;
template<class BASE, class T> inline constexpr bool Type_IsBaseOf = std::is_base_of_v<BASE, T>;

template<class T> constexpr bool Type_isTriviallyCopyAssignable = std::is_trivially_copy_assignable_v<T>;
template<class T> constexpr bool Type_IsTriviallyDestructible   = std::is_trivially_destructible_v<T>;

template<class T> constexpr bool Type_IsFundamental =  std::is_fundamental_v<T>;
template<class T> constexpr bool Type_AnyInt        =  std::is_integral_v<T>;
template<class T> constexpr bool Type_AnySInt       =  std::is_integral_v<T> && std::is_signed_v<T>;
template<class T> constexpr bool Type_AnyUInt       =  std::is_integral_v<T> && std::is_unsigned_v<T>;
template<class T> constexpr bool Type_AnyFloat      =  std::is_floating_point_v<T>;
template<class T> constexpr bool Type_AnyChar       =  std::is_same_v<std::remove_cv_t<T>, CharA>
                                                    || std::is_same_v<std::remove_cv_t<T>, CharW>
                                                    || std::is_same_v<std::remove_cv_t<T>, Char8>
                                                    || std::is_same_v<std::remove_cv_t<T>, Char16>
                                                    || std::is_same_v<std::remove_cv_t<T>, Char32>;

template<class T> constexpr bool Type_Is_f32        =  std::is_same_v<T, f32>;
template<class T> constexpr bool Type_Is_f64        =  std::is_same_v<T, f64>;
template<class T> constexpr bool Type_IsEnum        =  std::is_enum_v<T>;

template<class T>
struct Type_MaybeEnumInt_Struct { using Type = T; }; 

template<class T>	requires std::is_enum_v<T>
struct Type_MaybeEnumInt_Struct<T> { using Type = std::underlying_type_t<T>; }; 

template<class T> using Type_MaybeEnumInt = Type_MaybeEnumInt_Struct<T>::Type;

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

template<class FUNC, class FUNC_SIG>
struct Type_IsFuncSig_Struct { static constexpr bool value = false; };

template<class FUNC, class RETURN, class...ARGS>
struct Type_IsFuncSig_Struct<FUNC, RETURN (ARGS...)> {
	static constexpr bool value =  std::is_invocable_v< FUNC, ARGS...>
								&& std::is_same_v<RETURN, std::invoke_result_t<FUNC, ARGS...>>;
};

template<class FUNC, class FUNC_SIG>
constexpr bool Type_IsFuncSig = Type_IsFuncSig_Struct<FUNC, FUNC_SIG>::value;

template<class T> AX_INLINE constexpr Int ax_strlen(const T* sz) {
	if (!sz) return 0;
	Int i = 0;
	for (; *sz; ++sz, ++i) { /* nothing */ }
	return i;
}

template<class T> class MutZStrView_;
using MutZStrView	= MutZStrView_<Char>;
using MutZStrViewA	= MutZStrView_<CharA>;
using MutZStrViewW	= MutZStrView_<CharW>;
using MutZStrView8	= MutZStrView_<Char8>;
using MutZStrView16	= MutZStrView_<Char16>;
using MutZStrView32	= MutZStrView_<Char32>;

template<class T> using ZStrView_ = MutZStrView_<const T>;
using ZStrView		= ZStrView_<Char>;
using ZStrViewA		= ZStrView_<CharA>;
using ZStrViewW		= ZStrView_<CharW>;
using ZStrView8		= ZStrView_<Char8>;
using ZStrView16	= ZStrView_<Char16>;
using ZStrView32	= ZStrView_<Char32>;

template<class T>
class MutZStrView_ {
	using This = MutZStrView_;
protected:
	T*	_data = nullptr;
	Int _size = 0;
public:
	
	using std_string_view = std::basic_string_view< std::remove_const_t<T> >;
	using CZView = MutZStrView_<const T>;

	AX_INLINE constexpr MutZStrView_() = default;
	AX_INLINE constexpr MutZStrView_(T* sz, Int size) : _data(sz), _size(size) {}

	template<Int N>
	AX_INLINE constexpr MutZStrView_(T (&sz)[N]) noexcept : _data(sz), _size(N > 0 ? N-1 : 0) {}

	AX_INLINE constexpr std_string_view to_string_view() const noexcept { return std_string_view(_data, _size); }

	constexpr 		T*	data() noexcept			{ return _data; }
	constexpr const	T*	data() const noexcept	{ return _data; }
	constexpr	Int		size() const noexcept	{ return _size; }
	
	constexpr CZView	constView() const		{ return CZView(_data, _size); }
	constexpr const T*	c_str() const			{ return _size ? _data : &_empty_c_str; }
	
	static constexpr This s_from_c_str(T *sz) { return This(sz, ax_strlen(sz)); } 
	
private:
	static constexpr T _empty_c_str = 0;
};

template<class T> using ZStrView_ = MutZStrView_<const T>;

template<class T> inline
ZStrView_<T> ZStrView_c_str(const T* sz) { return ZStrView_<T>(sz, ax_strlen(sz)); }


AX_INLINE consteval ZStrViewA  operator ""_sv(const CharA * sz, size_t n)  noexcept { return ZStrViewA (sz, n); }
AX_INLINE consteval ZStrViewW  operator ""_sv(const CharW * sz, size_t n)  noexcept { return ZStrViewW (sz, n); }
AX_INLINE consteval ZStrView8  operator ""_sv(const Char8 * sz, size_t n)  noexcept { return ZStrView8 (sz, n); }
AX_INLINE consteval ZStrView16 operator ""_sv(const Char16* sz, size_t n)  noexcept { return ZStrView16(sz, n); }
AX_INLINE consteval ZStrView32 operator ""_sv(const Char32* sz, size_t n)  noexcept { return ZStrView32(sz, n); }

//TODO change to consteval, to ensure it's real string literal
template<class T>
class MutStrLit_ : public MutZStrView_<T> {
	using This = MutStrLit_;
	using Base = MutZStrView_<T>;
	using Base::_data;
	using Base::_size;
public:

	constexpr MutStrLit_() noexcept = default;
	constexpr MutStrLit_(const This&) noexcept = default;

	AX_INLINE constexpr MutStrLit_(T* sz, Int size) noexcept : Base(sz, size) {}
	
	template<Int N>
	AX_INLINE constexpr MutStrLit_(T (&sz)[N]) noexcept : Base(sz) {}
	
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const { return _size > 0; } 

	constexpr MutStrLit_ s_from_c_str(T* sz) noexcept { return MutStrLit_(sz, ax_strlen(sz)); }

protected:
	static constexpr T _empty_c_str = 0;	
};

template<class T> using StrLit_ = MutStrLit_<const T>;
using StrLit   = StrLit_<Char>;
using StrLitA  = StrLit_<CharA >;
using StrLitW  = StrLit_<CharW >;
using StrLit8  = StrLit_<Char8 >;
using StrLit16 = StrLit_<Char16>;
using StrLit32 = StrLit_<Char32>;

AX_INLINE constexpr StrLit ConstStrLit_bool(bool v) { return v ? StrLit("true") : StrLit("false"); }


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

inline void __ax_internal_assert(const char* title, const char* expr, const char* msg, const std::source_location& srcLoc = std::source_location::current()) {
	const int bufLen = 32 * 1024;
	char buf[bufLen + 1];
	snprintf(buf, bufLen,
		"\n%s\n"
		"  Expr: %s\n"
		"  Func: %s\n"
		"Source: %s:%u:%u\n"
		"-------------\n"
		"%s\n",
		title,
		expr,
		srcLoc.function_name(),
		srcLoc.file_name(), srcLoc.line(), srcLoc.column(),
		msg);
	buf[bufLen] = 0; //snprintf might not end with zero if exists bufLen limit

#if AX_OS_WINDOWS & _DEBUG
	if (1 == _CrtDbgReport(_CRT_ASSERT, srcLoc.file_name(), static_cast<int>(srcLoc.line()), "bax", "%s", buf)) {
		_CrtDbgBreak();
	}
#else
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_ERROR, "libax", buf);
#else
	std::wcerr << msg;
#endif
	assert(false);
#endif
}

inline void ax_assert(bool expr, ZStrView exprStr, const std::source_location & srcLoc = std::source_location::current()) {
	if (expr) return;
	__ax_internal_assert("ax_assert", exprStr.c_str(), "", srcLoc);
}

template<class T> using Opt = std::optional<T>;

template<class A, class B> using Pair = std::pair<A, B>;
template<class A, class B> constexpr auto Pair_make(A && a, B && b) { return Pair(AX_FORWARD(a), AX_FORWARD(b)); }

// namespace Tag {
// 	class NewObject_{};		inline constexpr NewObject_ 	NewObject	= {};
// 	class NoInit_{};		inline constexpr NoInit_		NoInit		= {};
// 	class All_{};			inline constexpr All_			All			= {};
// } // namespace Tag

struct SrcLoc {
	constexpr SrcLoc(AxTag::NoInit_) noexcept {}
	constexpr SrcLoc(const std::source_location & loc) noexcept : _loc(loc) {};

	static constexpr SrcLoc s_current(const std::source_location & loc = std::source_location::current()) { return loc; }
	
	constexpr Int    		column	() const noexcept	{ return _loc.column(); }
	constexpr Int    		line	() const noexcept	{ return _loc.line(); }
	constexpr const char*	file	() const noexcept	{ return _loc.file_name(); }
	constexpr const char*	function() const noexcept	{ return _loc.function_name(); }
	
protected:
	std::source_location _loc;
};

enum class StrCase : u8 {
	Ignore,
	Sensitive,
};

template<class A, class B> using Type_KeepConst = std::conditional_t< std::is_const_v<A>, const B, B>;

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

template<class A, class B> constexpr bool ax_op_equal			(const A& a, const B& b) { return a == b; }
template<class A, class B> constexpr bool ax_op_not_equal		(const A& a, const B& b) { return a != b; }
template<class A, class B> constexpr bool ax_op_less			(const A& a, const B& b) { return a <  b; }
template<class A, class B> constexpr bool ax_op_less_equal		(const A& a, const B& b) { return a <= b; }
template<class A, class B> constexpr bool ax_op_greater			(const A& a, const B& b) { return a >  b; }
template<class A, class B> constexpr bool ax_op_greater_equal	(const A& a, const B& b) { return a >= b; }

template<class CH> struct CharHexPair {
	CH c0, c1;
	constexpr CharHexPair() = default;
	constexpr CharHexPair(CH c0_, CH c1_) noexcept : c0(c0_), c1(c1_) {}

	constexpr 		CH*	data() noexcept			{ return &c0; }
	constexpr const	CH*	data() const noexcept	{ return &c0; }
	consteval		Int	size() const noexcept	{ return 2; }
};

consteval Int ax_consteval_Int(size_t v) {
	Int o = static_cast<Int>(v);
	if (o < 0) throw std::exception("ax_consteval_Int");
	return o;
} 

template<class T, class... Args > AX_INLINE
T* ax_call_constructor(T* p, Args&&... args ) {
	return ::new(p) T(AX_FORWARD(args)...);
}

template<class T> AX_INLINE
void ax_call_destructor(T* p ) noexcept {
	p->~T();
}

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

template<class T> struct NumLimit_Struct {
	using Type = typename T::_NumLimit;
};

template<class T>
struct NumLimit_FundamentalType {
	static constexpr bool isExact        =  std::numeric_limits<T>::is_exact;    
	static constexpr bool hasInfinity    =  std::numeric_limits<T>::has_infinity; 
	static constexpr T    infinity       () { return  std::numeric_limits<T>::infinity();    }
	static constexpr T    negInfinity    () { return -std::numeric_limits<T>::infinity();    }
	static constexpr T    lowest         () { return  std::numeric_limits<T>::lowest();      }
	static constexpr T    min            () { return  std::numeric_limits<T>::min();         }
	static constexpr T    max            () { return  std::numeric_limits<T>::max();         }
	static constexpr T    epsilon        () { return  std::numeric_limits<T>::epsilon();     }
	static constexpr T    NaN            () { return  std::numeric_limits<T>::quiet_NaN();   }
};

template<class T> requires Type_IsFundamental<T>
struct NumLimit_Struct<T> { using Type = NumLimit_FundamentalType<T>; };

template<class T>
using NumLimit = typename NumLimit_Struct<T>::Type;

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
