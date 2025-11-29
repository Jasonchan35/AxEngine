module;
#include "AxPlatform-pch.h"

export module AxPlatform.FixedArray;
export import AxPlatform.FixedSpan;

export namespace ax {

template<class T, Int N>
class FixedArray {
	T _data[N];
	
	using This = FixedArray;
public:
	using Element = T;
	static const Int elementCount = N;

	FixedArray() = default;
	FixedArray(FixedArray && r);

	MutSpan<T>	  span		()			{ return MutSpan<T>(data(), N); }
	   Span<T>	  span		() const	{ return    Span<T>(data(), N); }
	   Span<T>	  constSpan	() const	{ return    Span<T>(data(), N); }

	operator  MutSpan<T>	()			{ return span(); }
	operator  Span<T>		() const	{ return span(); }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr    FixedSpan<T, N> fixedSpan() const { return    FixedSpan<T, N>(data()); }
	AX_INLINE constexpr MutFixedSpan<T, N> fixedSpan()       { return MutFixedSpan<T, N>(data()); }

	AX_INLINE	 	  T &	operator[]	( Int i )			{ return at(i); }
	AX_INLINE	const T &	operator[]	( Int i ) const		{ return at(i); }

	AX_INLINE	 	  T &	at			( Int i )			{ _checkBound(i); return data()[i]; }
	AX_INLINE	const T &	at			( Int i ) const		{ _checkBound(i); return data()[i]; }

	AX_INLINE	 		T *	tryAt		(Int i)				{ return inBound(i) ? &data()[i] : nullptr; }
	AX_INLINE	const	T *	tryAt		(Int i) const		{ return inBound(i) ? &data()[i] : nullptr; }

	//-----------------------
	AX_INLINE		Int		size		() const			{ return N; }
	AX_INLINE		bool	inBound		( Int  i ) const	{ return i >= 0 && i < N; }

	AX_INLINE 		  T*	data		()					{ return _data; }
	AX_INLINE	const T*	data		() const			{ return _data; }

	//------- forward View functions ----------------
	AX_INLINE		MutSpan<T>	slice		( Int offset, Int size )		{ return span().slice(offset, size); }
	AX_INLINE		Span<T>		slice		( Int offset, Int size ) const	{ return span().slice(offset, size); }

	AX_INLINE		MutSpan<T>	sliceFrom	( Int offset )					{ return span().sliceFrom(offset); }
	AX_INLINE		Span<T>		sliceFrom	( Int offset ) const			{ return span().sliceFrom(offset); }

	AX_INLINE		MutSpan<T>	sliceBack	( Int offset )					{ return span().sliceBack(offset); }
	AX_INLINE		Span<T>		sliceBack	( Int offset ) const			{ return span().sliceBack(offset); }

	AX_INLINE		void		copyValues	(Span<T> v, Int offset = 0)		{ span().copyValues(); }
	AX_INLINE		void		fillValues	( const T& v )					{ span().fillValues(v); }

	//---------------
	using  Iter			= T*;
	using CIter			= const T*;

	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

private:
	AX_INLINE void _checkBound			( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE void	_debug_checkBound	( Int i ) const {
		#ifdef AX_BUILD_CONFIG_Debug
			_checkBound(i);
		#endif
	}
};

template<class T>		 struct TypeIsFixedArray : std::false_type {};
template<class T, Int N> struct TypeIsFixedArray< FixedArray<T,N> > : std::true_type {};

template<class T, Int N> inline
FixedArray<T, N>::FixedArray(FixedArray&& r) {
	for (Int i = 0; i < N; i++) {
		_data[i] = std::move(r._data[i]);
	}
}

} // namespace
