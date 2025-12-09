module;

export module AxCore.ZStrUtil;
export import AxCore.BasicType;
export import AxCore.Float16;

export namespace ax {

template<class T>
class _ZStrUtil_T_Impl {
//	static_assert(!std::is_const_v<T>);
public:
	static constexpr const T* kEmpty();
	static constexpr const T* kPathSeparatorChars();
	static constexpr const T* kHexUpperChars();
	static constexpr const T* kHexLowerChars();	
	static constexpr Int length( const T* s ) {
		if( !s ) return 0;

		Int len = 0;
		for( ; *s; s++, len++ ) {
		}
		return len;
	}

	static constexpr Int length(const T* s, Int maxLen) {
		if (!s) return 0;
		Int l=0;
		for (;;) {
			if (l > maxLen) return maxLen;
			if (*s == 0) return l;
			l++;
			s++;
		}
	}

	static Int	charCaseCompare(T a, T b) { return toUpper(a) - toUpper(b); }
	static bool	charCaseEquals (T a, T b) { return charCaseCompare(a, b) == 0; }

	static Int	caseCompare(const T* s1, const T* s2);
	static Int	caseCompare(const T* s1, const T* s2, Int n);

	static bool	equals(const T* s1, const T* s2)			{ return compare(s1, s2) == 0; }
	static bool	equals(const T* s1, const T* s2, Int n)		{ return compare(s1, s2, n) == 0; }

	static bool	caseEquals(const T* s1, const T* s2)		{ return caseCompare(s1, s2) == 0; }
	static bool	caseEquals(const T* s1, const T* s2, Int n)	{ return caseCompare(s1, s2, n) == 0; }

	static bool	startsWith		( const T* full, const T* prefix );
	static bool	endsWith		( const T* full, const T* suffix );
	
	static T*	findChars		(T* sz, const T* chrs);
	static T*	findCharsBack	(T* sz, const T* chrs);
	static T*	findCaseChar	(T* sz, T ch);
	static T*	findCaseStr		(T* big, const T* little );
	static T*	findChar		( T* sz, T ch );
	static T*	findCharBack	( T* sz, T ch );
	static T*	findStr			( T* a, T* b );
	static T*	findBackStr		( T* a, T* b );

	static Int	compare	( const T* s1, const T* s2 );
	static Int	compare	( const T* s1, const T* s2, Int n );

	static constexpr T* fileBasename(T* sz);

	static T* copy(T* dst, const T* src, Int n);
	static T* copy(T* dst, const T* src );

	static bool parse(const T* sz, bool &v);

	static bool parse(const T* sz, i8  &v);
	static bool parse(const T* sz, i16 &v);
	static bool parse(const T* sz, i32 &v);
	static bool parse(const T* sz, i64 &v);

	static bool parse(const T* sz, u8  &v);
	static bool parse(const T* sz, u16 &v);
	static bool parse(const T* sz, u32 &v);
	static bool parse(const T* sz, u64 &v);

	static bool parse(const T* sz, f16  &v);
	static bool parse(const T* sz, f32  &v);
	static bool parse(const T* sz, f64  &v);
	static bool parse(const T* sz, f128 &v);
};

#define E(T) \
	template<> AX_INLINE constexpr const Char##T* _ZStrUtil_T_Impl<Char##T>::kEmpty				() { return AX_SZ_##T(""); } \
	template<> AX_INLINE constexpr const Char##T* _ZStrUtil_T_Impl<Char##T>::kPathSeparatorChars() { return AX_SZ_##T("\\/"); } \
	template<> AX_INLINE constexpr const Char##T* _ZStrUtil_T_Impl<Char##T>::kHexUpperChars		() { return AX_SZ_##T("0123456789ABCDEF"); } \
	template<> AX_INLINE constexpr const Char##T* _ZStrUtil_T_Impl<Char##T>::kHexLowerChars		() { return AX_SZ_##T("0123456789abcdef"); } \
//------
	AX_CHAR_TYPE_SUFFIX_LIST(E)
#undef E

class ZStrUtil {
	template<class T> using Util_T = _ZStrUtil_T_Impl<T>;
public:

	template<class T> static const T* kEmpty()				{ return Util_T<T>::kEmpty()				;} 
	template<class T> static const T* kPathSeparatorChars()	{ return Util_T<T>::kPathSeparatorChars()	;} 
	template<class T> static const T* kHexUpperChars()		{ return Util_T<T>::kHexUpperChars()		;} 
	template<class T> static const T* kHexLowerChars()		{ return Util_T<T>::kHexLowerChars()		;} 


