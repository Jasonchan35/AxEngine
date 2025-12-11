module;

export module AxCore.IString;

export import AxCore.IArrayStorage;
export import AxCore.Format;
export import AxCore.HashInt;
export import AxCore.Tuple;

export namespace ax {

template<class T> class IString_;
using IString   = IString_<Char>;
using IStringA  = IString_<CharA>;
using IStringW  = IString_<CharW>;
using IString8  = IString_<Char8>;
using IString16 = IString_<Char16>;
using IString32 = IString_<Char32>;

template<class OBJ, class CH>
concept CON_IString_ = std::is_base_of_v<OBJ, IString_<CH>>;

template<class OBJ, class CH>
concept CON_StrView_ = std::is_convertible_v<OBJ, StrView_<CH>>;

template<class T>
class IString_ : public IArrayStorage<T>, public StrView_BaseFunc<IString_<T>, T> {
	using This = IString_;
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
	
	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }

//	constexpr void reserve(Int newCapacity) { ensureCapacity(newCapacity); }
	constexpr void ensureCapacity(Int newCapacity) { Base::_storageEnsureCapacity(newCapacity); }

	template<class... Args> constexpr void resize(Int newSize, Args&&... args);
	template<class... Args> constexpr void incSize(Int n, Args&&... args)	{ resize(size() + n, AX_FORWARD(args)...); }
							constexpr void decSize(Int n)					{ resize(size() - n); }
							constexpr void resizeToCapacity() { resize(capacity()); }
							constexpr void ensureSize(Int n) { if (size() < n) resize(n); }

	constexpr explicit operator bool() const { return size() != 0; }
	constexpr       T* data()			{ return _storage.data(); }
	constexpr const T* data() const		{ return _storage.data(); }
	constexpr Int capacity() const		{ return _storage.capacity(); }
	constexpr Int size() const			{ return _storage.size(); }
	constexpr Int sizeInBytes() const	{ return size() * AX_SIZEOF(T); }

	AX_INLINE constexpr		T	popBack()	{ T tmp = back(); decSize(1); return tmp; }

	template<class R>
	AX_INLINE constexpr bool		isOverlapped(StrView_<R> r) const	{ return view().isOverlapped(r); }
	AX_INLINE constexpr	bool		inBound(Int i) const				{ return i >= 0 && i < size(); }
	AX_INLINE constexpr			T&	operator[]	(Int i)					{ return at(i); }
	AX_INLINE constexpr	const	T&	operator[]	(Int i) const			{ return at(i); }
	AX_INLINE constexpr			T&	at			(Int i)					{ _checkBound(i); return at_noBoundCheck(i); }
	AX_INLINE constexpr	const	T&	at			(Int i) const			{ _checkBound(i); return at_noBoundCheck(i); }
	AX_INLINE constexpr 		T&	back		(Int i = 0)				{ return at( size()-i-1 ); }
	AX_INLINE constexpr const	T&	back		(Int i = 0) const		{ return at( size()-i-1 ); }
	AX_INLINE constexpr 		T&	at_noBoundCheck	(Int i)	noexcept		{ _debug_boundCheck(i); return data()[i]; }
	AX_INLINE constexpr const	T&	at_noBoundCheck	(Int i) const noexcept	{ _debug_boundCheck(i); return data()[i]; }
	AX_INLINE constexpr 		T&	back_noBoundCheck	(Int i)					{ return at_noBoundCheck( size()-i-1 ); }
	AX_INLINE constexpr const	T&	back_noBoundCheck	(Int i)  const			{ return at_noBoundCheck( size()-i-1 ); }
	//---------------
	AX_INLINE constexpr CView	constView	() const noexcept	{ return CView(data(), size()); }
	AX_INLINE constexpr CView	view		() const noexcept	{ return CView(data(), size()); }
	AX_INLINE constexpr MView	view		()       noexcept	{ return MView(data(), size()); }
	AX_INLINE constexpr operator CView() const noexcept			{ return constView(); }

	AX_INLINE constexpr CSpan constSpan() const noexcept	{ return CSpan(data(), size()); }
	AX_INLINE constexpr CSpan      span() const noexcept	{ return CSpan(data(), size()); }
	AX_INLINE constexpr MSpan      span() noexcept			{ return MSpan(data(), size()); }
	//---------------

