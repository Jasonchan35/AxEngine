#pragma once

#define AX_UNUSED(v)	((void)v)
#define AX_COMMA ,
#define AX_EMPTY
#define AX_STRINGIFY(...)	#__VA_ARGS__

#define AX_IDENTITY(x) x
#define AX_WRAP(...)		__VA_ARGS__	

#define AX_CALL(M, ARGS) AX_IDENTITY( M(ARGS) )
// work around for MSVC bug - MSVC expands all __VA_ARGS__ as one argument
//#define ax_CALL_MACRO(MACRO_NAME, ARGS) MACRO_NAME ARGS

#define AX_SIZEOF(...)  ::ax::ax_const_eval_Int(sizeof(__VA_ARGS__))
#define AX_ALIGNOF(...) ::ax::ax_const_eval_Int(alignof(__VA_ARGS__))

#define AX_FILE		__FILE__
#define AX_LINE		static_cast<::ax::Int>(__LINE__)

#if AX_COMPILER_GCC | AX_COMPILER_CLANG
	#define AX_PRAGMA_GCC_STRINGIZE(x)	_Pragma(#x)
	#define AX_PRAGMA_GCC(x)			AX_PRAGMA_GCC_STRINGIZE(GCC x)
#else
	#define AX_PRAGMA_GCC(x)	
#endif

#if AX_COMPILER_VC
	#define AX_PROGMA_VC(x)		__pragma(x)
#else
	#define AX_PROGMA_VC(x)
#endif

#define AX_FORWARD(a) ::std::forward<decltype(a)>(a)
#define AX_ASSERT(EXPR) ::ax::ax_assert((EXPR), #EXPR)
#define AX_ASSERT_MSG(EXPR, MSG) ::ax::ax_assert((EXPR), MSG)

#define _AX_JOIN_WORD_IMPL( w0,w1)		w0##w1
#define _AX_JOIN_WORD3_IMPL(w0,w1,w2)	w0##w1##w2

#define AX_JOIN_WORD( w0,w1)			_AX_JOIN_WORD_IMPL( w0, w1)
#define AX_JOIN_WORD3(w0,w1,w2)			_AX_JOIN_WORD3_IMPL(w0, w1, w2)


#if 0 // AX_COMPILER_VC
	#define AX_SZ_A_JOIN(A,B)	(  ""     A ##     B)
	#define AX_SZ_W_JOIN(A,B)	( L""  L##A ##  L##B)
	#define AX_SZ_8_JOIN(A,B)	(u8"" u8##A ## u8##B)
	#define AX_SZ_16_JOIN(A,B)	( u""  u##A ##  u##B)
	#define AX_SZ_32_JOIN(A,B)	( U""  U##A ##  u##B)

	#define AX_SZ_A( sz)		(  ""     sz)
	#define AX_SZ_W( sz)		( L""  L##sz)
	#define AX_SZ_8( sz)		(u8""  u##sz)
	#define AX_SZ_16(sz)		( u""  u##sz)
	#define AX_SZ_32(sz)		( U""  U##sz)

	#define AX_CH_A( c)			::ax::CharA(    c)
	#define AX_CH_W( c)			::ax::CharW( L##c)
	#define AX_CH_8( c)			::ax::Char8(u8##c)
	#define AX_CH_16(c)			::ax::Char16(u##c)
	#define AX_CH_32(c)			::ax::Char32(U##c)

#else
	#define AX_SZ_A_JOIN(A,B)	( ""  A ## B)
	#define AX_SZ_W_JOIN(A,B)	( L"" A ## B)
	#define AX_SZ_8_JOIN(A,B)	(u8"" A ## B)
	#define AX_SZ_16_JOIN(A,B)	( u"" A ## B)
	#define AX_SZ_32_JOIN(A,B)	( U"" A ## B)

	#define AX_SZ_A( sz)		(  ""  sz)
	#define AX_SZ_W( sz)		( L""  sz)
	#define AX_SZ_8( sz)		(u8""  sz)
	#define AX_SZ_16(sz)		( u""  sz)
	#define AX_SZ_32(sz)		( U""  sz)

	#define AX_CH_A( c)			::ax::CharA(    c)
	#define AX_CH_W( c)			::ax::CharW( L##c)
	#define AX_CH_8( c)			::ax::Char8(u8##c)
	#define AX_CH_16(c)			::ax::Char16(u##c)
	#define AX_CH_32(c)			::ax::Char32(U##c)