	template<class T> AX_INLINE static Int		length(const T* sz)				{ return Util_T<T>::length(sz); }
	template<class T> AX_INLINE static Int		length(const T* sz, Int maxLen)	{ return Util_T<T>::length(sz, maxLen); }

	template<class T> AX_INLINE static Int		charCaseCompare(T a, T b) { return Util_T<T>::charCaseCompare(a, b); }
	template<class T> AX_INLINE static bool		charCaseEquals (T a, T b) { return Util_T<T>::charCaseCompare(a, b) == 0; }

	template<class T> AX_INLINE static Int		compare		(const T* s1, const T* s2)			{ return Util_T<T>::compare(s1, s2); }
	template<class T> AX_INLINE static Int		compare		(const T* s1, const T* s2, Int n)	{ return Util_T<T>::compare(s1, s2, n); }

	template<class T> AX_INLINE static Int		caseCompare	(const T* s1, const T* s2)			{ return Util_T<T>::caseCompare(s1, s2); }
	template<class T> AX_INLINE static Int		caseCompare	(const T* s1, const T* s2, Int n)	{ return Util_T<T>::caseCompare(s1, s2, n); }

	template<class T> AX_INLINE static bool		equals		(const T* s1, const T* s2)			{ return Util_T<T>::equals(s1, s2); }
	template<class T> AX_INLINE static bool		equals		(const T* s1, const T* s2, Int n)	{ return Util_T<T>::equals(s1, s2, n); }

	template<class T> AX_INLINE static bool		caseEquals	(const T* s1, const T* s2)			{ return Util_T<T>::caseEquals(s1, s2); }
	template<class T> AX_INLINE static bool		caseEquals	(const T* s1, const T* s2, Int n)	{ return Util_T<T>::caseEquals(s1, s2, n); }

	template<class T> AX_INLINE constexpr static bool	startsWith	(const T* s1, const T* s2)			{ return Util_T<T>::startsWith(s1, s2); }
	template<class T> AX_INLINE constexpr static bool	endsWith	(const T* s1, const T* s2)			{ return Util_T<T>::endsWith(s1, s2); }

	template<class T> AX_INLINE	constexpr static T*		findChar		(T* s1, T ch)					{ return Util_T<T>::findChar		(s1, ch); }
	template<class T> AX_INLINE	constexpr static T*		findCaseChar	(T* s1, T ch)					{ return Util_T<T>::findCaseChar	(s1, ch); }
	template<class T> AX_INLINE	constexpr static T*		findChars		(T* s1, const T* chs)			{ return Util_T<T>::findChars	(s1, chs); }
	template<class T> AX_INLINE constexpr static T*		findStr			(T* s1, const T* s2)			{ return Util_T<T>::findStr		(s1, s2); }
	template<class T> AX_INLINE constexpr static T*		findCaseStr		(T* s1, const T* s2)			{ return Util_T<T>::findCaseStr	(s1, s2); }

	template<class T> AX_INLINE constexpr static T*		findEndChar		(T* s1, T ch)					{ return Util_T<T>::findChar		(s1, ch); }
	template<class T> AX_INLINE constexpr static T*		findEndCaseChar	(T* s1, T ch)					{ return Util_T<T>::findCaseChar	(s1, ch); }
	template<class T> AX_INLINE constexpr static T*		findEndMultiChars	(T* s1, const T* chs)		{ return Util_T<T>::findChars	(s1, chs); }
	template<class T> AX_INLINE constexpr static T*		findEndStr		(T* s1, const T* s2)			{ return Util_T<T>::findStr		(s1, s2); }
	template<class T> AX_INLINE constexpr static T*		findEndCaseStr	(T* s1, const T* s2)			{ return Util_T<T>::findCaseStr	(s1, s2); }

	template<class T> AX_INLINE constexpr static T*		fileBasename	(T* sz)							{ return Util_T<T>::fileBasename(sz); }

	template<class T> AX_INLINE constexpr static T*		getHash(const T* s1)							{ return Util_T<T>::getHash(s1); }

	template<class T> AX_INLINE static bool		parse(const T* sz, bool &v)		{ return Util_T<T>::parse(sz, v); }
					 	 				
	template<class T> AX_INLINE static bool		parse(const T* sz, i8   &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, i16  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, i32  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, i64  &v)		{ return Util_T<T>::parse(sz, v); }

	template<class T> AX_INLINE static bool		parse(const T* sz, u8   &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, u16  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, u32  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, u64  &v)		{ return Util_T<T>::parse(sz, v); }

	template<class T> AX_INLINE static bool		parse(const T* sz, f16  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, f32  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, f64  &v)		{ return Util_T<T>::parse(sz, v); }
	template<class T> AX_INLINE static bool		parse(const T* sz, f128 &v)		{ return Util_T<T>::parse(sz, v); }

