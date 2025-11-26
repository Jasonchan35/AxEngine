module;
#include "AxCore-pch.h"

export module AxCore.Math;
export import AxCore._PCH;

export namespace ax::Math {

template< class T>	inline constexpr T		PI_			= T(3.14159265358979323846);
template<class T>	inline constexpr T		PI_2_		= PI_<T> * T(2);
template<class T>	inline constexpr T		PI_half_	= PI_<T> * T(0.5);

inline constexpr Float PI		= PI_<Float>;
inline constexpr Float PI_2		= PI_2_<Float>;
inline constexpr Float PI_half	= PI_half_<Float>;

template<class T> AX_INLINE T	radians	(T deg) { return deg * (PI_<T> / T(180.0)); }
template<class T> AX_INLINE T	degrees	(T rad) { return rad * (T(180.0) / PI_<T>); }

template< class T > constexpr T	rcp	( const T& a )	{ return safeDiv(T(1), a); }

AX_INLINE float		sin	( float  rad ) { return std::sin(rad); }
AX_INLINE double	sin	( double rad ) { return std::sin(rad); }

AX_INLINE float		cos	( float  rad ) { return std::cos(rad); }
AX_INLINE double	cos	( double rad ) { return std::cos(rad); }

#if AX_OS_MACOSX
	AX_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { ::__sincosf(rad, &out_sin, &out_cos); }
	AX_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { ::__sincos (rad, &out_sin, &out_cos); }
#elif AX_OS_LINUX
	AX_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { ::sincosf(rad, &out_sin, &out_cos); }
	AX_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { ::sincos (rad, &out_sin, &out_cos); }
#else
	AX_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { out_sin = sin(rad); out_cos = cos(rad); }
	AX_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { out_sin = sin(rad); out_cos = cos(rad); }
#endif

AX_INLINE float		tan	( float  rad ) { return std::tan(rad); }
AX_INLINE double	tan	( double rad ) { return std::tan(rad); }

AX_INLINE float		asin( float  rad ) { return std::asin(rad); }
AX_INLINE double	asin( double rad ) { return std::asin(rad); }

AX_INLINE float		acos( float  rad ) { return std::acos(rad); }
AX_INLINE double	acos( double rad ) { return std::acos(rad); }

AX_INLINE float		atan2( float  a, float  b ) { return std::atan2(a,b); }
AX_INLINE double	atan2( double a, double b ) { return std::atan2(a,b); }



//-------------------------
AX_INLINE f32	pow(f32 base, f32 exp)	{ return std::pow(base, exp); }
AX_INLINE f64	pow(f64 base, f64 exp)	{ return std::pow(base, exp); }

//Splits a floating-point value into fractional and integer parts
AX_INLINE float		modf	( float  v, float  *i ) { return std::modf( v, i ); }
AX_INLINE double	modf	( double v, double *i ) { return std::modf( v, i ); }

//get remainder
AX_INLINE float		fmod	( float  a, float  b ) { return std::fmod(a,b); }
AX_INLINE double	fmod	( double a, double b ) { return std::fmod(a,b); }

AX_INLINE float		frac	( float  v )	{ float  intPart; return modf(v, &intPart); }
AX_INLINE double	frac	( double v )	{ double intPart; return modf(v, &intPart); }

template<class T>
constexpr Int  roundup_div(T x, T d) {
	static_assert(std::is_integral_v<T>);
	return (x > 0 ? x + (abs(d) - 1) : x - (abs(d) - 1)) / d;
}

template<class T> AX_INLINE void round_it	( T & v ) { v = round(v); }
template<class T> AX_INLINE void floor_it	( T & v ) { v = floor(v); }
template<class T> AX_INLINE void ceil_it 	( T & v ) { v = ceil (v); }

//------------------------------------
template<class T> AX_INLINE T ax_square(const T& v) { return v * v; }

AX_INLINE float		sqrt(float  n) { return std::sqrt(n); }
AX_INLINE double	sqrt(double n) { return std::sqrt(n); }
AX_INLINE Int		sqrt(Int    n) { return static_cast<Int>(std::sqrt(static_cast<double>(n))); }

AX_INLINE float		cbrt(float  n) { return std::cbrt(n); }
AX_INLINE double	cbrt(double n) { return std::cbrt(n); }
AX_INLINE Int		cbrt(Int    n) { return static_cast<Int>(std::cbrt(static_cast<double>(n))); }

//------- reciprocal square root ---------------
AX_INLINE float rsqrt_fast(float n) {
#if AX_CPU_FEATURE_SSE2
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(n)));
#else // https://en.wikipedia.org/wiki/Fast_inverse_square_root
	uint32_t i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = n * 0.5f;
	y = n;
	i = ax_bit_cast<u32>(y);
	i = 0x5F375A86UL - (i >> 1);
	y = ax_bit_cast<float>(i);
	y = y * (threehalfs - (x2 * y * y));
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
#endif
}

AX_INLINE double rsqrt_fast(double n) {
	u64 i;
	double x2, y;
	const double threehalfs = 1.5;

	x2 = n * 0.5;
	y = n;
	i = ax_bit_cast<u64>(y);
	i = 0x5FE6EB50C7B537A9ULL - (i >> 1);
	y = ax_bit_cast<double>(i);
	y = y * (threehalfs - (x2 * y * y));
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
}

AX_INLINE Int rsqrt_fast(Int n) {
	double d = static_cast<double>(n);
	return roundToInt(rsqrt_fast(d));
}

AX_INLINE float rsqrt(float v) {
#if 0 //AX_CPU_FEATURE_SSE2
	float  xrsqrt_est = rsqrt_fast(v);
	return xrsqrt_est * (1.5f - v * 0.5f * xrsqrt_est * xrsqrt_est); // NR iteration
#else
	return rcp(sqrt(v));
#endif
}

AX_INLINE double rsqrt(double v) { return rcp(sqrt(v)); }
AX_INLINE Int    rsqrt(Int    v) { return roundToInt(rsqrt(static_cast<double>(v))); }

template<class T, class WEIGHT = Type_Float_From<T>> AX_INLINE constexpr
T lerp(const T& a, const T& b, const WEIGHT& w) {
	if constexpr (std::is_integral_v<T>) {
		return static_cast<T>(round(static_cast<WEIGHT>(a) + w * static_cast<WEIGHT>(b - a)));
	} else {
		return (1 - w) * a + w * b;
	}
}

template<class T> requires Type_IsFloat<T>
AX_INLINE constexpr T smoothstep(const T& w) { return (-2.0f * w * w * w + 3.0f * w * w); }

template<class T> AX_INLINE constexpr 
T step(const T& a, const T& x) { return x>=a ? T(1) : T(0); }

template<class T> AX_INLINE constexpr
T invLerp(const T& from, const T& to, const T& value) {
	if constexpr (std::is_integral_v<T>) {
		
	} else {
		return (value - from) / (to - from);
	}
}

template<class T> AX_INLINE constexpr T easeIn (const T& a) { return a * a; }
template<class T> AX_INLINE constexpr T easeOut(const T& a) { return 1 - a * a; }


} // namespace
