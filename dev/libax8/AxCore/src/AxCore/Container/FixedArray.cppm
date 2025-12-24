module;
export module AxCore.FixedArray;
export import AxCore.MemAllocator;
export import AxCore.MemUtil;
export import AxCore.FixedSpan;

export namespace ax {

template<class T, Int N>
class FixedArray : public Span_BaseFunc<FixedArray<T,N>, T> {
	T _data[N];
	
	using This = FixedArray;
public:
	using Element = T;
	static const Int kSize = N;

	constexpr FixedArray() = default;

	constexpr MutSpan<T>	  span		()			{ return MutSpan<T>(data(), size()); }
	constexpr    Span<T>	  span		() const	{ return    Span<T>(data(), size()); }
	constexpr    Span<T>	  constSpan	() const	{ return    Span<T>(data(), size()); }

	constexpr operator  MutSpan<T>	()			{ return span(); }
	constexpr operator  Span<T>		() const	{ return span(); }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr    FixedSpan<T, N> fixedSpan() const { return    FixedSpan<T, N>(data()); }
	AX_INLINE constexpr MutFixedSpan<T, N> fixedSpan()       { return MutFixedSpan<T, N>(data()); }

	AX_INLINE constexpr	 	  T &	operator[]	( Int i )			{ return at(i); }
	AX_INLINE constexpr	const T &	operator[]	( Int i ) const		{ return at(i); }

	AX_INLINE constexpr	 	  T &	at			( Int i )			{ return fixedSpan().at(i); }
	AX_INLINE constexpr	const T &	at			( Int i ) const		{ return fixedSpan().at(i); }

	AX_INLINE constexpr	 		T *	tryGetElement		(Int i)				{ return fixedSpan().tryGetElement(i); }
	AX_INLINE constexpr	const	T *	tryGetElement		(Int i) const		{ return fixedSpan().tryGetElement(i); }

	//-----------------------
	AX_INLINE constexpr		Int		size		() const			{ return N; }
	AX_INLINE constexpr		bool	inBound		( Int  i ) const	{ return i >= 0 && i < N; }

	AX_INLINE constexpr 	  T*	data		()					{ return _data; }
	AX_INLINE constexpr	const T*	data		() const			{ return _data; }

	//------- forward View functions ----------------
	AX_INLINE constexpr		MutSpan<T>	slice		( Int offset, Int size )		{ return span().slice(offset, size); }
	AX_INLINE constexpr		Span<T>		slice		( Int offset, Int size ) const	{ return span().slice(offset, size); }

	AX_INLINE constexpr		MutSpan<T>	sliceFrom	( Int offset )					{ return span().sliceFrom(offset); }
	AX_INLINE constexpr		Span<T>		sliceFrom	( Int offset ) const			{ return span().sliceFrom(offset); }

	AX_INLINE constexpr		MutSpan<T>	sliceBack	( Int offset )					{ return span().sliceBack(offset); }
	AX_INLINE constexpr		Span<T>		sliceBack	( Int offset ) const			{ return span().sliceBack(offset); }

	AX_INLINE constexpr		void		copyValues	(Span<T> v, Int offset = 0)		{ span().copyValues(v, offset); }
	AX_INLINE constexpr		void		fillValues	(const T& v)					{ span().fillValues(v); }

	//---------------
	using  Iter			= T*;
	using CIter			= const T*;

	constexpr  Iter	begin	()			{ return data(); }
	constexpr CIter	begin	() const	{ return data(); }
	constexpr  Iter	end		()			{ return data() + size(); }
	constexpr CIter	end		() const	{ return data() + size(); }

private:
	AX_INLINE constexpr void _checkBound			( Int i ) const { if( ! inBound(i) ) throw Error_IndexOutOfRange(); }
	AX_INLINE constexpr void	_debug_boundCheck	( Int i ) const {
		#ifdef AX_BUILD_CONFIG_Debug
			_checkBound(i);
		#endif
	}
};

template<class T> constexpr bool Type_IsFixedArray = false; 
template<class T, Int N> constexpr bool Type_IsFixedArray<FixedArray<T, N>> = true; 


} // namespace