	template<class T> AX_INLINE static T*		copy	(T* s1, const T* s2)			{ return Util_T<T>::copy(s1, s2); }
	template<class T> AX_INLINE static T*		copy	(T* s1, const T* s2, Int n)		{ return Util_T<T>::copy(s1, s2, n); }
};



AX_PRAGMA_GCC(diagnostic push)
AX_PRAGMA_GCC(diagnostic ignored "-Wunsafe-buffer-usage")


template<class T> AX_INLINE
constexpr T* _ZStrUtil_T_Impl<T>::fileBasename(T* sz) {
	auto p = findCharsBack(sz, kPathSeparatorChars());
	return p ? p + 1 : sz;
}

template<class T> AX_INLINE
T* _ZStrUtil_T_Impl<T>::copy(T* dst, const T* src, Int n) {
	if( !dst || !src ) return nullptr;

	T* ret = dst;
	for(Int i=0; i<n; i++ ) {
		if( *src == 0 ) break;
		*dst = *src;
		dst++; src++;
	}
	*dst = T(0);
	return ret;
}

template<class T> AX_INLINE
T* _ZStrUtil_T_Impl<T>::copy( T* dst, const T* src ) {
	if( !dst || !src ) return nullptr;

	T* ret = dst;
	for(;;) {
		if( *src == 0 ) break;
		*dst = *src;
		dst++; src++;
	}
	*dst = T(0);
	return ret;
}


template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse(const char* sz, bool &v) {
	if (equals(sz, "true" )) { v = true;  return true; }
	if (equals(sz, "True" )) { v = true;  return true; }
	if (equals(sz, "TRUE" )) { v = true;  return true; }
	if (equals(sz, "false")) { v = false; return true; }
	if (equals(sz, "False")) { v = false; return true; }
	if (equals(sz, "FALSE")) { v = false; return true; }
	return false;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, u8    &v ) {
	u32 tmp = 0;
//	if( 1 != sscanf(sz, "%u", &tmp ) ) return false;
	if( 1 != sscanf_s(sz, "%u", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, u16    &v ) {
	u32 tmp;
//	if( 1 != sscanf(sz, "%u", &tmp ) ) return false;
	if( 1 != sscanf_s(sz, "%u", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, u32   &v ) {
//	if( 1 != sscanf(sz, "%u", &v ) ) return false;
	if( 1 != sscanf_s(sz, "%u", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, u64   &v ) {
	u64 tmp = 0;
//	if( 1 != sscanf(sz, "%llu", &tmp) ) return false;
	if( 1 != sscanf_s(sz, "%llu", &tmp) ) return false;
	v = static_cast<u64>(tmp);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, i8    &v ) {
	i32 tmp;
//	if( 1 != sscanf(sz, "%d", &tmp ) ) return false;
	if( 1 != sscanf_s(sz, "%d", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, i16    &v ) {
	i32 tmp;
//	if( 1 != sscanf(sz, "%d", &tmp ) ) return false;
	if( 1 != sscanf_s(sz, "%d", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, i32   &v ) {
//	if( 1 != sscanf(sz, "%d", &v ) ) return false;
	if( 1 != sscanf_s(sz, "%d", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, i64   &v ) {
	i64 tmp = 0;
//	if( 1 != sscanf(sz, "%lld", &tmp) ) return false;
	if( 1 != sscanf_s(sz, "%lld", &tmp) ) return false;
	v = static_cast<i64>(tmp);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, f32  &v ) {
//	if( 1 != sscanf(sz, "%f", &v ) ) return false;
	if( 1 != sscanf_s(sz, "%f", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, f64 &v ) {
//	if( 1 != sscanf(sz, "%lf", &v ) ) return false;
	if( 1 != sscanf_s(sz, "%lf", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, f128 &v ) {
//	if( 1 != sscanf(sz, "%Lf", &v ) ) return false;
	if( 1 != sscanf_s(sz, "%Lf", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<char>::parse( const char* sz, f16  &v ) {
	float tmp;
	if (!parse(sz, tmp)) return false;
	v = Float16(tmp);
	return true;
}

//==================  axCStr_parse wchar ============================
template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse(const wchar_t* sz, bool &v) {
	if (caseEquals(sz, L"true" )) { v = true;  return true; }
	if (caseEquals(sz, L"false")) { v = false; return true; }
	return false;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, u8    &v ) {
	u32 tmp;
//	if( 1 != swscanf( sz, L"%u", &tmp ) ) return false;
	if( 1 != swscanf_s( sz, L"%u", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, u16    &v ) {
	u32 tmp;
//	if( 1 != swscanf( sz, L"%u", &tmp ) ) return false;
	if( 1 != swscanf_s( sz, L"%u", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, u32   &v ) {
//	if( 1 != swscanf( sz, L"%u", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%u", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, u64   &v ) {
//	if( 1 != swscanf( sz, L"%llu", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%llu", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, i8    &v ) {
	i32 tmp;
//	if( 1 != swscanf( sz, L"%d", &tmp ) ) return false;
	if( 1 != swscanf_s( sz, L"%d", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, i16    &v ) {
	i32 tmp;
//	if( 1 != swscanf( sz, L"%d", &tmp ) ) return false;
	if( 1 != swscanf_s( sz, L"%d", &tmp ) ) return false;
	tmp = SafeCast(v);
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, i32   &v ) {
//	if( 1 != swscanf( sz, L"%d", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%d", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, i64   &v ) {
//	if( 1 != swscanf( sz, L"%lld", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%lld", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, f32  &v ) {
//	if( 1 != swscanf( sz, L"%f", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%f", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, f64 &v ) {
//	if( 1 != swscanf( sz, L"%lf", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%lf", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, f128 &v ) {
//	if( 1 != swscanf( sz, L"%Lf", &v ) ) return false;
	if( 1 != swscanf_s( sz, L"%Lf", &v ) ) return false;
	return true;
}

template<> AX_INLINE
bool _ZStrUtil_T_Impl<wchar_t>::parse( const wchar_t* sz, f16   &v ) {
	float tmp;
	if (!parse(sz, tmp)) return false;
	v = Float16(tmp);
	return true;
}

//==================  parse axChar16 / axChar32 ============================

// forward declare
constexpr Int Old_CStr_parse_impl_bufSize = 120;

template<class CHAR, class VAL> inline
bool Old_CStr_parse_impl(VAL& outValue, const CHAR* sz, Int szSize) {
	if (!sz) return false;

	const Int kBufSize = Old_CStr_parse_impl_bufSize;
	wchar_t buf[kBufSize + 1];

	Int copiedCount = 0;

	auto* dst = buf;
	auto* end = dst + kBufSize;
	auto* src = sz;
	for (; *src; src++, dst++) {
		if (copiedCount >= szSize) break;

		if (dst >= end) {
			AX_ASSERT(false); // buffer is too small
			return false;
		}
		Int v = *src;
		if (v > 255) {
			AX_ASSERT(false); // string should only contains ANSI for digit or bool("true"/"false")
			return false;
		}
		*dst = static_cast<wchar_t>(v);
		copiedCount++;
	}
	*dst = 0;
	buf[kBufSize] = 0; // ensure zero end
	return _ZStrUtil_T_Impl<CharW>::parse(buf, outValue);
}

#define AX_MACRO_OP(VAL)                                                                        \
	template<>                                                                                  \
	AX_INLINE bool _ZStrUtil_T_Impl<Char16>::parse(const Char16* sz, VAL& outValue) { \
		return Old_CStr_parse_impl(outValue, sz, Old_CStr_parse_impl_bufSize);                  \
	}                                                                                           \
	template<>                                                                                  \
	AX_INLINE bool _ZStrUtil_T_Impl<Char32>::parse(const Char32* sz, VAL& outValue) { \
		return Old_CStr_parse_impl(outValue, sz, Old_CStr_parse_impl_bufSize);                  \
	}                                                                                           \
//-----------
	AX_MACRO_OP(i8)
	AX_MACRO_OP(i16)
	AX_MACRO_OP(i32)
	AX_MACRO_OP(i64)
	AX_MACRO_OP(u8)
	AX_MACRO_OP(u16)
	AX_MACRO_OP(u32)
	AX_MACRO_OP(u64)
	AX_MACRO_OP(f16)
	AX_MACRO_OP(f32)
	AX_MACRO_OP(f64)
	AX_MACRO_OP(bool)

#undef AX_MACRO_OP

//=============================

template<class T> inline
bool _ZStrUtil_T_Impl<T>::startsWith(const T* full, const T* prefix) {
	if (!full || !prefix) return false;
	Int prefix_len = length(prefix);
	return equals(prefix, full, prefix_len);
}

template<class T> inline
bool _ZStrUtil_T_Impl<T>::endsWith(const T* full, const T* suffix) {
	if (!full || !suffix) return false;

	Int	suffix_len = length(suffix);
	Int	full_len = length(full);

	if (full_len < suffix_len) return false;
	Int n = full_len - suffix_len;
	const T* p = full + n;
	return equals(suffix, p, suffix_len);
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findBackStr(T* a, T* b) {
	if (!a || !b) return nullptr;

	Int alen = length(a);
	Int blen = length(b);

	if (blen > alen) return nullptr;

	for (T* p = a + alen - blen; p >= a; p--) {
		if (equals(p, b, blen)) {
			return p;
		}
	}
	return nullptr;
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findChar(T* sz, T ch) {
	if (!sz) return nullptr;

	for (; *sz; sz++) {
		if (*sz == ch) return sz;
	}
	return nullptr;
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findStr(T* a, T* b) {
	if (!a || !b) return nullptr;

	T* sa = a;
	T* sb = b;
	if (*b == 0) return nullptr;
	for (; *a; a++) {
		sa = a;
		sb = b;
		for (;;) {
			if (*sb == T(0)) return a; //found
			if (*sb != *sa) break;
			sa++; sb++;
		}
	}
	return nullptr;
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findCharBack(T* sz, T ch) {
	if (!sz) return nullptr;

	Int len = length(sz);
	if (len == 0) return nullptr;
	T *s = &sz[len - 1];
	for (; s >= sz; s--) {
		if (*s == ch) return s;
	}
	return nullptr;
}


template<class T> inline
Int _ZStrUtil_T_Impl<T>::compare( const T* s1, const T* s2 ) {
		if( s1 == s2 ) return 0;
		if( !s1 ) return -1;
		if( !s2 ) return  1;

		for( ;; s1++, s2++ ) {
			if( *s1 != *s2 ) return (*s1-*s2);
			if( *s1 == T(0) || *s2 == T(0) ) break;
		}
		return 0;
	}

template<class T> inline
Int _ZStrUtil_T_Impl<T>::compare( const T* s1, const T* s2, Int n ) {
	if( s1 == s2 ) return 0;
	if( !s1 ) return -1;
	if( !s2 ) return  1;

	Int i = 0;
	for( i=0; i<n; s1++, s2++, i++ ) {
		if( *s1 != *s2 ) return (*s1-*s2);
		if( *s1 == T(0) || *s2 == T(0) ) break;
	}
	return 0;
}

template<class T> inline
Int _ZStrUtil_T_Impl<T>::caseCompare(const T* s1, const T* s2) {
	if( s1 == s2 ) return 0;
	if( ! s1 ) return -1;
	if( ! s2 ) return  1;

	for( ;; s1++, s2++ ) {
		Int c1 = CharUtil::toLower(*s1);
		Int c2 = CharUtil::toLower(*s2);
		if( c1 != c2 ) return (c1-c2);
		if( c1 == T(0) || c2 == T(0) ) break;
	}
	return 0;
}

template<class T> inline
Int _ZStrUtil_T_Impl<T>::caseCompare(const T* s1, const T* s2, Int n) {
	if( s1 == s2 ) return 0;
	if( ! s1 ) return -1;
	if( ! s2 ) return  1;

	for(Int i=0; i<n; s1++, s2++, i++ ) {
		if (auto d = charCaseCompare(*s1, *s2)) {
			return d;
		}
		if( *s1 == T(0) || *s2 == T(0) ) break;
	}
	return 0;
}

template<class T> inline 
T* _ZStrUtil_T_Impl<T>::findChars(T* sz, const T* chrs) {
	if (!sz || !chrs) return nullptr;

	T* p = sz;
	for (; *p; p++) {
		for (const T* c = chrs; *c; c++) {
			if (*p == *c) return p;
		}
	}
	return nullptr;
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findCharsBack(T* sz, const T* chrs) {
	if (!*sz || !chrs) return nullptr;

	Int len = length(sz);
	T* p = sz + len - 1;
	for (; p >= sz; p--) {
		for (const T* c = chrs; *c; c++) {
			if (*p == *c) return p;
		}
	}
	return nullptr;
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findCaseStr(T* big, const T* little) {
	if (!big || !little) return nullptr;

	Int big_len = length(big);
	Int little_len = length(little);

	if (little_len > big_len) return nullptr;

	Int n = big_len - little_len;
	for (Int i = 0; i <= n; i++) {
		if (caseCompare(big + i, little, little_len) == 0)
			return big + i;
	}
	return nullptr;
}

template<class T> inline
T* _ZStrUtil_T_Impl<T>::findCaseChar(T* sz, T ch) {
	if (!sz) return nullptr;

	T c = toUpper(ch);
	for (; *sz; sz++) {
		if (toUpper(*sz) == c) return sz;
	}
	return nullptr;
}

AX_PRAGMA_GCC(diagnostic pop)


} // namespace
