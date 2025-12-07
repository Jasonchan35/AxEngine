module;

#include "AxCore-pch.h"
#include <cfloat>

export module AxCore.BasicMath;
export import AxCore.BasicType;

export namespace ax::Math {

template<Int N, class T> struct s_pos_struct;
template<class T> struct s_pos_struct<0,T> { static constexpr T compute(const T& v) { return 1; } };
template<class T> struct s_pos_struct<1,T> { static constexpr T compute(const T& v) { return v; } };
template<class T> struct s_pos_struct<2,T> { static constexpr T compute(const T& v) { return v * v; } };
template<class T> struct s_pos_struct<3,T> { static constexpr T compute(const T& v) { return v * v * v; } };

template<Int N, class T>
constexpr T s_pow(const T& v) { return s_pos_struct<N, T>::compute(v); } 

//-------------------------
template< class T > constexpr Int sign( const T& a ) {
	if constexpr (std::is_signed_v<T>) {
		if (a < 0) return -1;
	}
	return (a > 0) ? 1 : 0;
}

template <class T>                                inline constexpr T zero    = T::s_zero();
template <class T> requires Type_IsFundamental<T> inline constexpr T zero<T> = T(0);

template <class T>                                inline constexpr T one     = T::s_one();
template <class T> requires Type_IsFundamental<T> inline constexpr T one<T>  = T(1);

template< class T >                                AX_INLINE constexpr T	abs	( const T& a )	{ return a.abs(); }
template< class T > requires Type_IsFundamental<T> AX_INLINE constexpr T	abs	( const T& a )	{ return a >= T(0) ? a : -a ; }

template<class T>                                AX_NODISCARD AX_INLINE constexpr T min		( const T& a, const T& b )	{ return a.min(b); }
template<class T>                                AX_NODISCARD AX_INLINE constexpr T max		( const T& a, const T& b )	{ return a.max(b); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T min		( const T& a, const T& b )	{ return (a<b)?a:b; }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T max		( const T& a, const T& b )	{ return (a>b)?a:b; }

template<class T> AX_NODISCARD AX_INLINE T min	( const T& a, const T& b, const T& c )					{ return min(min(a,b),c); }
template<class T> AX_NODISCARD AX_INLINE T max	( const T& a, const T& b, const T& c )					{ return max(max(a,b),c); }
template<class T> AX_NODISCARD AX_INLINE T min	( const T& a, const T& b, const T& c, const T& d )		{ return min(min(a,b), min(c,d)); }
template<class T> AX_NODISCARD AX_INLINE T max	( const T& a, const T& b, const T& c, const T& d )		{ return max(max(a,b), max(c,d)); }

template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T	min_0	( const T& a )	{ return min(a, zero<T>); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T	max_0	( const T& a )	{ return max(a, zero<T>); }

template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T	min_1	( const T& a )	{ return min(a, one<T>); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T	max_1	( const T& a )	{ return max(a, one<T>); }

//! x clamped to the range [a,b]
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T	clamp	( const T& x, const T& a, const T & b )	{ return max(a, min(b,x)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr T	clamp_01( const T& x )							{ return clamp(x, zero<T>(), one<T>()); }

//--------------
template<class T> AX_INLINE constexpr void min_itself		(T& x, const T& a ) { x = min(x, a); }
template<class T> AX_INLINE constexpr void max_itself		(T& x, const T& a ) { x = max(x, a); }
template<class T> AX_INLINE constexpr void min_0_itself		(T& x, const T& a ) { x = min_0(x, a); }
template<class T> AX_INLINE constexpr void max_0_itself		(T& x, const T& a ) { x = max_0(x, a); }
template<class T> AX_INLINE constexpr void min_1_itself		(T& x, const T& a ) { x = min_1(x, a); }
template<class T> AX_INLINE constexpr void max_1_itself		(T& x, const T& a ) { x = max_1(x, a); }
template<class T> AX_INLINE constexpr void clamp_itself		(T& x, const T& a, const T & b ) { x = clamp(x, a, b); }
template<class T> AX_INLINE constexpr void clamp_01_itself	(T& x, const T& a, const T & b ) { x = clamp_01(x, a, b); }

//----- float ----

AX_NODISCARD AX_INLINE float	ceil	( float  a )	{ return std::ceil(a); }
AX_NODISCARD AX_INLINE double	ceil	( double a )	{ return std::ceil(a); }

AX_NODISCARD AX_INLINE float	floor	( float  a )	{ return std::floor(a); }
AX_NODISCARD AX_INLINE double	floor	( double a )	{ return std::floor(a); }

#if AX_COMPILER_VC | AX_OS_CYGWIN
	AX_NODISCARD AX_INLINE float  trunc	( float  n )	{ float  i; return std::modf( n, &i ); }
	AX_NODISCARD AX_INLINE double trunc	( double n )	{ double i; return std::modf( n, &i ); }

	AX_NODISCARD AX_INLINE float  round	( float  a )	{ return a > 0 ? floor(a+0.5f) : ceil(a-0.5f); }
	AX_NODISCARD AX_INLINE double round	( double a )	{ return a > 0 ? floor(a+0.5 ) : ceil(a-0.5 ); }
#else
	AX_NODISCARD AX_INLINE float  trunc	( float  n )	{ return std::trunc(n); }
	AX_NODISCARD AX_INLINE double trunc	( double n )	{ return std::trunc(n); }

	AX_NODISCARD AX_INLINE float  round	( float  a )	{ return std::round(a); }
	AX_NODISCARD AX_INLINE double round	( double a )	{ return std::round(a); }
#endif

template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int trunc_to_Int(const T& src) { return static_cast<Int>(trunc(src)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int round_to_Int(const T& src) { return static_cast<Int>(round(src)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int  ceil_to_Int(const T& src) { return static_cast<Int>(ceil( src)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int floor_to_Int(const T& src) { return static_cast<Int>(floor(src)); }

template<class T> inline constexpr T infinity_() { return NumLimit<T>::infinity; }
template<class T> inline constexpr T epsilon		= NumLimit<T>::epsilon;
template<class T> inline constexpr T NaN			= NumLimit<T>::NaN;
template<class T> inline constexpr T negInfinity	= NumLimit<T>::negInfinity;

struct infinity { template<class T> constexpr operator T() const { return infinity_<T>(); } };

template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr bool	isNaN			( const T& v ) { return std::isnan(v); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr bool	isInfinity		( const T& v ) { return NumLimit<T>::hasInfinity && v == infinity_<T>(); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr bool	isNegInfinity	( const T& v ) { return NumLimit<T>::hasInfinity && v == negInfinity<T>; }

template <class T> requires Type_AnyInt< T> AX_INLINE T fmod(const T& a, const T& b) { return a % b; }
template <class T> requires Type_Is_f32<T> AX_INLINE T fmod(const T& a, const T& b) { return ::fmodf(a, b); }
template <class T> requires Type_Is_f64<T> AX_INLINE T fmod(const T& a, const T& b) { return ::fmod(a, b); }

template<class T>
struct modf_Result {
	T int_part;		// integer part
	T frac_part;	// fractional part
};

template <class T> requires Type_AnyInt<T>
AX_NODISCARD AX_INLINE constexpr modf_Result<T> modf(const T& v) {
	modf_Result<T> o;
	o.int_part  = v;
	o.frac_part = 0;
	return o;
}

template <class T> requires Type_Is_f32<T>
AX_NODISCARD AX_INLINE constexpr modf_Result<T> modf(const T& v) {
	modf_Result<T> o;
	o.int_part = std::modff(v, &o.frac_part);
	return o;
}

template <class T> requires Type_Is_f64<T>
AX_NODISCARD AX_INLINE constexpr modf_Result<T> modf(const T& v) {
	modf_Result<T> o;
	o.int_part = std::modf(v, &o.frac_part);
	return o;
}

template<class T> requires Type_IsFundamental<T>
AX_NODISCARD AX_INLINE constexpr bool almostEqual(const T& a, const T& b) {
	if constexpr (Type_AnyInt<T>) {
		return a == b;
	} else {
		auto diff = abs(a - b);
		return diff <= epsilon<T>;
	}
}

template<class A, class B>
AX_NODISCARD AX_INLINE constexpr bool almostEqual(const A& a, const B& b) {
	return a.almostEqual(b);
}

template<class T> AX_NODISCARD AX_INLINE constexpr
bool almostZero(const T& a) {
	return (almostEqual(a, zero<T>));
}

template<class T> AX_NODISCARD AX_INLINE constexpr
bool exactlyEqual(const T& a, const T& b) {
	AX_PRAGMA_GCC(diagnostic push)
	AX_PRAGMA_GCC(diagnostic ignored "-Wfloat-equal")
	return a == b; 
	AX_PRAGMA_GCC(diagnostic pop)
}

template< class T > AX_NODISCARD AX_INLINE constexpr
T	safeDiv	( const T& a, const T& b )	{ return almostZero(b) ? zero<T> : a / b; }

template< class T > AX_NODISCARD AX_INLINE constexpr
bool	isInRange	(const T& x, const T& a, const T & b)		{ return x >= a && x <= b; }

template< class T > requires Type_AnyInt<T>
AX_NODISCARD AX_INLINE constexpr bool	isPow2		( const T& v )	{ return v != 0 && (v & (v - 1)) == 0; }

AX_NODISCARD AX_INLINE constexpr i8	nextPow2	( i8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return max_0(v); }
AX_NODISCARD AX_INLINE constexpr i16	nextPow2	( i16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return max_0(v); }
AX_NODISCARD AX_INLINE constexpr i32	nextPow2	( i32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return max_0(v); }
AX_NODISCARD AX_INLINE constexpr i64	nextPow2	( i64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return max_0(v); }

AX_NODISCARD AX_INLINE constexpr u8	nextPow2	( u8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return v; }
AX_NODISCARD AX_INLINE constexpr u16	nextPow2	( u16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return v; }
AX_NODISCARD AX_INLINE constexpr u32	nextPow2	( u32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return v; }
AX_NODISCARD AX_INLINE constexpr u64	nextPow2	( u64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return v; }

template< class T > AX_NODISCARD AX_INLINE
constexpr T nextPow2_half(const T& v) {
	auto o = nextPow2(v);
	auto h = o - (o >> 2);
	return h >= v ? h : o;
}

template<class T> requires Type_AnyFloat<T>
AX_NODISCARD AX_INLINE constexpr T alignTo(const T& n, const T& a) {
	if constexpr(std::is_floating_point_v<T>) {
		T i = floor( n / a ) * a;
		if (almostEqual(i,n)) return i;
		return (n > 0) ? i+a : i-a;
	}
}

template<class T> requires Type_AnyInt<T>
AX_NODISCARD AX_INLINE constexpr T alignTo(const T& n, const T& a) {
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
}

template<class T> AX_NODISCARD AX_INLINE constexpr T isAlignTo(const T& n, const T& a) { return alignTo(n,a) == n; }

} // namespace