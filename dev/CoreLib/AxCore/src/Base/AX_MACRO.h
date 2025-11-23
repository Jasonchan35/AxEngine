#pragma once

#define AX_FORWARD(a) ::std::forward<decltype(a)>(a)

#define AX_ASSERT(EXPR) ::ax::ax_assert((EXPR), #EXPR)
#define AX_ASSERT_MSG(EXPR, MSG) ::ax::ax_assert((EXPR), MSG)

#define AX_UNUSED(v)	((void)v)


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
	constexpr T    operator+ (const T&  a, ::ax::EnumFn_<T>::IntType b) { return static_cast<T>(::ax::ax_enum_int(a) + b); } \
	constexpr T    operator- (const T&  a, ::ax::EnumFn_<T>::IntType b) { return static_cast<T>(::ax::ax_enum_int(a) - b); } \
	constexpr T    operator* (const T&  a, ::ax::EnumFn_<T>::IntType b) { return static_cast<T>(::ax::ax_enum_int(a) * b); } \
	constexpr T    operator/ (const T&  a, ::ax::EnumFn_<T>::IntType b) { return static_cast<T>(::ax::ax_enum_int(a) / b); } \
	constexpr void operator+=(T& a, ::ax::EnumFn_<T>::IntType b) { a = a + b; } \
	constexpr void operator-=(T& a, ::ax::EnumFn_<T>::IntType b) { a = a - b; } \
	constexpr void operator*=(T& a, ::ax::EnumFn_<T>::IntType b) { a = a * b; } \
	constexpr void operator/=(T& a, ::ax::EnumFn_<T>::IntType b) { a = a / b; } \
//--------

#define AX_ENUM_COMPARE_OPERATOR(T) \
	constexpr bool operator <  (T a, T b) { return enumInt(a) <  enumInt(b); } \
	constexpr bool operator >  (T a, T b) { return enumInt(a) >  enumInt(b); } \
	constexpr bool operator <= (T a, T b) { return enumInt(a) <= enumInt(b); } \
	constexpr bool operator >= (T a, T b) { return enumInt(a) >= enumInt(b); } \
//-----

#define AX_ENUM_ALL_OPERATOR(T) \
	// AX_ENUM_BITWISE_OPERATOR(T) \
	// AX_ENUM_ARITHMETIC_OPERATOR(T) \
	// AX_ENUM_ARITHMETIC_OPERATOR_INT(T) \
	// AX_ENUM_COMPARE_OPERATOR(T) \
//-------

#define AX_ENUM_STR__CASE(V, ...) case CURRENT_ENUM_T::V: return #V;

#define AX_ENUM_STR(LIST, T) \
	inline StrLit enumStr(const T& v) { \
		using CURRENT_ENUM_T = T; \
		switch (v) { \
			LIST(AX_ENUM_STR__CASE) \
			default: AX_ASSERT(false); return ""; \
		} \
	} \
//----

#define AX_ENUM_TRY_PARSE__CASE(V, ...) if (str == StrView(#V)) { outValue = CURRENT_ENUM_T::V; return true; }

#define AX_ENUM_TRY_PARSE(LIST, T) \
	inline bool enumTryParse(T& outValue, StrView str) { \
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