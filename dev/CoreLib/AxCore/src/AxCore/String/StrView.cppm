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

template<class CH, class T> requires Type_IsEnum<T>
struct StrView_ParseHandler<CH, T> {
	constexpr static bool tryParse(StrView_<CH> str, T& obj) {
		return _ax_macro_enum_try_parse(str, obj);
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
//	constexpr MutStrView_(T& ch) noexcept : _data(&ch), _size(1) {}
	
	template <Int N>
	constexpr MutStrView_(T (&sz)[N]) noexcept : _data(sz), _size( N > 0 ? N - 1 : 0) {}
	
	AX_INLINE constexpr void setPtr(T* data, Int size) noexcept { _data = data; _size = size; }
	AX_NODISCARD constexpr bool samePtrAndSize(CSpan r) const noexcept { return _size == r._size && _data == r._data; }
		
	static constexpr MView s_empty() { return MutStrView_(); }
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
	//  sliceFrom     (  offset  )[_________till to end ___________]
	//  sliceFromBack [________________________________](  offset  )
	AX_INLINE constexpr MView	  slice         (Int offset, Int newSize) {
		if (offset < 0 || newSize < 0 || offset + newSize > _size) {
			if (std::is_constant_evaluated()) {
				AX_ASSERT(false);
			} else {
				throw Error_IndexOutOfRange();
			}
		}
		return MView(_data + offset, newSize);
	}
	AX_INLINE constexpr CView     slice	        (Int offset, Int newSize) const	{ return ax_const_cast(this)->slice(offset, newSize); }
	AX_INLINE constexpr MView     slice	        (IntRange range)				{ return slice(range.start, range.size); }
	AX_INLINE constexpr CView     slice	        (IntRange range) const			{ return slice(range.start, range.size); }
	AX_INLINE constexpr MView     sliceFrom	    (Int offset)					{ return slice(offset, _size - offset); }
	AX_INLINE constexpr CView     sliceFrom	    (Int offset) const				{ return slice(offset, _size - offset); }
	AX_INLINE constexpr MView     sliceBack	    (Int newSize)			 		{ return slice(_size - newSize, newSize); }
	AX_INLINE constexpr CView     sliceBack	    (Int newSize) const	 			{ return slice(_size - newSize, newSize); }
	AX_INLINE constexpr MView     sliceFromBack	(Int offset)					{ return slice(0, _size - offset); }
	AX_INLINE constexpr CView     sliceFromBack	(Int offset) const				{ return slice(0, _size - offset); }
	//----------------
	AX_INLINE constexpr CmpResult compare      (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	constexpr bool                equals       (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      startsWith   (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      endsWith     (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	inline    constexpr bool      matchWildcard(CView wildcard, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      operator==	(CView r) const	{ return  equals(r); }
	AX_INLINE constexpr bool      operator!=	(CView r) const	{ return !equals(r); }
	AX_INLINE constexpr bool      operator<		(CView r) const	{ return CmpResult_isLesser        (compare(r)); }
	AX_INLINE constexpr bool      operator<=	(CView r) const	{ return CmpResult_isLesserOrEqual (compare(r)); }
	AX_INLINE constexpr bool      operator>		(CView r) const	{ return CmpResult_isGreater       (compare(r)); }
	AX_INLINE constexpr bool      operator>=	(CView r) const	{ return CmpResult_isGreaterOrEqual(compare(r)); }
	//----------------
	AX_NODISCARD constexpr Opt<Int>	find			(CView      str, StrCase sc = StrCase::Sensitive) const;
	AX_NODISCARD constexpr Opt<Int>	findBack		(CView      str, StrCase sc = StrCase::Sensitive) const;
	AX_NODISCARD constexpr Opt<Int>	findChar		(const T&    ch, StrCase sc = StrCase::Sensitive) const;
	AX_NODISCARD constexpr Opt<Int>	findCharBack	(const T&    ch, StrCase sc = StrCase::Sensitive) const;
	AX_NODISCARD constexpr Opt<Int>	findAnyChar		(Span<T> chList, StrCase sc = StrCase::Sensitive) const;
	AX_NODISCARD constexpr Opt<Int>	findAnyCharBack	(Span<T> chList, StrCase sc = StrCase::Sensitive) const;
	//----------------
	using SplitResult = Pair<MView, MView>;
	AX_NODISCARD constexpr auto splitByIndex		(Opt<Int> index, Int separatorSize) -> SplitResult;
	AX_NODISCARD constexpr auto split				(CView      str, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitBack			(CView      str, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitByChar			(const T&    ch, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitByCharBack		(const T&    ch, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitByAnyChar		(Span<T> chList, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitByAnyCharBack	(Span<T> chList, StrCase sc = StrCase::Sensitive) -> SplitResult;
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

	static constexpr This s_from_c_str(T *sz) { return This(sz, ax_strlen(sz)); } 
	
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

template <class T>
AX_NODISCARD AX_INLINE constexpr StrView_<T> StrView_ref(Span<T> src) noexcept {
	return StrView_<T>(src.data(), src.size());
}

template <class T>
AX_NODISCARD AX_INLINE constexpr StrView_<T> StrView_ref(MutSpan<T> src) noexcept {
	return StrView_<T>(src.data(), src.size());
}

template <class T>
AX_NODISCARD AX_INLINE constexpr StrView_<T> StrView_ref(std::basic_string_view<T> src) noexcept {
	return StrView_<T>(src.data(), src.size());
}

template <class T>
AX_INLINE constexpr StrView_<T> StrView_c_str(const T* sz) noexcept { return StrView_<T>(sz, ax_strlen(sz)); }

template <class T>
constexpr CmpResult MutStrView_<T>::compare(CView r, StrCase sc) const noexcept {
	if (sc == StrCase::Ignore) {
		return span().compare(r.span(),	[](auto& a , auto& b) { return CharUtil::compare_<StrCase::Ignore>(a,b); } );
	} else {
		return span().compare(r.span(),	[](auto& a , auto& b) { return CharUtil::compare_<StrCase::Sensitive>(a,b); } );
	}
}

template <class T>
constexpr bool MutStrView_<T>::equals(CView r, StrCase sc) const noexcept {
	if (_size != r.size()) return false;
	return compare(r, sc) == CmpResult::Equal;
}

template <class T>
constexpr bool MutStrView_<T>::matchWildcard(CView wildcard, StrCase sc) const noexcept {
	const auto* p = begin();
	const auto* e = end();

	const auto* w = wildcard.begin();
	const auto* wEnd = wildcard.end();

	while (p < e && w < wEnd) {
		if (*w == T('?')) { ++p; ++w; continue; }
		if (CharUtil::equals(*w, *p, sc)) { ++p; ++w; continue; }
		
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

template <class T>
constexpr Opt<Int> MutStrView_<T>::find(CView str, StrCase sc) const {
	Int sliceSize = str.size();
	if (sliceSize > _size) return std::nullopt;
	Int loop = size() - str.size() + 1;
	
	for (Int i = 0; i < loop; ++i) {
		auto tmp = slice(i, sliceSize);
		auto ret = tmp.compare(str, sc);
		if (ret == CmpResult::Equal) {
			return i;
		}
	}

	return std::nullopt;
}

template <class T>
constexpr Opt<Int> MutStrView_<T>::findBack(CView str, StrCase sc) const {
	Int sliceSize = str.size();
	if (sliceSize > _size) return std::nullopt;
	Int loop = size() - str.size() + 1;

	static_assert(std::is_signed_v<Int>);
	for (Int i = loop - 1; i >= 0; --i) {
		auto ret = slice(i, sliceSize).compare(str, sc);
		if (ret == CmpResult::Equal) return i;
	}

	return std::nullopt;
}

template <class T>
constexpr Opt<Int> MutStrView_<T>::findChar(const T& ch, StrCase sc) const {
	Int loop = size();
	if (sc == StrCase::Ignore) {
		for (Int i = 0; i < loop; ++i) {
			if (CharUtil::equals_<StrCase::Ignore>(at(i), ch)) return i;
		}
	} else {
		for (Int i = 0; i < loop; ++i) {
			if (at(i) == ch) return i;
		}
	}

	return std::nullopt;
}

template <class T>
constexpr Opt<Int> MutStrView_<T>::findCharBack(const T& ch, StrCase sc) const {
	Int loop = size();
	
	if (sc == StrCase::Ignore) {
		static_assert(std::is_signed_v<Int>);
		for (Int i = loop-1; i >=0; --i) {
			if (CharUtil::equals_i(at(i), ch)) return i;
		}
	} else {
		static_assert(std::is_signed_v<Int>);
		for (Int i = loop-1; i >=0; --i) {
			if (at(i) == ch) return i;
		}
	}

	return std::nullopt;
}

template <class T>
constexpr Opt<Int> MutStrView_<T>::findAnyChar(Span<T> chList, StrCase sc) const {
	Int loop = size();
	if (sc == StrCase::Ignore) {
		for (Int i = 0; i < loop; ++i) {
			for (auto & ch : chList) {
				if (CharUtil::equals_i(at(i), ch)) return i;
			}
		}
	} else {
		for (Int i = 0; i < loop; ++i) {
			for (auto & ch : chList) {
				if (at(i) == ch) return i;
			}
		}
	}

	return std::nullopt;
}

template <class T>
constexpr Opt<Int> MutStrView_<T>::findAnyCharBack(Span<T> chList, StrCase sc) const {
	Int loop = size();
	if (sc == StrCase::Ignore) {
		static_assert(std::is_signed_v<Int>);
		for (Int i = loop-1; i >=0; --i) {
			for (auto & ch : chList) {
				if (CharUtil::equals_i(at(i), ch)) return i;
			}
		}
	} else {
		static_assert(std::is_signed_v<Int>);
		for (Int i = loop-1; i >=0; --i) {
			for (auto & ch : chList) {
				if (at(i) == ch) return i;
			}
		}
	}

	return std::nullopt;
}

template <class T>
constexpr auto MutStrView_<T>::splitByIndex(Opt<Int> index, Int separatorSize) -> SplitResult {
	if (_size <= 0 || !index || index.value() < 0) {
		return SplitResult(*this, s_empty());
	} else if (index.value() >= _size) {
		return SplitResult(s_empty(), *this);
	} else {
		Int second = Math::min(index.value() + separatorSize, _size - 1);
		return SplitResult(slice(0, index.value()), sliceFrom(second));
	}
}

template <class T>
constexpr auto MutStrView_<T>::split(CView str, StrCase sc) -> SplitResult {
	return splitByIndex(find(str, sc), str.size());
}

template <class T>
constexpr auto MutStrView_<T>::splitBack(CView str, StrCase sc) -> SplitResult {
	return splitByIndex(findBack(str, sc), str.size());
}

template <class T>
constexpr auto MutStrView_<T>::splitByChar(const T& ch, StrCase sc) -> SplitResult {
	return splitByIndex(findChar(ch, sc), 1);
}

template <class T>
constexpr auto MutStrView_<T>::splitByCharBack(const T& ch, StrCase sc) -> SplitResult {
	return splitByIndex(findCharBack(ch, sc), 1);
}

template <class T>
constexpr auto MutStrView_<T>::splitByAnyChar(Span<T> chList, StrCase sc) -> SplitResult {
	return splitByIndex(findAnyChar(chList, sc), 1);
}

template <class T>
constexpr auto MutStrView_<T>::splitByAnyCharBack(Span<T> chList, StrCase sc) -> SplitResult {
	return splitByIndex(findAnyCharBack(chList, sc), 1);
}

template<class T> inline
std::ostream& operator << ( std::ostream & s, const MutStrView_<T> & v ) { return s << v.to_string_view(); }

} // namespace