#endif

#ifdef _DEBUG
	#define AX_DEBUG_ONLY_SZ(...)			AX_SZ(__VA_ARGS__)
	#define AX_DEBUG_ONLY_STR_LIT(...)		AX_STR_LIT(__VA_ARGS__)
#else
	#define AX_DEBUG_ONLY_SZ(...)			AX_SZ("")
	#define AX_DEBUG_ONLY_STR_LIT(...)		AX_STR_LIT("")
#endif

#define AX_STR_A( sz)		::ax::StrLit_<const ::ax::CharA >(AX_SZ_A( sz))
#define AX_STR_W( sz)		::ax::StrLit_<const ::ax::CharW >(AX_SZ_W( sz))
#define AX_STR_8( sz)		::ax::StrLit_<const ::ax::Char8 >(AX_SZ_8( sz))
#define AX_STR_16(sz)		::ax::StrLit_<const ::ax::Char16>(AX_SZ_16(sz))
#define AX_STR_32(sz)		::ax::StrLit_<const ::ax::Char32>(AX_SZ_32(sz))

#define AX_TYPE_CHAR_DEFINE_A   1001
#define AX_TYPE_CHAR_DEFINE_W   1002
#define AX_TYPE_CHAR_DEFINE_8   1008
#define AX_TYPE_CHAR_DEFINE_16  1016
#define AX_TYPE_CHAR_DEFINE_32  1032

//define axChar type
#define AX_TYPE_CHAR_DEFINE AX_TYPE_CHAR_DEFINE_A

#if AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_A
	#define AX_SZ(sz)		AX_SZ_A(sz)
	#define AX_CH(sz)		AX_CH_A(sz)
	#define AX_STR(sz)		AX_STR_A(sz)
	#define AX_SZ_JOIN(sz)	AX_SZ_A_JOIN(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_W
	#define AX_SZ(sz)		AX_SZ_W(sz
	#define AX_CH(sz)		AX_CH_W(sz)
	#define AX_STR(sz)		AX_STR_W(sz)
	#define AX_SZ_JOIN(sz)	AX_SZ_W_JOIN(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_8
	#define AX_SZ(sz)		AX_SZ_8(sz)
	#define AX_CH(sz)		AX_CH_8(sz)
	#define AX_STR(sz)		AX_STR_8(sz)
	#define AX_SZ_JOIN(sz)	AX_SZ_8_JOIN(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_16
	#define AX_SZ(sz)		AX_SZ_16(sz)
	#define AX_CH(sz)		AX_CH_16(sz)
	#define AX_STR(sz)		AX_STR_16(sz)
	#define AX_SZ_JOIN(sz)	AX_SZ_16_JOIN(sz)

#elif AX_TYPE_CHAR_DEFINE == AX_TYPE_CHAR_DEFINE_32
	#define AX_SZ(sz)		AX_SZ_32(sz)
	#define AX_CH(sz)		AX_CH_32(sz)
	#define AX_STR(sz)		AX_STR_32(sz)
	#define AX_SZ_JOIN(sz)	AX_SZ_32_JOIN(sz)

#else
	#error "Unknown AX_TYPE_CHAR_DEFINE"
#endif


//----------------
#define AX_ALLOC_REQ	::ax::MemAllocRequest(ax_current_allocator())

#define AX_SIMPLE_ERROR(ERROR_TYPE) \
	class ERROR_TYPE : public Error { \
		public: \
		ERROR_TYPE(const SrcLoc& srcLoc = SrcLoc()) : Error(srcLoc) {} \
		ERROR_TYPE(std::string_view msg, const SrcLoc& srcLoc = SrcLoc()) : Error(msg, srcLoc) {} \
	}; \
//------

