module;
#include "AxPlatform-pch.h"

export module AxPlatform.IString;

export import AxPlatform.IArrayStorage;
export import AxPlatform.Format;

export namespace ax {

template<class FMT_CH>
struct FormatterBase_ : public std::formatter<std::basic_string_view<FMT_CH>, FMT_CH> {
	using Base = std::formatter<std::basic_string_view<FMT_CH>, FMT_CH>;
	
	template<class Context>
	constexpr auto parse(Context& ctx) { return Base::parse(ctx); }
};

template <class OBJ, class FMT_CH>
class FormatHandler;

template<class FMT_CH>
class Format_ : public NonCopyable {
public:
	using Context   = FormatContext_<FMT_CH>;
	using Formatter = FormatterBase_<FMT_CH>;

	constexpr Format_(const Formatter & formatter_, Context & ctx_) : formatter(formatter_), formatContext(ctx_) {}

	constexpr void append(StrView_<FMT_CH> view) {
		formatter.format(view.to_string_view(), formatContext);
	}

	template<class OBJ>
	constexpr void operator << (const OBJ& obj) {
		if constexpr (std::is_convertible_v<OBJ, StrView_<FMT_CH>>) {
			append(obj);
		} else {
			FormatHandler<OBJ, FMT_CH> handler;
			handler.onFormat(obj, *this);
		}
	}

	const Formatter& formatter;
	Context&   formatContext;
};


template<class T> class IString_;
using IString   = IString_<Char>;
using IStringA  = IString_<CharA>;
using IStringW  = IString_<CharW>;
using IString8  = IString_<Char8>;
using IString16 = IString_<Char16>;
using IString32 = IString_<Char32>;

template<class OBJ, class CH>
concept CON_IString_ = std::is_base_of_v<IString_<CH>, OBJ>;

template<class T>
class IString_ : public IArrayStorage<T> {
	using Base = IArrayStorage<T>;
	using Base::_storage;	
protected:
	constexpr IString_(T* data, Int initCap) : Base(data, initCap) {}
public:
	using CharType        = T;
	using CSpan           = Span<T>;
	using MSpan           = MutSpan<T>;
	using CView           = StrView_<T>;
	using MView           = MutStrView_<T>;
	using ZView           = ZStrView_<T>;
	using MutByte         = std::conditional_t<std::is_const_v<T>, const Byte, Byte>;
	using std_string_view = std::basic_string_view<std::remove_cv_t<T>>;

	const T* c_str() const { return zview().c_str(); }
	constexpr std_string_view to_string_view() const noexcept { return std_string_view(data(), size()); }
	constexpr operator std_string_view() const noexcept { return to_string_view(); }
	
	constexpr explicit operator bool() const { return size() != 0; }
	constexpr       T* data()			{ return _storage.data(); }
	constexpr const T* data() const		{ return _storage.data(); }
	constexpr Int capacity() const		{ return _storage.capacity(); }
	constexpr Int size() const			{ return _storage.size(); }
	constexpr Int sizeInBytes() const	{ return size() * AX_SIZEOF(T); }
	
	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }

	constexpr void reserve(Int newCapacity);

	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args);

	template<class R>
	AX_INLINE constexpr bool	isOverlapped(StrView_<R> r) const	{ return toByteSpan().isOverlapped(r.toByteSpan()); }
	AX_INLINE constexpr	bool	inBound(Int i) const				{ return i >= 0 && i < size(); }
	
	AX_INLINE constexpr			T&	operator[]	(Int i)				{ return at(i); }
	AX_INLINE constexpr	const	T&	operator[]	(Int i) const		{ return at(i); }
	AX_INLINE constexpr			T&	at			(Int i)				{ _checkBound(i); return unsafe_at(i); }
	AX_INLINE constexpr	const	T&	at			(Int i) const		{ _checkBound(i); return unsafe_at(i); }
	AX_INLINE constexpr 		T&	back		(Int i = 0)			{ return at( size()-i-1 ); }
	AX_INLINE constexpr const	T&	back		(Int i = 0) const	{ return at( size()-i-1 ); }
	AX_INLINE constexpr 		T&	unsafe_at	(Int i)				{ _debug_checkBound(i); return data()[i]; }
	AX_INLINE constexpr const	T&	unsafe_at	(Int i) const		{ _debug_checkBound(i); return data()[i]; }
	AX_INLINE constexpr 		T&	unsafe_back	(Int i)				{ return unsafe_at( size()-i-1 ); }
	AX_INLINE constexpr const	T&	unsafe_back	(Int i)  const		{ return unsafe_at( size()-i-1 ); }

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

	AX_INLINE constexpr ZView zview() const noexcept	{ return ZView(data(), size()); }
	AX_INLINE constexpr operator ZView() const noexcept	{ return zview(); }
	//---------------
	
	AX_INLINE constexpr MutSpan<MutByte> toMutByteSpan()		{ return view().toMutByteSpan(); }
	AX_INLINE constexpr    Span<MutByte>    toByteSpan() const	{ return view().toByteSpan(); }
	
	//                +--------------------------------------------+
	//                |         |                      |           |
	//                +--------------------------------------------+
	//  slice         (  offset  )[______ newSize ____]
	//  sliceBack                       [________ newSize__________]
	//  sliceTrim     (  offset  )[_________till to end ___________]
	//  sliceTrimBack [________________________________](  offset  )
	AX_INLINE	constexpr MView	slice			(Int offset, Int newSize)		{ return view().slice(offset, newSize); }
	AX_INLINE	constexpr CView	slice			(Int offset, Int newSize) const	{ return view().slice(offset, newSize); }
	AX_INLINE	constexpr MView	slice			(IntRange range)				{ return view().slice(range); }
	AX_INLINE	constexpr CView	slice			(IntRange range) const			{ return view().slice(range); }
	AX_INLINE	constexpr MView	sliceBack		(Int newSize)			 		{ return view().sliceBack(newSize); }
	AX_INLINE	constexpr CView	sliceBack		(Int newSize) const	 			{ return view().sliceBack(newSize); }
	AX_INLINE	constexpr MView	sliceTrim		(Int offset)					{ return view().sliceTrim(offset); }
	AX_INLINE	constexpr CView	sliceTrim		(Int offset) const				{ return view().sliceTrim(offset); }
	AX_INLINE	constexpr MView	sliceTrimBack	(Int offset)					{ return view().sliceTrimBack(offset); }
	AX_INLINE	constexpr CView	sliceTrimBack	(Int offset) const				{ return view().sliceTrimBack(offset); }
	//----------------------------------	
	AX_INLINE constexpr CmpResult compare      (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return view().compare   (r, sc); }
	AX_INLINE constexpr bool      equals       (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return view().equals    (r, sc); }
	AX_INLINE constexpr bool      startsWith   (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return view().startsWith(r, sc); }
	AX_INLINE constexpr bool      endsWith     (CView r, StrCase sc = StrCase::Sensitive) const noexcept { return view().endsWith  (r, sc); }
	AX_INLINE constexpr bool      matchWildcard(CView wildcard, StrCase sc = StrCase::Sensitive) const noexcept { return view().matchWildcard(wildcard, sc); }
	AX_INLINE constexpr bool operator==	(CView r) const noexcept { return view().operator==(r); }
	AX_INLINE constexpr bool operator!=	(CView r) const noexcept { return view().operator!=(r); }
	AX_INLINE constexpr bool operator<	(CView r) const noexcept { return view().operator< (r); }
	AX_INLINE constexpr bool operator>	(CView r) const noexcept { return view().operator> (r); }
	AX_INLINE constexpr bool operator<=	(CView r) const noexcept { return view().operator<=(r); }
	AX_INLINE constexpr bool operator>=	(CView r) const noexcept { return view().operator>=(r); }
	//-----------------------------------
	
	AX_INLINE constexpr void operator=(IString_<T> && rhs) { move(std::move(rhs)); }
	AX_INLINE constexpr void move(IString_<T> && rhs);

	constexpr void append(CView view);
	constexpr void append(const T& ch);

	template<class A> constexpr void operator<<(A & a) { append(a); }
	
	template<class... ARGS>
	constexpr void append_args(ARGS&&... args) { append(args...); }

	constexpr void appendList(const std::initializer_list<T    > & list);
	constexpr void appendList(const std::initializer_list<CView> & list);
	
	AX_INLINE constexpr void appendUtf(CharA  r) { _appendUtf(r); }
	AX_INLINE constexpr void appendUtf(CharW  r) { _appendUtf(r); }
	AX_INLINE constexpr void appendUtf(Char8  r) { _appendUtf(r); }
	AX_INLINE constexpr void appendUtf(Char16 r) { _appendUtf(r); }
	AX_INLINE constexpr void appendUtf(Char32 r) { _appendUtf(r); }
	
	constexpr void appendUtf(StrViewA  r);
	constexpr void appendUtf(StrViewW  r);
	constexpr void appendUtf(StrView8  r);
	constexpr void appendUtf(StrView16 r);
	constexpr void appendUtf(StrView32 r);

	template<class... ARGS>
	constexpr void appendFmt(const FormatString_<Char, ARGS...> & fmt, const ARGS&... args) {
		FmtTo(*this, fmt, AX_FORWARD(args)...);
	}

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const { fmt << view(); }
	
	//----------------------------------
	
	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

