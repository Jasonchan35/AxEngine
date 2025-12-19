module;

export module AxCore.StrView;
export import AxCore.CharUtil;
export import AxCore.HashInt;
export import AxCore.Span;
export import AxCore.Math;

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

template<class T, class CH> constexpr bool Type_IsConvertibleToStrView_	= std::is_convertible_v<T, StrView_<CH>>;

template<class T> constexpr bool Type_IsConvertibleToStrViewX	=  Type_IsConvertibleToStrView_<T, CharA >
																|| Type_IsConvertibleToStrView_<T, CharW >
																|| Type_IsConvertibleToStrView_<T, Char8 >
																|| Type_IsConvertibleToStrView_<T, Char16>
																|| Type_IsConvertibleToStrView_<T, Char32>;

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
	constexpr static bool tryParse(StrView_<CH> inStr, T& obj) {
		if constexpr (Type_IsEnumFlag<T>) {
			obj = static_cast<T>(0);
			auto tmp = inStr;
			while (tmp) {
				auto pair = tmp.splitByChar('|');
				auto s = pair.first.trimSpaceTabBoth();
				if (!s) break;
				T e;
				if (!_ax_macro_enum_try_parse(s, e)) {
					return false;
				}
				obj |= e;
				tmp = pair.second; 
			}
			return true;
		} else {
			return _ax_macro_enum_try_parse(inStr, obj);
		}
		
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
	
	constexpr MutStrView_() noexcept = default;
	constexpr MutStrView_(const This&) noexcept = default;
	constexpr MutStrView_(T* data, Int size) noexcept : _data(data), _size(size) {}
	constexpr MutStrView_(MutZStrView_<T> r) noexcept : _data(r.data()), _size(r.size()) {}
//	constexpr MutStrView_(T& ch) noexcept : _data(&ch), _size(1) {}
	
	template <Int N>
	constexpr MutStrView_(T (&sz)[N]) noexcept : _data(sz), _size( N > 0 ? N - 1 : 0) {}
	
	AX_INLINE constexpr void setPtr(T* data, Int size) noexcept { _data = data; _size = size; }
	AX_NODISCARD constexpr bool samePtrAndSize(CSpan r) const noexcept { return _size == r._size && _data == r._data; }
		
	static constexpr MView s_empty() { return MutStrView_(); }
	explicit constexpr operator bool() const { return size() > 0; }

	//--------------
	AX_INLINE constexpr       T*	data()       noexcept 				{ return _data; }
	AX_INLINE constexpr const T*	data() const noexcept 				{ return _data; }
	AX_INLINE constexpr Int      	size() const noexcept 				{ return _size; }
	AX_INLINE constexpr Int	     	sizeInBytes() const noexcept		{ return _size * AX_SIZEOF(T); }
	template<class R>
	AX_INLINE constexpr bool		isOverlapped(StrView_<R> r) const	{ return toByteSpan().isOverlapped(r.toByteSpan()); }
	AX_INLINE constexpr	bool		inBound(Int i) const				{ return i >= 0 && i < _size; }
	AX_INLINE constexpr			T&	operator[]	(Int i)					{ return at(i); }
	AX_INLINE constexpr	const	T&	operator[]	(Int i) const			{ return at(i); }
	AX_INLINE constexpr			T&	at			(Int i)					{ _checkBound(i); return _data[i]; }
	AX_INLINE constexpr	const	T&	at			(Int i) const			{ _checkBound(i); return _data[i]; }
	AX_INLINE constexpr 		T&	back		(Int i = 0)				{ return at( _size-i-1 ); }
	AX_INLINE constexpr const	T&	back		(Int i = 0) const		{ return at( _size-i-1 ); }
	AX_INLINE constexpr 		T&	at_noBoundCheck	(Int i)	noexcept		{ _debug_boundCheck(i); return _data[i]; }
	AX_INLINE constexpr const	T&	at_noBoundCheck	(Int i) const noexcept	{ _debug_boundCheck(i); return _data[i]; }
	AX_INLINE constexpr 		T&	back_noBoundCheck	(Int i)					{ return at_noBoundCheck( _size-i-1 ); }
	AX_INLINE constexpr const	T&	back_noBoundCheck	(Int i)  const			{ return at_noBoundCheck( _size-i-1 ); }
	//---------------
	AX_INLINE constexpr CView	constView	() const noexcept	{ return CView(_data, _size); }
	AX_INLINE constexpr CView	view		() const noexcept	{ return CView(_data, _size); }
	AX_INLINE constexpr MView	view		()       noexcept	{ return MView(_data, _size); }
	AX_INLINE constexpr operator CView() const noexcept			{ return constView(); }

	AX_INLINE constexpr CSpan constSpan() const noexcept	{ return CSpan(_data, _size); }
	AX_INLINE constexpr CSpan      span() const noexcept	{ return CSpan(_data, _size); }
	AX_INLINE constexpr MSpan      span() noexcept			{ return MSpan(_data, _size); }
	//---------------
	AX_INLINE constexpr static MView	s_fromMutByteSpan	(MutByteSpan	from) noexcept	{ return MView(reinterpret_cast<T*>(from.data()), from.sizeInBytes() / AX_SIZEOF(T)); }
	AX_INLINE constexpr void			  fromMutByteSpan	(MutByteSpan	from) noexcept	{ *this = s_fromMutByteSpan(from); }
	AX_INLINE constexpr MutByteSpan		toMutByteSpan		()       noexcept { return MutByteSpan(reinterpret_cast<      Byte*>(_data), sizeInBytes()); }
	AX_INLINE constexpr    ByteSpan		   toByteSpan		() const noexcept { return    ByteSpan(reinterpret_cast<const Byte*>(_data), sizeInBytes()); }
	
	constexpr std_string_view to_string_view() const noexcept { return std_string_view(_data, _size); }
	constexpr operator std_string_view() const noexcept { return to_string_view(); }

	//                +--------------------------------------------+
	//                |         |                      |           |
	//                +--------------------------------------------+
	//  slice         (offset)[------ newSize ---->]
	//  sliceBack                 [<------ newSize --------](offset)
	//  sliceFrom     (offset)[------ till to back---------------->]
	//  sliceFromBack [<------------- till to front -------](offset)
	AX_INLINE constexpr MView	  slice         (Int offset, Int newSize)		{ return _slice(offset, newSize); }
	AX_INLINE constexpr CView     slice	        (Int offset, Int newSize) const	{ return ax_const_cast(this)->_slice(offset, newSize); }
	AX_INLINE constexpr MView     slice	        (IntRange range)				{ return _slice(range.begin(), range.size()); }
	AX_INLINE constexpr CView     slice	        (IntRange range) const			{ return _slice(range.begin(), range.size()); }
	AX_INLINE constexpr MView     sliceFrom	    (Int offset)					{ return _slice(offset, _size - offset); }
	AX_INLINE constexpr CView     sliceFrom	    (Int offset) const				{ return _slice(offset, _size - offset); }
	AX_INLINE constexpr MView     sliceBack	    (Int offset, Int newSize)		{ return _slice(_size - newSize - offset, newSize); }
	AX_INLINE constexpr CView     sliceBack	    (Int offset, Int newSize) const	{ return _slice(_size - newSize - offset, newSize); }
	AX_INLINE constexpr MView     sliceFromBack	(Int offset)					{ return _slice(0, _size - offset); }
	AX_INLINE constexpr CView     sliceFromBack	(Int offset) const				{ return _slice(0, _size - offset); }
	//----------------
	AX_INLINE constexpr CmpResult compare      (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	constexpr bool                equals       (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      startsWith   (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	AX_INLINE constexpr bool      endsWith     (CView r, StrCase sc = StrCase::Sensitive) const noexcept;
	inline    constexpr bool      matchWildcard(CView wildcard, StrCase sc = StrCase::Sensitive) const noexcept;
//	AX_INLINE constexpr bool      operator==	(CView r) const	{ return  equals(r); }
	AX_INLINE constexpr bool      operator!=	(CView r) const	{ return !equals(r); }
	AX_INLINE constexpr bool      operator<		(CView r) const	{ return CmpResult_isLesser        (compare(r)); }
	AX_INLINE constexpr bool      operator<=	(CView r) const	{ return CmpResult_isLesserOrEqual (compare(r)); }
	AX_INLINE constexpr bool      operator>		(CView r) const	{ return CmpResult_isGreater       (compare(r)); }
	AX_INLINE constexpr bool      operator>=	(CView r) const	{ return CmpResult_isGreaterOrEqual(compare(r)); }
	//----------------
	AX_NODISCARD constexpr Opt<Int> find(CView str, StrCase sc = StrCase::Sensitive) const;
	AX_NODISCARD constexpr Opt<Int> findBack(CView str, StrCase sc = StrCase::Sensitive) const;
	
	AX_NODISCARD constexpr Opt<Int> findChar(const T& ch, StrCase sc = StrCase::Sensitive) const {
		if (sc == StrCase::Sensitive) {
			return _findChar([&](auto& it) { return it == ch; });
		} else {
			return _findChar([&](auto& it) { return CharUtil::equals_i(it, ch); });
		}
	}

	AX_NODISCARD constexpr Opt<Int> findCharBack(const T& ch, StrCase sc = StrCase::Sensitive) const {
		if (sc == StrCase::Sensitive) {
			return _findCharBack([&](auto& it) { return it == ch; });
		} else {
			return _findCharBack([&](auto& it) { return CharUtil::equals_i(it, ch); });
		}
	}

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr Opt<Int> findChar_(FUNC func) const {
		return _findChar(func);
	}
	
	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr Opt<Int> findCharBack_(FUNC func) const {
		return _findCharBack(func);
	}
	
	//----------------
	using SplitResult = Pair<MView, MView>;
	AX_NODISCARD constexpr auto splitByIndex		(Opt<Int> index, Int separatorSize) -> SplitResult;
	AX_NODISCARD constexpr auto split				(CView      str, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitBack			(CView      str, StrCase sc = StrCase::Sensitive) -> SplitResult;
	
	AX_NODISCARD constexpr auto splitByChar			(const T&    ch, StrCase sc = StrCase::Sensitive) -> SplitResult;
	AX_NODISCARD constexpr auto splitByCharBack		(const T&    ch, StrCase sc = StrCase::Sensitive) -> SplitResult;

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr auto splitByChar_		(FUNC func) -> SplitResult {
		return splitByIndex(_findChar(func), 1);
	}
	
	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr auto splitByCharBack_	(FUNC func) -> SplitResult{
		return splitByIndex(_findCharBack(func), 1);
	}

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr MView trim_(FUNC func) { return _trim(func); }

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr MView trimBack_(FUNC func) { return _trimBack(func); }

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr MView trimBoth_(FUNC func) { return _trimBoth(func); }
	
	AX_NODISCARD constexpr MView trim    (const T& ch)	{ return _trim    ([ch](const T& e){ return e == ch; }); }
	AX_NODISCARD constexpr MView trimBack(const T& ch)	{ return _trimBack([ch](const T& e){ return e == ch; }); }
	AX_NODISCARD constexpr MView trimBoth(const T& ch)	{ return _trimBoth([ch](const T& e){ return e == ch; }); }
	
	AX_NODISCARD constexpr MView trimSpaceTab    () { return _trim    ([](const T& e){ return e == ' ' || e == '\t'; }); }
	AX_NODISCARD constexpr MView trimSpaceTabBack() { return _trimBack([](const T& e){ return e == ' ' || e == '\t'; }); }
	AX_NODISCARD constexpr MView trimSpaceTabBoth() { return _trimBoth([](const T& e){ return e == ' ' || e == '\t'; }); }
	
	//----------------
	constexpr MView extractFromPrefix(CView prefix, StrCase sc = StrCase::Sensitive) {
		return startsWith(prefix, sc) ? MView(sliceFrom(prefix.size())) : s_empty();
	}

	constexpr CView extractFromPrefix(CView prefix, StrCase sc = StrCase::Sensitive) const {
		return ax_const_cast(this)->extractFromPrefix(prefix, sc);
	}
	//----------------
	
	template<class OBJ>
	bool	tryParse	(OBJ & outValue) const { return StrView_parse(*this, outValue); }
	
	AX_NODISCARD AX_INLINE constexpr HashInt onHashInt() const noexcept { return HashInt::s_make(span()); }
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()		 noexcept	{ return _data; }
	constexpr CIter	begin	() const noexcept	{ return _data; }
	constexpr  Iter	end		()		 noexcept	{ return _data + _size; }
	constexpr CIter	end		() const noexcept	{ return _data + _size; }

	template<class TT> using RevForEach_ = Span_RevForEach_<TT>;
	constexpr auto revForEach	()			{ return RevForEach_<      T>::s_make( _data, _data + _size ); }
	constexpr auto revForEach	() const	{ return RevForEach_<const T>::s_make( _data, _data + _size ); }

private:
	AX_INLINE constexpr void _checkBound		( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE constexpr void _debug_boundCheck	( Int i ) const {
#ifdef _DEBUG
		_checkBound(i);
#endif
	}

	constexpr MView	_slice(Int offset, Int newSize);
	constexpr CView	_slice(Int offset, Int newSize) const { return ax_const_cast(this)->_slice(offset, newSize); }
	
	template<class FUNC> AX_NODISCARD constexpr Opt<Int> _findChar		(FUNC func) const;
	template<class FUNC> AX_NODISCARD constexpr Opt<Int> _findCharBack	(FUNC func) const;
	template<class FUNC> AX_NODISCARD constexpr MView _trim		(FUNC func);
	template<class FUNC> AX_NODISCARD constexpr MView _trimBack	(FUNC func);
	template<class FUNC> AX_NODISCARD constexpr MView _trimBoth	(FUNC func) { return _trim(func)._trimBack(func); }
};

template<class OBJ, class T>
class StrView_BaseFunc {
	using CSpan           = Span<T>;
	using MSpan           = MutSpan<T>;
	using CView           = StrView_<T>;
	using MView           = MutStrView_<T>;
	using ZView           = ZStrView_<T>;

	constexpr MView _obj_view()			{ return static_cast<      OBJ*>(this)->view(); }
	constexpr CView _obj_view() const	{ return static_cast<const OBJ*>(this)->view(); }
	
public:	
	template<class R>
	AX_INLINE constexpr bool	isOverlapped(StrView_<R> r) const	{ return toByteSpan().isOverlapped(r.toByteSpan()); }
	
	AX_INLINE constexpr MutByteSpan toMutByteSpan()			{ return _obj_view().toMutByteSpan(); }
	AX_INLINE constexpr    ByteSpan    toByteSpan() const	{ return _obj_view().toByteSpan(); }
	
	//                +--------------------------------------------+
	//                |         |                      |           |
	//                +--------------------------------------------+
	//  slice         (offset)[------ newSize ---->]
	//  sliceBack                 [<------ newSize --------](offset)
	//  sliceFrom     (offset)[------ till to back---------------->]
	//  sliceFromBack [<------------- till to front -------](offset)
	AX_INLINE	constexpr MView	slice			(Int offset, Int newSize)		{ return _obj_view().slice(offset, newSize); }
	AX_INLINE	constexpr CView	slice			(Int offset, Int newSize) const	{ return _obj_view().slice(offset, newSize); }
	AX_INLINE	constexpr MView	slice			(IntRange range)				{ return _obj_view().slice(range); }
	AX_INLINE	constexpr CView	slice			(IntRange range) const			{ return _obj_view().slice(range); }
	AX_INLINE	constexpr MView	sliceBack		(Int offset, Int newSize)		{ return _obj_view().sliceBack(offset, newSize); }
	AX_INLINE	constexpr CView	sliceBack		(Int offset, Int newSize) const	{ return _obj_view().sliceBack(offset, newSize); }
	AX_INLINE	constexpr MView	sliceFrom		(Int offset)					{ return _obj_view().sliceFrom(offset); }
	AX_INLINE	constexpr CView	sliceFrom		(Int offset) const				{ return _obj_view().sliceFrom(offset); }
	AX_INLINE	constexpr MView	sliceFromBack	(Int offset)					{ return _obj_view().sliceFromBack(offset); }
	AX_INLINE	constexpr CView	sliceFromBack	(Int offset) const				{ return _obj_view().sliceFromBack(offset); }
	//----------------------------------	
	AX_INLINE constexpr CmpResult compare      (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return _obj_view().compare   (r, sc); }
	AX_INLINE constexpr bool      equals       (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return _obj_view().equals    (r, sc); }
	AX_INLINE constexpr bool      startsWith   (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return _obj_view().startsWith(r, sc); }
	AX_INLINE constexpr bool      endsWith     (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return _obj_view().endsWith  (r, sc); }
	AX_INLINE constexpr bool      matchWildcard(CView wildcard, StrCase sc = StrCase::Sensitive) const noexcept { return _obj_view().matchWildcard(wildcard, sc); }
//	AX_INLINE constexpr bool operator==	(CView r) const noexcept { return _obj_view().operator==(r); }
	AX_INLINE constexpr bool operator!=	(CView r) const noexcept { return _obj_view().operator!=(r); }
	AX_INLINE constexpr bool operator<	(CView r) const noexcept { return _obj_view().operator< (r); }
	AX_INLINE constexpr bool operator>	(CView r) const noexcept { return _obj_view().operator> (r); }
	AX_INLINE constexpr bool operator<=	(CView r) const noexcept { return _obj_view().operator<=(r); }
	AX_INLINE constexpr bool operator>=	(CView r) const noexcept { return _obj_view().operator>=(r); }
	//----------------
	AX_NODISCARD constexpr Opt<Int>	find			(CView      str, StrCase sc = StrCase::Sensitive) const { return _obj_view().find           (str   , sc); }
	AX_NODISCARD constexpr Opt<Int>	findBack		(CView      str, StrCase sc = StrCase::Sensitive) const { return _obj_view().findBack       (str   , sc); }
	AX_NODISCARD constexpr Opt<Int>	findChar		(const T&    ch, StrCase sc = StrCase::Sensitive) const { return _obj_view().findChar       (ch    , sc); }
	AX_NODISCARD constexpr Opt<Int>	findCharBack	(const T&    ch, StrCase sc = StrCase::Sensitive) const { return _obj_view().findCharBack   (ch    , sc); }
	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr Opt<Int>	findChar		(FUNC      func, StrCase sc = StrCase::Sensitive) const { return _obj_view().findChar       (func  , sc); }
	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr Opt<Int>	findCharBack	(FUNC      func, StrCase sc = StrCase::Sensitive) const { return _obj_view().findCharBack   (func  , sc); }
	//----------------
	using SplitResult = Pair<MView, MView>;
	AX_NODISCARD constexpr auto splitByIndex		(Opt<Int> index, Int separatorSize) -> SplitResult { return _obj_view().splitByIndex(index, separatorSize); }
	AX_NODISCARD constexpr auto split				(CView      str, StrCase sc = StrCase::Sensitive) -> SplitResult { return _obj_view().split             (str , sc); }
	AX_NODISCARD constexpr auto splitBack			(CView      str, StrCase sc = StrCase::Sensitive) -> SplitResult { return _obj_view().splitBack         (str , sc); }
	AX_NODISCARD constexpr auto splitByChar			(const T&    ch, StrCase sc = StrCase::Sensitive) -> SplitResult { return _obj_view().splitByChar       (ch  , sc); }
	AX_NODISCARD constexpr auto splitByCharBack		(const T&    ch, StrCase sc = StrCase::Sensitive) -> SplitResult { return _obj_view().splitByCharBack   (ch  , sc); }

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr auto splitByChar			(FUNC func, StrCase sc = StrCase::Sensitive) -> SplitResult { return _obj_view().splitByChar       (func, sc); }
	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr auto splitByCharBack		(FUNC func, StrCase sc = StrCase::Sensitive) -> SplitResult { return _obj_view().splitByCharBack   (func, sc); }
	//----------------
	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr MView trim_(FUNC func) { return _obj_view().trim_(func); }

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr MView trimBack_(FUNC func) { return _obj_view().trimBack_(func); }

	template<class FUNC> requires std::is_invocable_v<FUNC, const T&>
	AX_NODISCARD constexpr MView trimBoth_(FUNC func) { return _obj_view().trimBoth_(func); }
	
	AX_NODISCARD constexpr MView trim    (const T& ch)	{ return _obj_view().trim    (ch); }
	AX_NODISCARD constexpr MView trimBack(const T& ch)	{ return _obj_view().trimBack(ch); }
	AX_NODISCARD constexpr MView trimBoth(const T& ch)	{ return _obj_view().trimBoth(ch); }
	
	AX_NODISCARD constexpr MView trimSpaceTab    () { return _obj_view().trimSpaceTab    (); }
	AX_NODISCARD constexpr MView trimSpaceTabBack() { return _obj_view().trimSpaceTabBack(); }
	AX_NODISCARD constexpr MView trimSpaceTabBoth() { return _obj_view().trimSpaceTabBoth(); }

	
	//----------------
	AX_INLINE			MView	extractFromPrefix(CView prefix, StrCase sc = StrCase::Sensitive)		{ return _obj_view().extractFromPrefix(prefix, sc); }
	AX_INLINE			CView	extractFromPrefix(CView prefix, StrCase sc = StrCase::Sensitive) const	{ return _obj_view().extractFromPrefix(prefix, sc); }
	//----------------
};

template<class T> constexpr bool Type_IsMutStrView = false;
template<class T> constexpr bool Type_IsMutStrView<MutStrView_<T>> = true;

template<class T> constexpr bool Type_IsStrView = false; 
template<class T> constexpr bool Type_IsStrView<StrView_<T>> = true;

template<class T> AX_INLINE	constexpr 
bool MutStrView_<T>::startsWith(CView r, StrCase sc) const noexcept {
	if (_size < r.size()) return false;
	return ax_const_cast(this)->slice(0, r.size()).equals(r, sc);
}

template<class T> AX_INLINE	constexpr 
bool MutStrView_<T>::endsWith(CView r, StrCase sc) const noexcept {
	if (_size < r.size()) return false;
	return ax_const_cast(this)->sliceBack(0, r.size()).equals(r, sc);
}

template <class T>
AX_NODISCARD AX_INLINE constexpr StrView_<T> StrView_char(const T& ch) noexcept { return StrView_<T>(&ch, 1); }

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

template<class T>
constexpr typename MutStrView_<T>::MView MutStrView_<T>::_slice(Int offset, Int newSize) {
	if (offset < 0 || newSize < 0 || offset + newSize > _size) {
		if (std::is_constant_evaluated()) {
			AX_ASSERT(false);
		} else {
			throw Error_IndexOutOfRange();
		}
	}
	return MView(_data + offset, newSize);
}

template<class T>
template<class FUNC>
constexpr Opt<Int> MutStrView_<T>::_findChar(FUNC func) const {
	Int loop = size();
	for (Int i = 0; i < loop; ++i) {
		if (func(at(i))) return i;
	}
	return std::nullopt;
}

template <class T>
template<class FUNC>
constexpr Opt<Int> MutStrView_<T>::_findCharBack(FUNC func) const {
	Int loop = size();
	static_assert(std::is_signed_v<Int>);
	for (Int i = loop-1; i >=0; --i) {
		if (func(at(i))) return i;
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
		Int second = Math::min(index.value() + separatorSize, _size);
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

template<class T>
template<class FUNC>
constexpr typename MutStrView_<T>::MView MutStrView_<T>::_trim(FUNC func) {
	Int offset = 0;
	for (offset = 0; offset < _size; ++offset) {
		if (!func(_data[offset])) break;
	}
	return sliceFrom(offset);
}

template<class T>
template<class FUNC>
constexpr typename MutStrView_<T>::MView MutStrView_<T>::_trimBack(FUNC func) {
	Int offset = 0;
	for (offset = 0; offset < _size; ++offset) {
		if (!func(back(offset))) break;
	}
	return sliceFromBack(offset);
}

template<class T> inline
std::ostream& operator << ( std::ostream & s, const MutStrView_<T> & v ) { return s << v.to_string_view(); }

template<class A, class B> requires std::is_convertible_v<A, StrViewA> && std::is_convertible_v<B, StrViewA>
AX_NODISCARD AX_INLINE constexpr bool operator==(const A& a, const B& b) { return StrViewA(a).equals(StrViewA(b)); }

template<class A, class B> requires std::is_convertible_v<A, StrViewW> && std::is_convertible_v<B, StrViewW>
AX_NODISCARD AX_INLINE constexpr bool operator==(const A& a, const B& b) { return StrViewW(a).equals(StrViewW(b)); }

template<class A, class B> requires std::is_convertible_v<A, StrView8> && std::is_convertible_v<B, StrView8>
AX_NODISCARD AX_INLINE constexpr bool operator==(const A& a, const B& b) { return StrView8(a).equals(StrView8(b)); }

template<class A, class B> requires std::is_convertible_v<A, StrView16> && std::is_convertible_v<B, StrView16>
AX_NODISCARD AX_INLINE constexpr bool operator==(const A& a, const B& b) { return StrView16(a).equals(StrView16(b)); }

template<class A, class B> requires std::is_convertible_v<A, StrView32> && std::is_convertible_v<B, StrView32>
AX_NODISCARD AX_INLINE constexpr bool operator==(const A& a, const B& b) { return StrView32(a).equals(StrView32(b)); }


} // namespace