//----- TYPE_LIST ----
#define AX_CHAR_TYPE_SUFFIX_LIST(E)				E(A) E(W)     E(8)     E(16)     E(32)
#define	AX_TYPE_LIST_CHAR_OTHER_THAN_CHAR_A(E)		 E(CharW) E(Char8) E(Char16) E(Char32)

#define	AX_TYPE_LIST_CHAR(E) \
	E(CharA) \
	AX_TYPE_LIST_CHAR_OTHER_THAN_CHAR_A(E) \
//----

//--- Enum -----

#define AX_ENUM_BITWISE_OPERATOR(T) \
	constexpr T operator~ (const T&  a)             { return static_cast<T>(~::ax::ax_enum_int(a)); } \
	constexpr T operator| (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) | ::ax::ax_enum_int(b)); } \
	constexpr T operator& (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) & ::ax::ax_enum_int(b)); } \
	constexpr T operator^ (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) ^ ::ax::ax_enum_int(b)); } \
	constexpr void operator|=(T& a, const T& b) { a = a | b; } \
	constexpr void operator&=(T& a, const T& b) { a = a & b; } \
	constexpr void operator^=(T& a, const T& b) { a = a ^ b; } \
//--------

#define AX_ENUM_ARITHMETIC_OPERATOR(T) \
	constexpr T operator+ (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) + ::ax::ax_enum_int(b)); } \
	constexpr T operator- (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) - ::ax::ax_enum_int(b)); } \
	constexpr T operator* (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) * ::ax::ax_enum_int(b)); } \
	constexpr T operator/ (const T&  a, const T& b) { return static_cast<T>( ::ax::ax_enum_int(a) / ::ax::ax_enum_int(b)); } \
	constexpr void operator+=(T& a, const T& b) { a = a + b; } \
	constexpr void operator-=(T& a, const T& b) { a = a - b; } \
	constexpr void operator*=(T& a, const T& b) { a = a * b; } \
	constexpr void operator/=(T& a, const T& b) { a = a / b; } \
//--------

#define AX_ENUM_ARITHMETIC_OPERATOR_INT(T) \
	constexpr void operator++(T&  a, int) { a = static_cast<T>(::ax::ax_enum_int(a) + 1); } \
	constexpr T    operator++(T&  a)      { a = static_cast<T>(::ax::ax_enum_int(a) + 1); return a; } \
	constexpr T    operator+ (const T&  a, ::ax::ax_enum_int_t<T> b) { return static_cast<T>(::ax::ax_enum_int(a) + b); } \
	constexpr T    operator- (const T&  a, ::ax::ax_enum_int_t<T> b) { return static_cast<T>(::ax::ax_enum_int(a) - b); } \
	constexpr T    operator* (const T&  a, ::ax::ax_enum_int_t<T> b) { return static_cast<T>(::ax::ax_enum_int(a) * b); } \
	constexpr T    operator/ (const T&  a, ::ax::ax_enum_int_t<T> b) { return static_cast<T>(::ax::ax_enum_int(a) / b); } \
	constexpr void operator+=(T& a, ::ax::ax_enum_int_t<T> b) { a = a + b; } \
	constexpr void operator-=(T& a, ::ax::ax_enum_int_t<T> b) { a = a - b; } \
	constexpr void operator*=(T& a, ::ax::ax_enum_int_t<T> b) { a = a * b; } \
	constexpr void operator/=(T& a, ::ax::ax_enum_int_t<T> b) { a = a / b; } \
//--------

#define AX_ENUM_COMPARE_OPERATOR(T) \
	constexpr bool operator <  (T a, T b) { return ::ax::ax_enum_int(a) <  ::ax::ax_enum_int(b); } \
	constexpr bool operator >  (T a, T b) { return ::ax::ax_enum_int(a) >  ::ax::ax_enum_int(b); } \
	constexpr bool operator <= (T a, T b) { return ::ax::ax_enum_int(a) <= ::ax::ax_enum_int(b); } \
	constexpr bool operator >= (T a, T b) { return ::ax::ax_enum_int(a) >= ::ax::ax_enum_int(b); } \
//-----

