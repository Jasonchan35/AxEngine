module;

export module AxCore.Math;
export import AxCore.BasicMath;
export import AxCore.BitFn;

export namespace ax::Math {

template <class T> requires Type_IsIntType<T>
AX_NODISCARD AX_INLINE constexpr Type_FloatTypeFrom<T> to_floating_point(const T& i) { return static_cast<Type_FloatTypeFrom<T>>(i); }

template<class T> struct PI_Struct;
template<> struct PI_Struct<f32> { static constexpr f32 value = 3.14159265358979323846f; };
template<> struct PI_Struct<f64> { static constexpr f64 value = 3.14159265358979323846; };
template<class T> constexpr T	PI_ = PI_Struct<T>::value;

template<class T> requires Type_IsFundamental<T> 
AX_NODISCARD AX_INLINE constexpr T radians(T deg) { return deg * (PI_<T> / T(180.0)); }
template<class T> requires Type_IsFundamental<T> 
AX_NODISCARD AX_INLINE constexpr T degrees(T rad) { return rad * (T(180.0) / PI_<T>); }

template< class T > requires Type_IsFundamental<T> 
constexpr T	reciprocal( const T& a ) { return safeDiv(T(1), a); }

AX_NODISCARD AX_INLINE float	sin	( float  rad ) { return std::sin(rad); }
AX_NODISCARD AX_INLINE double	sin	( double rad ) { return std::sin(rad); }

AX_NODISCARD AX_INLINE float	cos	( float  rad ) { return std::cos(rad); }
AX_NODISCARD AX_INLINE double	cos	( double rad ) { return std::cos(rad); }

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

AX_NODISCARD AX_INLINE float	tan	 ( float  rad ) { return std::tan(rad); }
AX_NODISCARD AX_INLINE double	tan	 ( double rad ) { return std::tan(rad); }
AX_NODISCARD AX_INLINE float	asin ( float  rad ) { return std::asin(rad); }
AX_NODISCARD AX_INLINE double	asin ( double rad ) { return std::asin(rad); }
AX_NODISCARD AX_INLINE float	acos ( float  rad ) { return std::acos(rad); }
AX_NODISCARD AX_INLINE double	acos ( double rad ) { return std::acos(rad); }
AX_NODISCARD AX_INLINE float	atan2( float  a, float  b ) { return std::atan2(a,b); }
AX_NODISCARD AX_INLINE double	atan2( double a, double b ) { return std::atan2(a,b); }

//-------------------------
AX_NODISCARD AX_INLINE f32		pow(f32 base, f32 exp)	{ return std::pow(base, exp); }
AX_NODISCARD AX_INLINE f64		pow(f64 base, f64 exp)	{ return std::pow(base, exp); }

//Splits a floating-point value into fractional and integer parts
AX_NODISCARD AX_INLINE float	modf	( float  v, float  *i ) { return std::modf( v, i ); }
AX_NODISCARD AX_INLINE double	modf	( double v, double *i ) { return std::modf( v, i ); }

//get remainder
AX_NODISCARD AX_INLINE float	fmod	( float  a, float  b ) { return std::fmod(a,b); }
AX_NODISCARD AX_INLINE double	fmod	( double a, double b ) { return std::fmod(a,b); }

AX_NODISCARD AX_INLINE float	frac	( float  v )	{ float  intPart; return modf(v, &intPart); }
AX_NODISCARD AX_INLINE double	frac	( double v )	{ double intPart; return modf(v, &intPart); }

template<class T> requires Type_IsSIntType<T>
AX_NODISCARD AX_INLINE constexpr
T roundup_div(T x, T d) {
	auto remind = abs(d) - 1;
	auto v = x > 0 ? x + remind : x - remind;
	return v / d;
}

template<class T> void round_itself	( T & v ) { v = round(v); }
template<class T> void floor_itself	( T & v ) { v = floor(v); }
template<class T> void ceil_itself 	( T & v ) { v = ceil (v); }

//------------------------------------
template<class T> AX_INLINE T square(const T& v) { return v * v; }

AX_NODISCARD AX_INLINE float	sqrt(float  n) { return std::sqrt(n); }
AX_NODISCARD AX_INLINE double	sqrt(double n) { return std::sqrt(n); }
template<class T> requires Type_IsIntType<T>
AX_NODISCARD AX_INLINE T		sqrt(T      n) { return static_cast<T>(sqrt(to_floating_point(n))); }

AX_NODISCARD AX_INLINE float	cbrt(float  n) { return std::cbrt(n); }
AX_NODISCARD AX_INLINE double	cbrt(double n) { return std::cbrt(n); }
template<class T> requires Type_IsIntType<T>
AX_NODISCARD AX_INLINE T		cbrt(T      n) { return static_cast<T>(cbrt(to_floating_point(n))); }

//------- reciprocal square root ---------------
AX_NODISCARD AX_INLINE float rsqrt_fast(float n) {
#if AX_CPU_FEATURE_SSE2
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(n)));
#else // https://en.wikipedia.org/wiki/Fast_inverse_square_root
	constexpr float three_half = 1.5f;
	float    x2 = n * 0.5f;
	float    y  = n;
	uint32_t i  = ax_bit_cast<u32>(y);
	i           = 0x5F375A86UL - (i >> 1);
	y           = ax_bit_cast<float>(i);
	y           = y * (three_half - (x2 * y * y));
	//	y  = y * ( three_half - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
