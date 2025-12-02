module;
#include "AxCore-pch.h"

export module AxCore.StrView;
export import AxCore.BasicType;
export import AxCore.HashInt;
export import AxCore.Span;
import AxCore.Math;

export namespace ax {

template<class T> class MutStrView_;
using MutStrView   = MutStrView_<Char  >;
using MutStrViewA  = MutStrView_<CharA >;
using MutStrViewW  = MutStrView_<CharW >;
using MutStrView8  = MutStrView_<Char8 >;
using MutStrView16 = MutStrView_<Char16>;
using MutStrView32 = MutStrView_<Char32>;

template<class T> using StrView_ = MutStrView_<const T>;
using StrView   = StrView_<Char  >;
using StrViewA  = StrView_<CharA >;
using StrViewW  = StrView_<CharW >;
using StrView8  = StrView_<Char8 >;
using StrView16 = StrView_<Char16>;
using StrView32 = StrView_<Char32>;

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

template<class T> constexpr bool Type_IsConvertiableToStrViewT	=  std::is_convertible_v<T, StrViewA>
																|| std::is_convertible_v<T, StrViewW>
																|| std::is_convertible_v<T, StrView8>
																|| std::is_convertible_v<T, StrView16>
																|| std::is_convertible_v<T, StrView32>;

template<class CH, class T>
struct StrView_ParseHandler {
	static bool tryParse(StrView_<CH> str, T& obj) {
		return obj.onStrParse(str);
	}
};

template<class CH, class T> requires Type_IsFundamental<T>
struct StrView_ParseHandler<CH, T> {
	static bool tryParse(StrView_<CH> str, T& obj) {
		std::from_chars_result result = std::from_chars(str.begin(), str.end(), obj);
		if (result.ec == std::errc()) return true;
		return false;
	}
};

template<class CH, class T>
bool StrView_parse(StrView_<CH> sv, T& obj) { return StrView_ParseHandler<CH, T>::tryParse(sv, obj); }

template <class T>
class MutStrView_ { //Copyable
	using This = MutStrView_;
protected:
	T*  _data = nullptr;
	Int _size = 0;	
public:
	using CharType        = T;
	using CSpan           = Span<T>;
	using MSpan           = MutSpan<T>;
	using MView           = MutStrView_<T>;
	using CView           = MutStrView_<const T>;
	using std_string_view = std::basic_string_view<std::remove_cv_t<T>>;
	
	constexpr MutStrView_() = default;
	constexpr MutStrView_(T* data, Int size) noexcept : _data(data), _size(size) {}
	constexpr MutStrView_(MutStrLit_<T> r) noexcept : _data(r.data()), _size(r.size()) {}
	
	template<Int N>
	AX_INLINE constexpr MutStrView_(T (&sz)[N]) noexcept : _data(sz), _size(N > 0 ? N-1 : 0) {}

	explicit operator bool() const { return size() > 0; }

	//--------------
	AX_INLINE constexpr       T*	data()       noexcept 				{ return _data; }
	AX_INLINE constexpr const T*	data() const noexcept 				{ return _data; }
	AX_INLINE constexpr Int      	size() const noexcept 				{ return _size; }
	AX_INLINE constexpr Int	     	sizeInBytes() const noexcept		{ return _size * AX_SIZEOF(T); }
	template<class R>
	AX_INLINE constexpr bool		isOverlapped(StrView_<R> r) const	{ return toByteSpan().isOverlapped(r.toByteSpan()); }
	AX_INLINE constexpr	bool		inBound(Int i) const				{ return i >= 0 && i < size(); }
	AX_INLINE constexpr			T&	operator[]	(Int i)					{ return at(i); }
	AX_INLINE constexpr	const	T&	operator[]	(Int i) const			{ return at(i); }
	AX_INLINE constexpr			T&	at			(Int i)					{ _checkBound(i); return unsafe_at(i); }
	AX_INLINE constexpr	const	T&	at			(Int i) const			{ _checkBound(i); return unsafe_at(i); }
	AX_INLINE constexpr 		T&	back		(Int i = 0)				{ return at( size()-i-1 ); }
	AX_INLINE constexpr const	T&	back		(Int i = 0) const		{ return at( size()-i-1 ); }
	AX_INLINE constexpr 		T&	unsafe_at	(Int i)	noexcept		{ _debug_checkBound(i); return data()[i]; }
	AX_INLINE constexpr const	T&	unsafe_at	(Int i) const noexcept	{ _debug_checkBound(i); return data()[i]; }
	AX_INLINE constexpr 		T&	unsafe_back	(Int i)					{ return unsafe_at( size()-i-1 ); }
	AX_INLINE constexpr const	T&	unsafe_back	(Int i)  const			{ return unsafe_at( size()-i-1 ); }
	//---------------
	AX_INLINE constexpr CView	constView	() const noexcept	{ return CView(data(), size()); }
	AX_INLINE constexpr CView	view		() const noexcept	{ return CView(data(), size()); }
	AX_INLINE constexpr MView	view		()       noexcept	{ return MView(data(), size()); }
	AX_INLINE constexpr operator CView() const noexcept { return constView(); }

