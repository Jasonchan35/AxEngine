module;
#include "AxPlatform-pch.h"

export module AxPlatform.UtfUtil;
export import AxPlatform.IString;

namespace ax {

export struct UtfUtil {
	UtfUtil() = delete;

	template<class DST, class SRC>
	AX_INLINE static void	append(IString_<DST>& dst, StrView_<SRC>  src);
	
	template<class SRC, class DST> AX_INLINE
	static Int	getConvertedCount ( const SRC* src, Int src_len );
private:
	template< class SRC > static Int	_utfCount 	( CharU v );
	template< class SRC > static CharU 	_decodeUtf	( const SRC* & src, const SRC* e );
	template< class DST > static void 	_encodeUtf	( IString_<DST>& dst, CharU v );
};

template<class SRC, class DST> inline
Int UtfUtil::getConvertedCount(const SRC* src, Int src_len) {
	if constexpr (sizeof(DST) == sizeof(SRC)) {
		return src_len;
	}

	Int	  out_len = 0;
	auto* srcEnd  = src + src_len;
	for (Int i = 0; i < src_len; i++) {
		if (src >= srcEnd) break;
		out_len += _utfCount<DST>(_decodeUtf(src, srcEnd));
	}
	return out_len;
}

template<> inline
void UtfUtil::_encodeUtf(IString_<Char16>& dst, CharU ch) {
	const auto& v = ch;
	if (v < 0x10000) {
		dst.append(static_cast<Char16>(v));
		return;
	}

	if (v < 0x110000) {
		dst.append(static_cast<Char16>((v >> 10)   + 0xD800));
		dst.append(static_cast<Char16>((v & 0x3FF) + 0xDC00));
		return;
	}

	{
		AX_ASSERT(false);
		dst.append(static_cast<Char16>(v));
		return;
	}
}

template<class DST, class SRC> inline
void UtfUtil::append(IString_<DST>& dst, StrView_<SRC> src) {
	if constexpr (std::is_same_v<DST, SRC>) {
		auto old_size = dst.size();
		dst.resize(old_size + src.size());
		MemUtil::copy(dst.data() + old_size, src.data(), src.size());
	} else {
		auto		src_len = src.size();
		const auto* s		= src.begin();
		const auto* e		= src.end();
		for (Int i = 0; i < src_len; i++) {
			if (s >= e) break;
			_encodeUtf(dst, _decodeUtf(s, e));
		}
	}
}

template<> AX_INLINE
CharU UtfUtil::_decodeUtf<Char32>( const Char32* & src, const Char32* end) {
	static_assert(sizeof(Char32) == sizeof(CharU));
	CharU a = *src; src++;
	return a;
}

template<> AX_INLINE
CharU UtfUtil::_decodeUtf<Char16>( const Char16* & src, const Char16* end ) {
	auto v = static_cast<uint16_t>(*src);

	if (v >= 0xD800 && v < 0xDBFF) {
		if( src+2 > end ) throw Error_Utf();
		CharU a = static_cast<CharU>(*src); src++;
		CharU b = static_cast<CharU>(*src); src++;
		return ((a - 0xD800) << 10) | (b - 0xDC00);
	}

	CharU a = static_cast<CharU>(*src); src++;
	return a;
}

template<> AX_INLINE
CharU UtfUtil::_decodeUtf<Char8>( const Char8* & src, const Char8* end ) {
	const auto v = static_cast<uint8_t>(*src);
	CharU o = 0;
	if (v < 0x80) {
		o = static_cast<uint8_t>(*src); src++;
		return o;
	}

	if ((v & 0xE0) == 0xC0) {
		if( src+2 > end ) throw Error_Utf();
		o += (static_cast<uint8_t>(*src) & 0x1FU) << 6; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);      src++;
		return o;
	}

	if ((v & 0xF0) == 0xE0) {
		if( src+3 > end ) throw Error_Utf();
		o += (static_cast<uint8_t>(*src) & 0x0FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
		return o;
	}

	if ((v & 0xF8) == 0xF0) {
		if( src+4 > end ) throw Error_Utf();
		o += (static_cast<uint8_t>(*src) & 0x07U) << 18; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
		return o;
	}

	if ((v & 0xFC) == 0xF8) {
		if( src+5 > end ) throw Error_Utf();
		o += (static_cast<uint8_t>(*src) & 0x03U) << 24; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 18; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
	}

	if ((v & 0xFE) == 0xFC) {
		if( src+6 > end ) throw Error_Utf();
		o += (static_cast<uint8_t>(*src) & 0x01U) << 30; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 24; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 18; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 12; src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU) << 6;  src++;
		o += (static_cast<uint8_t>(*src) & 0x3FU);       src++;
		return o;
	}

	{
		AX_ASSERT(false);
		o = static_cast<uint8_t>(*src); src++;
		return o;
	}
}

