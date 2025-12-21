module;
export module AxCore.Vec;
export import AxCore.VecSimd;
export import AxCore.Random;

export namespace  ax {

template<Int N, class T, VecSimd SIMD> class Vec_;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec1_ = Vec_<1, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec2_ = Vec_<2, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec3_ = Vec_<3, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec4_ = Vec_<4, T, SIMD>;

using Vec1h			= Vec1_<f16>;
using Vec1h_SSE		= Vec1_<f16, VecSimd::SSE>;
using Vec1h_Basic	= Vec1_<f16, VecSimd::None>;
using Vec2h			= Vec2_<f16>;
using Vec2h_SSE		= Vec2_<f16, VecSimd::SSE>;
using Vec2h_Basic	= Vec2_<f16, VecSimd::None>;
using Vec3h			= Vec3_<f16>;
using Vec3h_SSE		= Vec3_<f16, VecSimd::SSE>;
using Vec3h_Basic	= Vec3_<f16, VecSimd::None>;
using Vec4h			= Vec4_<f16>;
using Vec4h_SSE		= Vec4_<f16, VecSimd::SSE>;
using Vec4h_Basic	= Vec4_<f16, VecSimd::None>;

using Vec1f			= Vec1_<f32>;
using Vec1f_SSE		= Vec1_<f32, VecSimd::SSE>;
using Vec1f_Basic	= Vec1_<f32, VecSimd::None>;
using Vec2f			= Vec2_<f32>;
using Vec2f_SSE		= Vec2_<f32, VecSimd::SSE>;
using Vec2f_Basic	= Vec2_<f32, VecSimd::None>;
using Vec3f			= Vec3_<f32>;
using Vec3f_SSE		= Vec3_<f32, VecSimd::SSE>;
using Vec3f_Basic	= Vec3_<f32, VecSimd::None>;
using Vec4f			= Vec4_<f32>;
using Vec4f_SSE		= Vec4_<f32, VecSimd::SSE>;
using Vec4f_Basic	= Vec4_<f32, VecSimd::None>;

using Vec1d			= Vec1_<f64>;
using Vec1d_SSE		= Vec1_<f64, VecSimd::SSE>;
using Vec1d_Basic	= Vec1_<f64, VecSimd::None>;
using Vec2d			= Vec2_<f64>;
using Vec2d_SSE		= Vec2_<f64, VecSimd::SSE>;
using Vec2d_Basic	= Vec2_<f64, VecSimd::None>;
using Vec3d			= Vec3_<f64>;
using Vec3d_SSE		= Vec3_<f64, VecSimd::SSE>;
using Vec3d_Basic	= Vec3_<f64, VecSimd::None>;
using Vec4d			= Vec4_<f64>;
using Vec4d_SSE		= Vec4_<f64, VecSimd::SSE>;
using Vec4d_Basic	= Vec4_<f64, VecSimd::None>;

using Vec1i			= Vec1_<Int>;
using Vec1i_SSE		= Vec1_<Int, VecSimd::SSE>;
using Vec1i_Basic	= Vec1_<Int, VecSimd::None>;
using Vec2i			= Vec2_<Int>;
using Vec2i_SSE		= Vec2_<Int, VecSimd::SSE>;
using Vec2i_Basic	= Vec2_<Int, VecSimd::None>;
using Vec3i			= Vec3_<Int>;
using Vec3i_SSE		= Vec3_<Int, VecSimd::SSE>;
using Vec3i_Basic	= Vec3_<Int, VecSimd::None>;
using Vec4i			= Vec4_<Int>;
using Vec4i_SSE		= Vec4_<Int, VecSimd::SSE>;
using Vec4i_Basic	= Vec4_<Int, VecSimd::None>;

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator+(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) + vec;
}

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator-(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) - vec;
}

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator*(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) * vec;
}

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator/(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) / vec;
}

template<class T, VecSimd SIMD>
class Vec_<1, T, SIMD> {
	static constexpr Int N = 1;
	using This = Vec_;
public:
	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Num1 = Num1_<T>;
	
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num1& v) : _simd(v.e00) {}
	AX_INLINE constexpr Vec_(const T& x_) : _simd(x_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	AX_INLINE constexpr void set(const T& x_) { *this = This(x_); }

	constexpr Num1 toNum() const { return Num2(x); }
	constexpr operator Num1() const { return toNum(); }
	
	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); } 

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero() const { return _simd.almostZero(); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }
	
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