#endif
}

AX_NODISCARD AX_INLINE double rsqrt_fast(double n) {
	constexpr double three_half = 1.5;
	double x2 = n * 0.5;
	double y  = n;
	u64    i  = ax_bit_cast<u64>(y);
	i         = 0x5FE6EB50C7B537A9ULL - (i >> 1);
	y         = ax_bit_cast<double>(i);
	y         = y * (three_half - (x2 * y * y));
	//	y  = y * ( three_half - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
}

template<class T> requires Type_IsIntType<T>
AX_NODISCARD AX_INLINE T rsqrt_fast(T n) {
	return static_cast<T>(rsqrt_fast(cast_Int_To_Float(n)));
}

AX_NODISCARD AX_INLINE float rsqrt(float v) {
#if 0 //AX_CPU_FEATURE_SSE2
	float  xrsqrt_est = rsqrt_fast(v);
	return xrsqrt_est * (1.5f - v * 0.5f * xrsqrt_est * xrsqrt_est); // NR iteration
#else
	return reciprocal(sqrt(v));
#endif
}

AX_NODISCARD AX_INLINE double rsqrt(double v) { return reciprocal(sqrt(v)); }

template<class T> requires Type_IsIntType<T>
AX_NODISCARD AX_INLINE T      rsqrt(T      v) { return static_cast<T>(rsqrt(to_floating_point(v))); }

template<class T, class WEIGHT = Type_FloatTypeFrom<T>> AX_NODISCARD AX_INLINE
T lerp(const T& a, const T& b, const WEIGHT& w) {
	if constexpr (std::is_integral_v<T>) {
		return static_cast<T>(round(static_cast<WEIGHT>(a) + w * static_cast<WEIGHT>(b - a)));
	} else {
		return (one_<T>() - w) * a + w * b;
	}
}

template<class T> requires Type_IsFloatType<T>
AX_NODISCARD AX_INLINE T smoothstep(const T& w) { return (T(-2.0) * w * w * w + T(3.0) * w * w); }

template<class T> AX_NODISCARD AX_INLINE 
T step(const T& a, const T& x) { return x >= a ? T(1) : T(0); }

template<class T> AX_NODISCARD AX_INLINE
T inv_lerp(const T& from, const T& to, const T& value) {
	if constexpr (std::is_integral_v<T>) {
		return roundToInt(inv_lerp(static_cast<f64>(from), static_cast<f64>(to), static_cast<f64>(value)));
	} else {
		return (value - from) / (to - from);
	}
}

template<class T> AX_NODISCARD AX_INLINE T easeIn (const T& a) { return a * a; }
template<class T> AX_NODISCARD AX_INLINE T easeOut(const T& a) { return one_<T>() - a * a; }


} // namespace