#define AX_ENUM_ALL_OPERATOR(T) \
	AX_ENUM_BITWISE_OPERATOR(T) \
	AX_ENUM_ARITHMETIC_OPERATOR(T) \
	AX_ENUM_ARITHMETIC_OPERATOR_INT(T) \
	AX_ENUM_COMPARE_OPERATOR(T) \
//-------

#define AX_ENUM_STR__CASE(V, ...) case CURRENT_ENUM_T::V: return #V;

#define AX_ENUM_STR(LIST, T) \
	constexpr StrLit _ax_macro_enum_str(const T& v) { \
		using CURRENT_ENUM_T = T; \
		switch (v) { \
			LIST(AX_ENUM_STR__CASE) \
			default: AX_ASSERT(false); return ""; \
		} \
	} \
//----

#define AX_ENUM_TRY_PARSE__CASE(V, ...) if (str == StrView(#V)) { outValue = CURRENT_ENUM_T::V; return true; }

#define AX_ENUM_TRY_PARSE(LIST, T) \
	constexpr bool _ax_macro_enum_try_parse(StrView str, T& outValue) { \
		using CURRENT_ENUM_T = T; \
		LIST(AX_ENUM_TRY_PARSE__CASE) \
		return false; \
	} \
//----

#define AX_ENUM_STR_UTIL(LIST, T) \
	AX_ENUM_STR(LIST, T) \
	AX_ENUM_TRY_PARSE(LIST, T) \
//----

#define AX_ENUM_DECLARE__ITEM(ITEM, VALUE, ...) ITEM VALUE,

#define AX_ENUM_DECLARE(LIST, T, BASE_TYPE) \
	enum class T : BASE_TYPE { \
		LIST(AX_ENUM_DECLARE__ITEM) \
	}; \
//----

#define AX_ENUM_CLASS(LIST, T, BASE_TYPE) \
	AX_ENUM_DECLARE(LIST, T, BASE_TYPE) \
	AX_ENUM_STR_UTIL(LIST, T) \
	AX_ENUM_ALL_OPERATOR(T) \
//----

#define AX_ENUM_FLAGS_CLASS(LIST,T, BASE_TYPE) AX_ENUM_CLASS(LIST, T, BASE_TYPE)

//----- Logger
#define AX_LOG(fmt, ...)			do{ ::ax::Logger::s_get()->log(::ax::SrcLoc(), ::ax::LogLevel::Info,	AX_STR(fmt), ##__VA_ARGS__); }while(false)
#define AX_LOG_WARNING(fmt, ...)	do{ ::ax::Logger::s_get()->log(::ax::SrcLoc(), ::ax::LogLevel::Warning,	AX_STR(fmt), ##__VA_ARGS__); }while(false)
#define AX_LOG_ERROR(fmt, ...)		do{ ::ax::Logger::s_get()->log(::ax::SrcLoc(), ::ax::LogLevel::Error,	AX_STR(fmt), ##__VA_ARGS__); }while(false)
#define AX_LOG_FLUSH()				do{ ::ax::Logger::s_get()->flush(); } while(false)

#define AX_LOG_WIN32_LAST_ERROR(msg)	do{ AX_LOG("Win32 Error {} {}", AX_Win32_Error::s_lastError(), StrView(msg)); }while(false)
#define AX_LOG_WIN32_HRESULT(code, msg)	do{ AX_LOG("HRESULT Error {} {}", code, AX_Win32_HRESULT(code),  StrView(msg)); }while(false)


#define AX_DUMP(v0)					do{ AX_LOG("AX_DUMP: {}=[{}]", AX_STR(#v0), (v0)); } while(false)

//----- NameId
#define AX_NAMEID(NAME_STR) ([]() -> const NameId& { static NameId s = NameId::s_compute(NAME_STR); return s; }())


#if AX_OS_WINDOWS
	#define AX_APP_MAIN(T) \
	 	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPWSTR lpCmdLine, int nCmdShow) { return ::ax::App_run<T>(0, nullptr); } \
	 	int main(int argc, const char* argv[]) { return ::ax::App_run<T>(argc, argv); } \
	// //--------

#else
	#define AX_APP_MAIN(T) \
	 	int main(int argc, const char* argv[]) { return ::ax::App_run<T>(argc, argv); } \
	//--------
	
#endif