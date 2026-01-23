module;

export module AxCore.BasicMath;
export import AxCore.BasicType;

export namespace ax::Math {

constexpr Int KiloBytes = 1024;
constexpr Int MegaBytes = KiloBytes * 1024;
constexpr Int GigaBytes = MegaBytes * 1024;
constexpr Int TeraBytes = GigaBytes * 1024;
constexpr Int PetaBytes = TeraBytes * 1024;

template<Int N, class T> struct pow_IMPL;
template<class T> struct pow_IMPL<0,T> { static constexpr T compute(const T& v) { return 1; } };
template<class T> struct pow_IMPL<1,T> { static constexpr T compute(const T& v) { return v; } };
template<class T> struct pow_IMPL<2,T> { static constexpr T compute(const T& v) { return v * v; } };
template<class T> struct pow_IMPL<3,T> { static constexpr T compute(const T& v) { return v * v * v; } };

template<Int N, class T>
constexpr T pow_(const T& v) { return pow_IMPL<N, T>::compute(v); } 

//-------------------------
template< class T > constexpr Int sign( const T& a ) {
	if constexpr (std::is_signed_v<T>) {
		if (a < 0) return -1;
	}
	return (a > 0) ? 1 : 0;
}

template<class T>
struct zero_T { static constexpr T get() { return T::s_zero(); } };

template<class T> requires Type_IsFundamental<T>
struct zero_T<T> { static constexpr T get() { return T(0); } };

template<class T>
constexpr T zero_() { return zero_T<T>::get(); }

struct zero { // auto return type
	template<class T> constexpr operator T() const { return zero_<T>(); }
};

template<class T>
struct one_T { static constexpr T get() { return T::s_one(); } };

template<class T> requires Type_IsFundamental<T>
struct one_T<T> { static constexpr T get() { return T(1); } };

template<class T>
constexpr T one_() { return one_T<T>::get(); }

struct one {
	template<class T> constexpr operator T() const { return one_<T>(); }
};

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

template<class T> AX_NODISCARD AX_INLINE constexpr T	min_0	( const T& a )	{ return min(a, zero_<T>()); }
template<class T> AX_NODISCARD AX_INLINE constexpr T	max_0	( const T& a )	{ return max(a, zero_<T>()); }

template<class T> AX_NODISCARD AX_INLINE constexpr T	min_1	( const T& a )	{ return min(a, one_<T>()); }
template<class T> AX_NODISCARD AX_INLINE constexpr T	max_1	( const T& a )	{ return max(a, one_<T>()); }

//! x clamped to the range [a,b]
template<class T> AX_NODISCARD AX_INLINE constexpr T	clamp	( const T& x, const T& a, const T& b )	{ return min(max(x, a), b); }
template<class T> AX_NODISCARD AX_INLINE constexpr T	clamp_01( const T& x )							{ return clamp(x, zero_<T>(), one_<T>()); }

//--------------
template<class T> AX_INLINE constexpr void min_itself		(T& x, const T& a ) { x = min<T>(x, a); }
template<class T> AX_INLINE constexpr void max_itself		(T& x, const T& a ) { x = max<T>(x, a); }
template<class T> AX_INLINE constexpr void min_0_itself		(T& x, const T& a ) { x = min_0<T>(x, a); }
template<class T> AX_INLINE constexpr void max_0_itself		(T& x, const T& a ) { x = max_0<T>(x, a); }
template<class T> AX_INLINE constexpr void min_1_itself		(T& x, const T& a ) { x = min_1<T>(x, a); }
template<class T> AX_INLINE constexpr void max_1_itself		(T& x, const T& a ) { x = max_1<T>(x, a); }
template<class T> AX_INLINE constexpr void clamp_itself		(T& x, const T& a, const T & b ) { x = clamp<T>(x, a, b); }
template<class T> AX_INLINE constexpr void clamp_01_itself	(T& x, const T& a, const T & b ) { x = clamp_01<T>(x, a, b); }

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

template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int truncToInt(const T& src) { return static_cast<Int>(trunc(src)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int roundToInt(const T& src) { return static_cast<Int>(round(src)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int  ceilToInt(const T& src) { return static_cast<Int>(ceil( src)); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr Int floorToInt(const T& src) { return static_cast<Int>(floor(src)); }

template<class T> inline constexpr T infinity_() { return NumLimit<T>::infinity(); }
struct infinity { template<class T> constexpr operator T() const { return infinity_<T>(); } };

template<class T> inline constexpr auto epsilon_()	{ return NumLimit<T>::epsilon(); }
struct epsilon { template<class T> constexpr operator T() const { return epsilon_<T>(); } };

template<class T> inline constexpr T NaN_() { return NumLimit<T>::NaN(); }
struct NaN { template<class T> constexpr operator T() const { return NaN_<T>(); } };

template<class T> inline constexpr T negInfinity_() { return NumLimit<T>::negInfinity(); }
struct negInfinity { template<class T> constexpr operator T() const { return negInfinity_<T>(); } };

template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr bool	isNaN			( const T& v ) { return std::isnan(v); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr bool	isInfinity		( const T& v ) { return NumLimit<T>::hasInfinity && v == infinity_<T>(); }
template<class T> requires Type_IsFundamental<T> AX_NODISCARD AX_INLINE constexpr bool	isNegInfinity	( const T& v ) { return NumLimit<T>::hasInfinity && v == negInfinity_<T>(); }

template <class T> T fmod(const T& a, const T& b);
template <> AX_INLINE f32 fmod(const f32& a, const f32& b) { return ::fmodf(a, b); }
template <> AX_INLINE f64 fmod(const f64& a, const f64& b) { return ::fmod(a, b); }

template <class T> requires Type_IsIntType< T>
AX_INLINE T fmod(const T& a, const T& b) { return a % b; }


template<class T>
struct modf_Result {
	T int_part;		// integer part
	T frac_part;	// fractional part
};

template <class T> requires Type_IsIntType<T>
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
AX_NODISCARD AX_INLINE constexpr bool almostEqual(const T& a, const T& b, const T& epsilon = Math::epsilon_<T>()) {
	if constexpr (Type_IsIntType<T>) {
		return a == b;
	} else {
		auto diff = abs(a - b);
		return diff <= epsilon;
	}
}

template<class A, class B, class EP = decltype(Math::epsilon_<A>())>
AX_NODISCARD AX_INLINE constexpr bool almostEqual(const A& a, const B& b, const EP& epsilon = Math::epsilon_<A>()) {
	return a.almostEqual(b, epsilon);
}

template<class T> AX_NODISCARD AX_INLINE constexpr
bool almostZero(const T& a, T epsilon = Math::epsilon_<T>()) {
	return almostEqual(a, epsilon);
}

template<class T> AX_NODISCARD AX_INLINE constexpr
bool exactlyEqual(const T& a, const T& b) {
	AX_GCC_WARNING_PUSH_AND_DISABLE("-Wfloat-equal")
	return a == b; 
	AX_GCC_WARNING_POP()
}

template<class T> requires Type_IsFundamental<T>
AX_NODISCARD AX_INLINE constexpr T safeDiv(const T& a, const T& b) { return almostZero(b) ? zero_<T>() : a / b; }

template< class T > AX_NODISCARD AX_INLINE constexpr
bool	isInRange	(const T& x, const T& a, const T & b)		{ return x >= a && x <= b; }

template< class T > requires Type_IsIntType<T>
AX_NODISCARD AX_INLINE constexpr bool	isPow2		( const T& v )	{ return v != 0 && (v & (v - 1)) == 0; }

AX_NODISCARD AX_INLINE constexpr i8		nextPow2	( i8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return max_0(v); }
AX_NODISCARD AX_INLINE constexpr i16	nextPow2	( i16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return max_0(v); }
AX_NODISCARD AX_INLINE constexpr i32	nextPow2	( i32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return max_0(v); }
AX_NODISCARD AX_INLINE constexpr i64	nextPow2	( i64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return max_0(v); }

AX_NODISCARD AX_INLINE constexpr u8		nextPow2	( u8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return v; }
AX_NODISCARD AX_INLINE constexpr u16	nextPow2	( u16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return v; }
AX_NODISCARD AX_INLINE constexpr u32	nextPow2	( u32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return v; }
AX_NODISCARD AX_INLINE constexpr u64	nextPow2	( u64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return v; }

template< class T > AX_NODISCARD AX_INLINE
constexpr T nextPow2_half(const T& v) {
	auto o = nextPow2(v);
	auto h = o - (o >> 2);
	return h >= v ? h : o;
}

template<class T>
AX_NODISCARD AX_INLINE constexpr T alignTo(const T& value, const T& alignment) {
	if constexpr (std::is_floating_point_v<T>) {
		T i = floor(value / alignment) * alignment;
		if (almostEqual(i, value)) return i;
		return (value > 0) ? i + alignment : i - alignment;

	} else if constexpr (std::is_unsigned_v<T>) {
		T remain = value % alignment;
		if (remain == 0) return value;
		return value + (alignment - remain);

	} else {
		T abs_a = abs(alignment);
		if (value >= 0) {
			T remain = value % abs_a;
			if (remain == 0) return value;
			return value + (abs_a - remain);
		} else {
			T remain = (-value) % abs_a;
			if (remain == 0) return value;
			return value - (abs_a - remain);
		}
	}
}

template<class T> AX_NODISCARD AX_INLINE constexpr T isAlignedTo(const T& n, const T& a) { return alignTo(n,a) == n; }

template<class T>
AX_NODISCARD AX_INLINE constexpr T alignDown(const T& value, const T& alignment) {
	auto tmp = alignTo(value, alignment);
	if (tmp == value) return value;
	if (value >= 0) {
		return tmp - alignment;
	} else {
		return tmp + alignment;
	}
}

} // namespace