module;

#include "AxCore-pch.h"

export module AxCore.Math;
import AxCore.BasicType;

export namespace ax::Math {

//-------------------------
template< class T > constexpr Int sign( const T& a ) {
	if constexpr (std::is_signed_v<T>) {
		if (a < 0) return -1;
	}
	return (a > 0) ? 1 : 0;
}

template< class T > constexpr T	abs	( const T& a )	{ return a >= 0 ? a : -a ; }

template< class T > constexpr T	min	( const T& a, const T& b )					{ return (a<b)?a:b; }
template< class T > constexpr T	max	( const T& a, const T& b )					{ return (a>b)?a:b; }

template< class T > constexpr T	min	( const T& a, const T& b, const T& c )		{ return min(min(a,b),c); }
template< class T > constexpr T	max	( const T& a, const T& b, const T& c )		{ return max(max(a,b),c); }

template< class T > constexpr T	min	( const T& a, const T& b, const T& c, const T& d )	{ return min(min(a,b), min(c,d)); }
template< class T > constexpr T	max	( const T& a, const T& b, const T& c, const T& d )	{ return max(max(a,b), max(c,d)); }

template< class T > constexpr T	min0	( const T& a )	{ return min(a, T(0)); }
template< class T > constexpr T	max0	( const T& a )	{ return max(a, T(0)); }

template< class T > constexpr T	min1	( const T& a )	{ return min(a, T(1)); }
template< class T > constexpr T	max1	( const T& a )	{ return max(a, T(1)); }

//! x clamped to the range [a,b]
template< class T > constexpr T	clamp	( const T& x, const T& a, const T & b )		{ return max(a, min(b,x)); }
template< class T > constexpr T	clamp01	( const T& x )								{ return clamp(x, T(0), T(1)); }

template< class T > constexpr bool	isInRange	(const T& x, const T& a, const T & b)		{ return x >= a && x <= b; }

template< class T > constexpr bool	isPow2 ( const T& v )	{ return v != 0 && (v & (v - 1)) == 0; }

AX_INLINE constexpr i8	nextPow2	( i8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return max0(v); }
AX_INLINE constexpr i16	nextPow2	( i16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return max0(v); }
AX_INLINE constexpr i32	nextPow2	( i32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return max0(v); }
AX_INLINE constexpr i64	nextPow2	( i64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return max0(v); }

AX_INLINE constexpr u8	nextPow2	( u8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return v; }
AX_INLINE constexpr u16	nextPow2	( u16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return v; }
AX_INLINE constexpr u32	nextPow2	( u32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return v; }
AX_INLINE constexpr u64	nextPow2	( u64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return v; }

template< class T > AX_INLINE
constexpr T nextPow2_half(const T& v) {
	auto o = nextPow2(v);
	auto h = o - (o >> 2);
	return h >= v ? h : o;
}

template<class T> AX_INLINE
constexpr T alignTo(T n, T a) {
	if constexpr(std::is_floating_point_v<T>) {
		T i = floor( n / a ) * a;
		if (almostEqual(i,n)) return i;
		return (n > 0) ? i+a : i-a;

	} else if constexpr (std::is_integral_v<T>) {
		if constexpr(std::is_unsigned_v<T>) {
			T r = n % a;
			if( r == 0 ) return n;
			return n + (a-r);

		} else {
			a = abs(a);
			if( n >= 0 ) {
				T r = n % a;
				if( r == 0 ) return n;
				return n + (a-r);
			}else{
				T r = (-n) % a;
				if( r == 0 ) return n;
				return n - (a-r);
			}
		}
	} else {
		static_assert(false);
	}
}

template<class T> constexpr T isAlignTo(T n, T a) { return alignTo(n,a) == n; }

} // namespace

