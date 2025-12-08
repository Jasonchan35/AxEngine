module;
#include "AxCore-pch.h"

export module AxCore.FixedSpan;
export import AxCore.Span;

export namespace ax {

template<class T, Int N> class MutFixedSpan;
template<class T, Int N> using FixedSpan = MutFixedSpan<const T, N>;

template<class T, Int N>
class MutFixedSpan {
	using This = MutFixedSpan;
public:
	Int	kSize = N;

	MutFixedSpan() = default;
	AX_INLINE constexpr MutFixedSpan(T* data) : _data(data) {}

	AX_INLINE constexpr operator MutFixedSpan<const T, N>() const { return MutFixedSpan<const T, N>(_data); }

	AX_INLINE constexpr T*		 data() { return _data; }
	AX_INLINE constexpr const T* data() const { return _data; }
	AX_INLINE constexpr Int		 size() const { return N; }

	AX_INLINE constexpr MutSpan<T>	span() { return MutSpan<T>(_data, N); }
	AX_INLINE constexpr operator MutSpan<T>() { return span(); }

	AX_INLINE	constexpr		T &	operator[]	( Int i )			{ return at(i); }
	AX_INLINE	constexpr const T &	operator[]	( Int i ) const		{ return at(i); }

	AX_INLINE	constexpr		T &	at			( Int i )			{ _checkBound(i); return _data[i]; }
	AX_INLINE	constexpr const T &	at			( Int i ) const		{ _checkBound(i); return _data[i]; }

	AX_INLINE	constexpr		T *	try_at		( Int i )			{ return inBound(i) ? &_data[i] : nullptr; }
	AX_INLINE	constexpr const T *	try_at		( Int i ) const		{ return inBound(i) ? &_data[i] : nullptr; }
	
	AX_INLINE 	constexpr 		T &	back		()					{ return at( kSize-1 ); }
	AX_INLINE 	constexpr const T &	back		() const			{ return at( kSize-1 ); }

	AX_INLINE 	constexpr 		T &	back		( Int i )			{ return at( kSize-i-1 ); }
	AX_INLINE 	constexpr const T &	back		( Int i ) const		{ return at( kSize-i-1 ); }

	AX_INLINE 	constexpr 		T &	unsafe_at	( Int i ) 			{ _debug_checkBound(i); return _data[i]; }
	AX_INLINE 	constexpr const T &	unsafe_at	( Int i ) const		{ _debug_checkBound(i); return _data[i]; }

	AX_INLINE 	constexpr 		T &	unsafe_back	( Int i )			{ return unsafe_at( kSize - i - 1 ); }
	AX_INLINE 	constexpr const T &	unsafe_back	( Int i )  const	{ return unsafe_at( kSize - i - 1 ); }

	AX_INLINE	constexpr bool operator==(const This& rhs) const {
		for (Int i = 0; i < N; ++i) {
			if (_data[i] != rhs._data[i]) return false;
		}
		return true;
	}
	
	AX_INLINE	constexpr bool		inBound		(Int  i) const		{ return i >= 0 && i < kSize; }

	template<class TT> using Iter = TT*; 
	constexpr Iter<T>		begin	()		 noexcept	{ return _data; }
	constexpr Iter<const T>	begin	() const noexcept	{ return _data; }
	constexpr Iter<T>		end		()		 noexcept	{ return _data + N; }
	constexpr Iter<const T>	end		() const noexcept	{ return _data + N; }

	template<class TT> using RevForEach_ = Span_RevForEach_<TT>;
	constexpr auto	revForEach	()			{ return RevForEach_<      T>::s_make( _data, _data + N ); }
	constexpr auto	revForEach	() const	{ return RevForEach_<const T>::s_make( _data, _data + N ); }

private:
	AX_INLINE void _checkBound(Int i) const {
		if (!inBound(i)) throw Error_IndexOutOfRange();
	}
	AX_INLINE void _debug_checkBound(Int i) const {
#ifdef _DEBUG
		_checkBound(i);
#endif
	}
	T*	_data = nullptr;
};

} // namespace 