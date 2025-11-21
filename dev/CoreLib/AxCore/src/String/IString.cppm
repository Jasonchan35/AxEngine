export module AxCore.IString;

#include "AxBase.h"

export import AxCore.IArrayStorage;
export import AxCore.ZStrView;

export namespace ax {

template<class T> class IString_;
using IString   = IString_<Char>;
using IStringW  = IString_<CharW>;
using IString8  = IString_<Char8>;
using IString16 = IString_<Char16>;
using IString32 = IString_<Char32>;

template<class T>
class IString_ : public IArrayStorage<T> {
	using Base = IArrayStorage<T>;
	using Base::_storage;	
protected:
	IString_(T* data, Int initCap) : Base(data, initCap) {}
public:
	using CharType = T;
	using CView    = StrView_<T>;
	using MView    = MutStrView_<T>;
	using ZView    = ZStrView_<T>;
	using MutByte  = std::conditional_t<std::is_const_v<T>, const Byte, Byte>;
	
	constexpr explicit operator bool() const { return size() != 0; }
	constexpr       T* data()			{ return _storage.data(); }
	constexpr const T* data() const		{ return _storage.data(); }
	constexpr Int capacity() const		{ return _storage.capacity(); }
	constexpr Int size() const			{ return _storage.size(); }
	constexpr Int sizeInBytes() const	{ return size() * ax_sizeof<T>; }
	
	constexpr void clear() { Base::_storageClear(); }
	constexpr void clearAndFree() { Base::_storageClearAndFree(); }

	constexpr void reserve(Int newCapacity);

	template<class... Args>
	constexpr void resize(Int newSize, Args&&... args);

	template<class R>
	AX_INLINE constexpr bool	isOverlapped(StrView_<R> r) const	{ return getByteSpan().isOverlapped(r.toByteSpan()); }
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
	
	AX_INLINE constexpr MView getMutStrView()		{ return MView(data(), size()); }
	AX_INLINE constexpr CView getStrView() const	{ return CView(data(), size()); }
	AX_INLINE constexpr ZView getZStrView() const	{ return ZView(data(), size()); }

	AX_INLINE constexpr MutSpan<T> getMutSpan()		{ return getMutStrView().mutSpan(); }
	AX_INLINE constexpr Span<T>    getSpan() const	{ return getStrView().span(); }

	AX_INLINE constexpr MutSpan<MutByte> getMutByteSpan()		{ return getMutStrView().toMutByteSpan(); }
	AX_INLINE constexpr Span<MutByte>    getByteSpan() const	{ return getStrView().toByteSpan(); }

	//------- CView wrapper ---------------
	AX_INLINE constexpr MView	slice		(Int offset, Int size)			{ return getMutStrView().slice(offset, size); }
	AX_INLINE constexpr CView	slice		(Int offset, Int size) const	{ return    getStrView().slice(offset, size); }
	AX_INLINE constexpr MView	sliceFrom	(Int offset) 					{ return getMutStrView().sliceFrom(offset); }
	AX_INLINE constexpr CView	sliceFrom	(Int offset) const				{ return    getStrView().sliceFrom(offset); }
	AX_INLINE constexpr MView	sliceBack	(Int offset)					{ return getMutStrView().sliceBack(offset); }
	AX_INLINE constexpr CView	sliceBack	(Int offset) const				{ return    getStrView().sliceBack(offset); }
	AX_INLINE constexpr MView	sliceTill	(Int n)							{ return getMutStrView().sliceTill(n); }	
	AX_INLINE constexpr CView	sliceTill	(Int n) const					{ return    getStrView().sliceTill(n); }
	AX_INLINE constexpr MView	extractFromPrefix(CView prefix, bool ignoreCase = false)		{ return getMutStrView().extractFromPrefix(prefix, ignoreCase); }
	AX_INLINE constexpr CView	extractFromPrefix(CView prefix, bool ignoreCase = false) const	{ return    getStrView().extractFromPrefix(prefix, ignoreCase); }
	//----------------------------------	
	AX_INLINE constexpr bool operator==	(CView r) const	{ return equals(r); }
	AX_INLINE constexpr bool operator!=	(CView r) const	{ return !equals(r); }
	AX_INLINE constexpr bool operator<	(CView r) const	{ return compare(r) <  0; }
	AX_INLINE constexpr bool operator<=	(CView r) const	{ return compare(r) <= 0; }
	AX_INLINE constexpr bool operator>	(CView r) const	{ return compare(r) >  0; }
	AX_INLINE constexpr bool operator>=	(CView r) const	{ return compare(r) >= 0; }
	AX_INLINE constexpr Int  compare		(CView r,  bool ignoreCase = false) const { return getStrView().compare	  (r,  ignoreCase); }
	AX_INLINE constexpr bool equals		(CView r,  bool ignoreCase = false) const { return getStrView().equals    (r,  ignoreCase); }
	AX_INLINE constexpr bool startsWith	(CView r,  bool ignoreCase = false) const { return getStrView().startsWith(r,  ignoreCase); }
	AX_INLINE constexpr bool endsWith	(CView r,  bool ignoreCase = false) const { return getStrView().endsWith  (r,  ignoreCase); }
	AX_INLINE constexpr bool matchWildcard(CView wildcard, bool ignoreCase) const  { return getStrView().matchWildcard(wildcard, ignoreCase); }
	
	//-----------------------------------
	AX_INLINE void operator=(IString_<T> && rhs) { move(std::move(rhs)); }
	AX_INLINE void move(IString_<T> && rhs);
	
	void                     append(CView view);
	void                     appendChar(const T& ch) { append(CView(&ch, 1)); }

	using  Iter	= T*;
	using CIter	= const T*;
	
	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

protected:
	AX_INLINE void  _setNullTerminator();
	AX_INLINE void	_checkBound			( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE void	_debug_checkBound	( Int i ) const {
	#ifdef _DEBUG
		_checkBound(i);
	#endif
	}
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
void IString_<T>::move(IString_<T> && rhs) {
	Base::_storageMove(std::move(rhs));
	_setNullTerminator(); // _storageMove may copy without null terminator
}

template <class T> inline
void IString_<T>::append(CView view) {
	if (isOverlapped(view)) throw Error_BufferOverlapped();
	
	auto oldSize = size();
	auto newSize = oldSize + view.size();
	reserve(newSize);
	MemoryUtil::copy(data() + oldSize, view.data(), view.size());
	_storage.setSize(newSize);
	_setNullTerminator();
}

template <class T> AX_INLINE
void IString_<T>::_setNullTerminator() {
	auto s = size();
	if (s > 0) data()[s] = 0;
}

} // namespace