protected:
	AX_INLINE constexpr void _setNullTerminator();
	AX_INLINE constexpr void _checkBound		( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE constexpr void _debug_checkBound	( Int i ) const {
	#ifdef _DEBUG
		_checkBound(i);
	#endif
	}

	template<class R> constexpr void _appendUtf(const R& ch);
};

template<class T> inline
std::ostream& operator << ( std::ostream & s, const IString_<T> & v ) { return s << v.to_string_view(); }

template <class T>
template <class R>
constexpr void IString_<T>::_appendUtf(const R& r) {
	if constexpr (std::is_same_v<R,T>) {
		append(r);
	} else {
		appendUtf(StrView_<R>(&r, 1));
	}
}

template<class OUT_CH>	
struct IStringBackInserter_ {
	using This = IStringBackInserter_;
	using OutIString = IString_<OUT_CH>;
	
	using difference_type = ptrdiff_t;

	constexpr explicit IStringBackInserter_(OutIString& s) noexcept : _s(&s) {}

	template<class R> constexpr This& operator=(const R&  ch) { _s->appendUtf(ch); return *this; }
	template<class R> constexpr This& operator=(      R&& ch) { _s->appendUtf(std::move(ch)); return *this; }

	constexpr       This& operator*()       noexcept { return *this; }
	constexpr const This& operator*() const noexcept { return *this; }
	
	constexpr This& operator++() noexcept { return *this; }
	constexpr This  operator++(int) noexcept { return *this; }

protected:
	OutIString* _s = nullptr;
};


template <class T> AX_INLINE
constexpr void IString_<T>::reserve(Int newCapacity) {
	Base::_storageReserve(newCapacity);
}

template <class T>
template <class ... Args> AX_INLINE
constexpr void IString_<T>::resize(Int newSize, Args&&... args) {
	Base::_storageResize(newSize, AX_FORWARD(args)...);
	_setNullTerminator();
}

template <class T> inline
constexpr void IString_<T>::move(IString_<T> && rhs) {
	Base::_storageMove(std::move(rhs));
	_setNullTerminator(); // _storageMove may copy without null terminator
}

template <class T> inline
constexpr void IString_<T>::append(CView view) {
	if (isOverlapped(view)) throw Error_BufferOverlapped();
	if (view.size() <= 0) return;
	
	auto oldSize = size();
	auto newSize = oldSize + view.size();
	reserve(newSize);
	MemUtil::copy(data() + oldSize, view.data(), view.size());
	_storage.setSize(newSize);
	_setNullTerminator();
}

template <class T> AX_INLINE
constexpr void IString_<T>::append(const T& ch) {
	auto oldSize = size();
	auto newSize = oldSize + 1;
	reserve(newSize);
	_storage.setSize(newSize);
	auto* newData = data();
	newData[oldSize] = ch;
	newData[newSize] = 0;
}

template <class T>
constexpr void IString_<T>::appendList(const std::initializer_list<T> & list) {
	auto oldSize = size();
	auto newSize = oldSize + list.size();
	reserve(newSize);
	_storage.setSize(newSize);
	auto* newData = data();
	MemUtil::copy(newData + oldSize, list.begin(), list.size());
	newData[newSize] = 0;	
}

template <class T>
constexpr void IString_<T>::appendList(const std::initializer_list<CView>& list) {
	auto newSize = size();
	for (auto& it : list) {
		newSize += it.size();
	}
	
	reserve(newSize);
	for (auto& it : list) {
		append(it);
	}
}


template <class T> AX_INLINE
constexpr void IString_<T>::_setNullTerminator() {
	auto s = size();
	if (s > 0) data()[s] = 0;
}

} // namespace