template <>
AX_INLINE CharU UtfUtil::_decodeUtf<CharA>(const CharA* & src, const CharA* end) {
	static_assert(sizeof(Char8) == sizeof(CharA));
	return _decodeUtf<Char8>(reinterpret_cast<const Char8* &>(src), reinterpret_cast<const Char8* &>(end));
}

template <>
AX_INLINE CharU UtfUtil::_decodeUtf<CharW>(const CharW* & src, const CharW* end) {
	return _decodeUtf<CharW_Native>(*CharW_to_Native(&src), CharW_to_Native(end));
}

//----------------------------------------------------

template<> AX_INLINE
Int UtfUtil::_utfCount< Char32 >(CharU v) {
	return 1;
}

template<> AX_INLINE
Int UtfUtil::_utfCount< Char16 >(CharU ch) {
	const auto& v = ch;
	if( v <  0x10000 ) return 1;
	if( v < 0x110000 ) return 2;

	AX_ASSERT(false);
	return 1;
}

template<> AX_INLINE
Int UtfUtil::_utfCount< CharA >(CharU ch) {
	const auto& v = ch;
	if( v <       0x80 ) return 1;
	if( v <    0x00800 ) return 2;
	if( v <    0x10000 ) return 3;
	if( v <   0x200000 ) return 4;
// The patterns below are not part of UTF-8, but were part of the first specification.
	if( v <  0x4000000 ) return 5;
	if( v < 0x80000000 ) return 6;

	AX_ASSERT(false);
	return 1;
}

template<> AX_INLINE
Int UtfUtil::_utfCount< CharW >( CharU v ) {
	return _utfCount<CharW_Native>(v);
}

template<> AX_INLINE
void UtfUtil::_encodeUtf< Char8 >( IString_<Char8>& dst, CharU ch) {
	const auto& v = ch;
	if( v <       0x80 ) {
		dst.append(Char8(static_cast<uint8_t>(v)));
		return;
	}

	if( v <    0x00800 ) {
		Char8 t[] = {
			Char8(static_cast<uint8_t>(( v >> 6   ) | 0xC0)),
			Char8(static_cast<uint8_t>(( v & 0x3F ) | 0x80)),
			Char8(0)
		};
		dst.append(t);
		return;
	}

	if( v <    0x10000 ) {
		Char8 t[] = {
			Char8(static_cast<uint8_t>(( (v >> 12)        ) | 0xE0)),
			Char8(static_cast<uint8_t>(( (v >> 6 ) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>((  v        & 0x3F ) | 0x80)),
			Char8(0)
		};
		dst.append(t);
		return;
	}

	if( v <   0x200000 ) {
		Char8 t[] = {
			Char8(static_cast<uint8_t>(( (v >> 18)        ) | 0xF0)),
			Char8(static_cast<uint8_t>(( (v >> 12) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(( (v >> 6 ) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>((  v        & 0x3F ) | 0x80)),
			Char8(0)
		};
		dst.append(t);
		return;
	}

// The patterns below are not part of UTF-8, but were part of the first specification.
	if( v <  0x4000000 ) {
		Char8 t[] = {
			Char8(static_cast<uint8_t>(((v >> 24)        ) | 0xF8)),
			Char8(static_cast<uint8_t>(((v >> 18) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(((v >> 12) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(((v >> 6 ) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(( v        & 0x3F ) | 0x80)),
			Char8(0)
		};
		dst.append(t);
		return;
	}

	if( v < 0x80000000 ) {
		Char8 t[] = {
			Char8(static_cast<uint8_t>(((v >> 30)        ) | 0xFC)),
			Char8(static_cast<uint8_t>(((v >> 24) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(((v >> 18) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(((v >> 12) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(((v >> 6 ) & 0x3F ) | 0x80)),
			Char8(static_cast<uint8_t>(( v        & 0x3F ) | 0x80)),
			Char8(0)
		};
		dst.append(t);
		return;
	}

	{
		AX_ASSERT(false);
		dst.append(CharA(static_cast<uint8_t>(v)));
	}
}

template<> AX_INLINE
void UtfUtil::_encodeUtf< Char32 >( IString_<Char32>& dst, CharU v ) {
	static_assert(sizeof(Char32) == sizeof(v));
	dst.append(v);
}

template<> AX_INLINE
void UtfUtil::_encodeUtf< CharW >( IString_<CharW>& dst, CharU v ) {
	_encodeUtf(reinterpret_cast< IString_<CharW_Native>& >(dst), v ); // char32_t on unix
}

template<> AX_INLINE
void UtfUtil::_encodeUtf< CharA >( IString_<CharA>& dst, CharU v ) {
	_encodeUtf(reinterpret_cast< IString_<Char8>& >(dst), v );
}

} // namespace