	AX_INLINE constexpr ZView zview() const noexcept	{ return ZView(data(), size()); }
	AX_INLINE constexpr operator ZView() const noexcept	{ return zview(); }
	//---------------
	
	constexpr void replaceChars(const T& from, const T& to);
	constexpr Int  replaceAll(CView from, CView to, StrCase sc = StrCase::Sensitive);
	//----------------

	template<class OBJ> constexpr bool tryParse(OBJ & obj) const { return view().tryParse(obj); }
	
	AX_INLINE constexpr void operator=(StrLit_<T> rhs) { copy(rhs); }
	AX_INLINE constexpr void operator=(ZView      rhs) { copy(rhs); }
	AX_INLINE constexpr void operator=(MView      rhs) { copy(rhs); }
	AX_INLINE constexpr void operator=(CView      rhs) { copy(rhs); }
	AX_INLINE constexpr void operator=(const IString_<T> & rhs) { copy(rhs); }
	AX_INLINE constexpr void operator=(IString_<T> && rhs) { move(AX_FORWARD(rhs)); }
	
	template<Int N>
	AX_INLINE constexpr void operator=(const T (&sz)[N]) { copy(sz); }
	
	AX_INLINE constexpr void copy(CView rhs) { Base::_storageCopy(rhs.span()); _setNullTerminator(); }
	AX_INLINE constexpr void move(IString_<T> && rhs);

	template<class R> constexpr This& operator<<(const R& r) { append(r); return *this; }
	template<class R> constexpr void  operator+=(const R& r) { append(r); }

	template<class SRC> AX_INLINE constexpr void set(const SRC& src) { clear(); append(src); }

	template<CON_StrView_<T>... ARGS> constexpr void set(const ARGS&... args) { clear(); append(args...); }
	template<CON_StrView_<T>... ARGS> constexpr void append(const ARGS&... args);

	constexpr void append(const T& view);
	constexpr void append(const CharHexPair<T>& hex) { append(hex.c0); append(hex.c1); }
	
	template<class SRC>
	AX_INLINE constexpr void setUtf(const SRC& src) { clear(); appendUtf(src); }
	
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
	constexpr void appendFormat(const FormatString_<T, ARGS...> & fmt, const ARGS&... args) {
		FmtTo(*this, fmt, AX_FORWARD(args)...);
	}

	template<class SE> void onJsonIO_Value(SE& se) { se.io_string(*this); }
	
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
	AX_INLINE constexpr void _debug_boundCheck	( Int i ) const {
	#ifdef _DEBUG
		_checkBound(i);
	#endif
	}

	template<class R>
	constexpr void _appendUtf(const R& ch);
	constexpr void _append(CView view);
};

template <class T> constexpr 
void IString_<T>::replaceChars(const T& from, const T& to) {
	auto* s = data();
	auto* e = s + size();
	for ( ; s < e; s++ ) {
		if (*s == from) *s = to;
	}
}

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
constexpr void IString_<T>::_append(CView view) {
	if (isOverlapped(view)) throw Error_BufferOverlapped();
	if (view.size() <= 0) return;
	
	auto oldSize = size();
	auto newSize = oldSize + view.size();
	ensureCapacity(newSize);
	MemUtil::copyConstructor(data() + oldSize, view.data(), view.size());
	_storage.setSize(newSize);
	_setNullTerminator();
}

template <class T> AX_INLINE
constexpr void IString_<T>::append(const T& ch) {
	auto oldSize = size();
	auto newSize = oldSize + 1;
	ensureCapacity(newSize);
	_storage.setSize(newSize);
	auto* newData = data();
	newData[oldSize] = ch;
	newData[newSize] = 0;
}

template<class T>
template<CON_StrView_<T>... ARGS>
constexpr void IString_<T>::append(const ARGS&... args) {
	auto newSize = size();
	newSize = (newSize + ... +  CView(args).size());
	ensureCapacity(newSize);
	(_append(args), ...);
}

template <class T> AX_INLINE
constexpr void IString_<T>::_setNullTerminator() {
	auto s = size();
	if (s > 0) data()[s] = 0;
}

} // namespace
