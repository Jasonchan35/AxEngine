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

#define AX_FILE		__FILE__
#define AX_LINE		static_cast<::ax::Int>(__LINE__)

#define AX_FORWARD(a) ::std::forward<decltype(a)>(a)
#define AX_ASSERT(EXPR) ::ax::ax_assert((EXPR), #EXPR)
#define AX_ASSERT_MSG(EXPR, MSG) ::ax::ax_assert((EXPR), MSG)

#define _AX_JOIN_WORD_IMPL( w0,w1)		w0##w1
#define _AX_JOIN_WORD3_IMPL(w0,w1,w2)	w0##w1##w2

#define AX_JOIN_WORD( w0,w1)			_AX_JOIN_WORD_IMPL( w0, w1)
#define AX_JOIN_WORD3(w0,w1,w2)			_AX_JOIN_WORD3_IMPL(w0, w1, w2)

#define AX_WITH_NAME(v) ::ax::WithName(v, #v)

#define AX_ALLOC_REQ	::ax::MemAllocRequest(ax_current_allocator())

#define AX_SIMPLE_ERROR(ERROR_TYPE) \
	class ERROR_TYPE : public Error { \
		public: \
		ERROR_TYPE(const SrcLoc& srcLoc = SrcLoc()) : Error(srcLoc) {} \
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
	inline StrLit ax_enum_str(const T& v) { \
		using CURRENT_ENUM_T = T; \
		switch (v) { \
			LIST(AX_ENUM_STR__CASE) \
			default: AX_ASSERT(false); return ""; \
		} \
	} \
//----

#define AX_ENUM_TRY_PARSE__CASE(V, ...) if (str == StrView(#V)) { outValue = CURRENT_ENUM_T::V; return true; }

#define AX_ENUM_TRY_PARSE(LIST, T) \
	inline bool ax_enum_try_parse(T& outValue, StrView str) { \
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