template<class T, VecSimd SIMD>
class Vec_<2, T, SIMD> {
	static constexpr Int N = 2;
	using This = Vec_;
public:
	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Num2 = Num2_<T>;
	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
		
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num2& v) : _simd(v.e00, v.e01) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_) : _simd(x_, y_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	AX_INLINE constexpr void set(const T& x_, const T& y_) { *this = This(x_,y_); }

	constexpr Num2 toNum() const { return Num2(x,y); }
	constexpr operator Num2() const { return toNum(); }
	
	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); } 

		AX_NODISCARD AX_INLINE constexpr Vec3 xy0 () const { return Vec3(x,y,0); }
	AX_NODISCARD AX_INLINE constexpr Vec4 xy01() const { return Vec4(x,y,0,1); }
	
	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero() const { return _simd.almostZero(); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }
	
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
	
	static This s_randomOnCircle(RandomDevice& dev = RandomDevice::s_default()) {
		auto theta = dev.getRange<T>(0, 1) * 2 * Math::PI_<T>;
		This v;
		Math::sincos(theta, v.x, v.y);
		return v;
	}

	static Vec2_<T, SIMD> s_randomInsideCircle(RandomDevice& dev = RandomDevice::s_default()) {
		auto v = s_randomOnCircle(dev);
		auto r = dev.getRange<T>(0, 1);
		return v * Math::sqrt(r); // distribution in square root along radius
	}

#if AX_OS_WINDOWS
	AX_NODISCARD AX_INLINE static constexpr This s_from(const ::POINT& r) {
		return This(static_cast<T>(r.x), static_cast<T>(r.y));
	}

	AX_NODISCARD AX_INLINE constexpr POINT to_POINT() const {
		POINT o;
		o.x = static_cast<LONG>(x);
		o.y = static_cast<LONG>(y);
		return o;
	}
#endif
	
};

template<class T, VecSimd SIMD>
class Vec_<3, T, SIMD> {
	static constexpr Int N = 3;
	using This = Vec_;
public:
	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Num3 = Num3_<T>;
	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
		
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y, z; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num3& v) : _simd(v.e00, v.e01, v.e02) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_, const T& z_) : _simd(x_, y_, z_) {}
	AX_INLINE constexpr Vec_(const Vec2& v_, const T& z_) : _simd(v_.x, v_.y, z_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_) { *this = This(x_,y_,z_); }
	AX_INLINE constexpr void set(const Vec2& v_, const T& z_) { *this = This(v_.x,v_.y,z_); }

	constexpr Num3 toNum() const { return Num3(x,y,z); }
	constexpr operator Num3() const { return toNum(); }
	
	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); }

	AX_NODISCARD AX_INLINE constexpr Vec2 xx() const { return Vec2(x,x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 xy() const { return Vec2(x,y); }
	AX_NODISCARD AX_INLINE constexpr Vec2 xz() const { return Vec2(x,z); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yx() const { return Vec2(y,x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yy() const { return Vec2(y,y); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yz() const { return Vec2(y,z); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zx() const { return Vec2(z,x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zy() const { return Vec2(z,y); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zz() const { return Vec2(z,z); }

	AX_NODISCARD AX_INLINE constexpr Vec4 xyz1() const { return Vec4(x,y,z,1); }
	
	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero() const { return _simd.almostZero(); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }
	
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	static This s_randomOnSphere(RandomDevice& dev = RandomDevice::s_default()) {
		auto longitude = dev.getRange<T>(0, 1) * 2 * Math::PI_<T>;
		auto latitude  = acos(dev.getRange<T>(0, 1) * 2 - 1);

		T sinLongitude, cosLongitude;
		T sinLatitude,  cosLatitude;

		Math::sincos(longitude, sinLongitude, cosLongitude);
		Math::sincos(latitude, sinLatitude, cosLatitude);

		Vec3_<T, SIMD> v(sinLatitude * cosLongitude, sinLatitude * sinLongitude, cosLatitude);
		return v;
	}

	static This s_randomInsideSphere(RandomDevice& dev = RandomDevice::s_default()) {
		auto v = s_randomOnSphere(dev);
		auto r = dev.getRange<T>(0, 1);
		return v * cbrt(r);
	}
};

template<class T, VecSimd SIMD>
class Vec_<4, T, SIMD> {
	static constexpr Int N = 4;
	using This = Vec_;
public:
	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Num4 = Num4_<T>;
	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
		
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y, z, w; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num4_<T>& v) : _simd(v) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_, const T& z_, const T& w_) : _simd(x_, y_, z_, w_) {}
	AX_INLINE constexpr Vec_(const Vec3& v_, const T& w_) : _simd(v_.x,v_.y,v_.z,w_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) { *this = This(x_,y_,z_,w_); }
	AX_INLINE constexpr void set(const Vec3& v_, const T& w_) { *this = This(v_.x,v_.y,v_.z,w_); }

	constexpr Num4 toNum() const { return Num4(x,y,z,w); }
	constexpr operator Num4() const { return toNum(); }
	
	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); }

	AX_INLINE constexpr Vec3	xyz_direct() const	{ return Vec3(x,y,z); }
	AX_INLINE constexpr Vec3	xyz_div_w() const	{ return (*this / w).xyz_direct(); } // de-homogenize

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero() const { return _simd.almostZero(); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }
	
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

} // namespace 