	AX_INLINE constexpr CSpan constSpan() const noexcept	{ return CSpan(data(), size()); }
	AX_INLINE constexpr CSpan      span() const noexcept	{ return CSpan(data(), size()); }
	AX_INLINE constexpr MSpan      span() noexcept			{ return MSpan(data(), size()); }
	AX_INLINE constexpr operator CSpan() const noexcept { return constSpan(); }
	//---------------
	AX_INLINE constexpr static MView	s_fromMutByteSpan	(MutByteSpan	from) noexcept	{ return MView(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / AX_SIZEOF(T)); }
	AX_INLINE constexpr void			  fromMutByteSpan	(MutByteSpan	from) noexcept	{ *this = s_fromMutByteSpan(from); }
	AX_INLINE constexpr MutByteSpan		toMutByteSpan		()       noexcept { return MutByteSpan((      Byte*)_data, sizeInBytes()); }
	AX_INLINE constexpr    ByteSpan		   toByteSpan		() const noexcept { return    ByteSpan((const Byte*)_data, sizeInBytes()); }
	
	constexpr std_string_view to_string_view() const noexcept { return std_string_view(_data, _size); }
	constexpr operator std_string_view() const noexcept { return to_string_view(); }

	//                +--------------------------------------------+
	//                |         |                      |           |
	//                +--------------------------------------------+
	//  slice         (  offset  )[______ newSize ____]
	//  sliceBack                       [________ newSize__________]
	//  sliceTrim     (  offset  )[_________till to end ___________]
	//  sliceTrimBack [________________________________](  offset  )
	AX_INLINE	constexpr MView	slice			(Int offset, Int newSize) {
		if (offset < 0 || newSize < 0 || offset + newSize > _size) throw Error_IndexOutOfRange();
		return MView(_data + offset, newSize);
	}
	AX_INLINE constexpr CView     slice	        (Int offset, Int newSize) const	{ return ax_const_cast(this)->slice(offset, newSize); }
	AX_INLINE constexpr MView     slice	        (IntRange range)				{ return slice(range.start, range.size); }
	AX_INLINE constexpr CView     slice	        (IntRange range) const			{ return slice(range.start, range.size); }
	AX_INLINE constexpr MView     sliceTrim	    (Int offset)					{ return slice(offset, _size - offset); }
	AX_INLINE constexpr CView     sliceTrim	    (Int offset) const				{ return slice(offset, _size - offset); }
	AX_INLINE constexpr MView     sliceBack	    (Int newSize)			 		{ return slice(_size - newSize, newSize); }
	AX_INLINE constexpr CView     sliceBack	    (Int newSize) const	 			{ return slice(_size - newSize, newSize); }
	AX_INLINE constexpr MView     sliceTrimBack	(Int offset)					{ return slice(0, _size - offset); }
	AX_INLINE constexpr CView     sliceTrimBack	(Int offset) const				{ return slice(0, _size - offset); }
	//----------------
	inline    constexpr CmpResult compare      (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      equals       (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return compare(r, sc) == CmpResult::Equal; }
	AX_INLINE constexpr bool      startsWith   (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      endsWith     (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	inline    constexpr bool      matchWildcard(CView wildcard, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool operator==	(CView r) const	{ return  equals(r); }
	AX_INLINE constexpr bool operator!=	(CView r) const	{ return !equals(r); }
	AX_INLINE constexpr bool operator<	(CView r) const	{ return CmpResult_isLesser        (compare(r)); }
	AX_INLINE constexpr bool operator<=	(CView r) const	{ return CmpResult_isLesserOrEqual (compare(r)); }
	AX_INLINE constexpr bool operator>	(CView r) const	{ return CmpResult_isGreater       (compare(r)); }
	AX_INLINE constexpr bool operator>=	(CView r) const	{ return CmpResult_isGreaterOrEqual(compare(r)); }
	//----------------

	template<class R>
	bool	tryParse	(R & outValue) const { return StrView_parse(*this, outValue); }
	
	AX_NODISCARD AX_INLINE constexpr HashInt onHashInt() const noexcept { return HashInt::s_make(span()); }
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()		 noexcept	{ return _data; }
	constexpr CIter	begin	() const noexcept	{ return _data; }
	constexpr  Iter	end		()		 noexcept	{ return _data + _size; }
	constexpr CIter	end		() const noexcept	{ return _data + _size; }

private:
	AX_INLINE constexpr void _checkBound		( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE constexpr void _debug_checkBound	( Int i ) const {
#ifdef _DEBUG
		_checkBound(i);
#endif
	}
};

template<class T> constexpr bool Type_IsMutStrView = false;
template<class T> constexpr bool Type_IsMutStrView<MutStrView_<T>> = true;

template<class T> constexpr bool Type_IsStrView = false; 
template<class T> constexpr bool Type_IsStrView<StrView_<T>> = true; 

// Null-terminated string, should use StrLit or StrView if possible
template<class T>
class MutZStrView_ : public MutStrView_<T> {
	using This = MutZStrView_;
	using Base = MutStrView_<T>;
protected:
	using Base::_data;
	using Base::_size;
public:
	using CView = MutZStrView_<const T>;

	AX_INLINE constexpr MutZStrView_() = default;
	AX_INLINE constexpr MutZStrView_(T* sz, Int size) : Base(sz, size) {}
	
	constexpr CView	constView() const { return CView(_data, _size); }

	constexpr const T* c_str() const { return _size ? _data : &_empty_c_str; }

private:
	static constexpr T _empty_c_str = 0;
};

AX_INLINE consteval ZStrViewA  operator ""_sv(const CharA * sz, size_t n)  noexcept { return ZStrViewA (sz, n); }
AX_INLINE consteval ZStrViewW  operator ""_sv(const CharW * sz, size_t n)  noexcept { return ZStrViewW (sz, n); }
AX_INLINE consteval ZStrView8  operator ""_sv(const Char8 * sz, size_t n)  noexcept { return ZStrView8 (sz, n); }
AX_INLINE consteval ZStrView16 operator ""_sv(const Char16* sz, size_t n)  noexcept { return ZStrView16(sz, n); }
AX_INLINE consteval ZStrView32 operator ""_sv(const Char32* sz, size_t n)  noexcept { return ZStrView32(sz, n); }

template<class T> AX_INLINE	constexpr 
bool MutStrView_<T>::startsWith(CView r, StrCase sc) const noexcept {
	if (_size < r.size()) return false;
	return ax_const_cast(this)->slice(0, r.size()).equals(r, sc);
}

template<class T> AX_INLINE	constexpr 
bool MutStrView_<T>::endsWith(CView r, StrCase sc) const noexcept {
	if (_size < r.size()) return false;
	return ax_const_cast(this)->sliceBack(r.size()).equals(r, sc);
}

template<class T> AX_INLINE
constexpr StrView_<T> StrView_make(std::basic_string_view<T> src) { return StrView_<T>(src.data(), src.size()); }

template<class T, Int N> AX_INLINE
constexpr StrView_<T> StrView_make(T (&sz)[N]) { return StrView_<T>(sz, N > 0 ? N-1 : 0); }

template <class T> inline
constexpr CmpResult MutStrView_<T>::compare(CView r, StrCase sc) const noexcept {
	if (_size > 0 && _size == r.size() && _data == r.data())
		return CmpResult::Equal;

	Int n = Math::min(_size, r.size());
	const T* p0 = _data;
	const T* p1 = r.data();

	if (sc == StrCase::Ignore) {
		Int i = 0;
		for( i=0; i<n; ++p0, ++p1, i++ ) {
			auto c0 = std::tolower(*p0);
			auto c1 = std::tolower(*p1);
			if (c0 != c1 ) return CmpResult_fromInt(c0 - c1);
			if (c0 == T(0) || c1 == T(0)) break;
		}
	} else {
		Int i = 0;
		for( i=0; i<n; ++p0, ++p1, i++ ) {
			const auto c0 = (*p0);
			const auto c1 = (*p1);
			if (c0 != c1 ) return CmpResult_fromInt(c0 - c1);
			if (c0 == T(0) || c1 == T(0)) break;
		}
	}

	return CmpResult_fromInt(_size - r.size());
}

template <class T> inline
constexpr bool MutStrView_<T>::matchWildcard(CView wildcard, StrCase sc) const noexcept {
	const auto* p = begin();
	const auto* e = end();

	const auto* w = wildcard.begin();
	const auto* wEnd = wildcard.end();

	while (p < e && w < wEnd) {
		if (*w == T('?')) { ++p; ++w; continue; }
		if (sc == StrCase::Ignore) {
			if (ax_char_ignore_case_equals(*w, *p)) { ++p; ++w; continue; }
		}else{
			if (*w == *p) { ++p; ++w; continue; }
		}
		
		if (*w == T('*')) {
			auto w1 = w + 1;
			if (w1 >= wEnd) return true; // * is the last

			auto p1 = p + 1;
			if (p1 >= e) return false;

			p = p1;
			if (*p1 == *w1) {
				w = w1; //next wildcard
			}
			continue;
		}
		return false;
	}
	return p == e && w == wEnd;
}

template<class T> inline
std::ostream& operator << ( std::ostream & s, const MutStrView_<T> & v ) { return s << v.to_string_view(); }

} // namespace
