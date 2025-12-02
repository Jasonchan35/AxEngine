module;
#include "AxCore-pch.h"
export module AxCore.Vec;
export import AxCore.VecSimd;

export namespace  ax {

template<Int N, class T, VecSIMD SIMD> class Vec_;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using Vec1_ = Vec_<1, T, SIMD>;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using Vec2_ = Vec_<2, T, SIMD>;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using Vec3_ = Vec_<3, T, SIMD>;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using Vec4_ = Vec_<4, T, SIMD>;

using Vec1h			= Vec1_<f16>;
using Vec1h_SSE		= Vec1_<f16, VecSIMD::SSE>;
using Vec1h_Basic	= Vec1_<f16, VecSIMD::None>;
using Vec2h			= Vec2_<f16>;
using Vec2h_SSE		= Vec2_<f16, VecSIMD::SSE>;
using Vec2h_Basic	= Vec2_<f16, VecSIMD::None>;
using Vec3h			= Vec3_<f16>;
using Vec3h_SSE		= Vec3_<f16, VecSIMD::SSE>;
using Vec3h_Basic	= Vec3_<f16, VecSIMD::None>;
using Vec4h			= Vec4_<f16>;
using Vec4h_SSE		= Vec4_<f16, VecSIMD::SSE>;
using Vec4h_Basic	= Vec4_<f16, VecSIMD::None>;

using Vec1f			= Vec1_<f32>;
using Vec1f_SSE		= Vec1_<f32, VecSIMD::SSE>;
using Vec1f_Basic	= Vec1_<f32, VecSIMD::None>;
using Vec2f			= Vec2_<f32>;
using Vec2f_SSE		= Vec2_<f32, VecSIMD::SSE>;
using Vec2f_Basic	= Vec2_<f32, VecSIMD::None>;
using Vec3f			= Vec3_<f32>;
using Vec3f_SSE		= Vec3_<f32, VecSIMD::SSE>;
using Vec3f_Basic	= Vec3_<f32, VecSIMD::None>;
using Vec4f			= Vec4_<f32>;
using Vec4f_SSE		= Vec4_<f32, VecSIMD::SSE>;
using Vec4f_Basic	= Vec4_<f32, VecSIMD::None>;

using Vec1d			= Vec1_<f64>;
using Vec1d_SSE		= Vec1_<f64, VecSIMD::SSE>;
using Vec1d_Basic	= Vec1_<f64, VecSIMD::None>;
using Vec2d			= Vec2_<f64>;
using Vec2d_SSE		= Vec2_<f64, VecSIMD::SSE>;
using Vec2d_Basic	= Vec2_<f64, VecSIMD::None>;
using Vec3d			= Vec3_<f64>;
using Vec3d_SSE		= Vec3_<f64, VecSIMD::SSE>;
using Vec3d_Basic	= Vec3_<f64, VecSIMD::None>;
using Vec4d			= Vec4_<f64>;
using Vec4d_SSE		= Vec4_<f64, VecSIMD::SSE>;
using Vec4d_Basic	= Vec4_<f64, VecSIMD::None>;

using Vec1i			= Vec1_<Int>;
using Vec1i_SSE		= Vec1_<Int, VecSIMD::SSE>;
using Vec1i_Basic	= Vec1_<Int, VecSIMD::None>;
using Vec2i			= Vec2_<Int>;
using Vec2i_SSE		= Vec2_<Int, VecSIMD::SSE>;
using Vec2i_Basic	= Vec2_<Int, VecSIMD::None>;
using Vec3i			= Vec3_<Int>;
using Vec3i_SSE		= Vec3_<Int, VecSIMD::SSE>;
using Vec3i_Basic	= Vec3_<Int, VecSIMD::None>;
using Vec4i			= Vec4_<Int>;
using Vec4i_SSE		= Vec4_<Int, VecSIMD::SSE>;
using Vec4i_Basic	= Vec4_<Int, VecSIMD::None>;

template <Int N, class T, VecSIMD SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator+(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) + vec;
}

template <Int N, class T, VecSIMD SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator-(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) - vec;
}

template <Int N, class T, VecSIMD SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator*(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) * vec;
}

template <Int N, class T, VecSIMD SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator/(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) / vec;
}

template<class T, VecSIMD SIMD>
class Vec_<1, T, SIMD> {
	static constexpr Int N = 1;
	using This = Vec_;
public:
	using _NumLimit = VecSIMD_NumLimit<This, T>;
	static constexpr Int elementCount = N;
	static constexpr VecSIMD cpuSIMD = SIMD;

	using Num1 = Num1_<T>;
	
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T x; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SIMD_Data & simd) : _simd(simd) {}
	AX_INLINE constexpr Vec_(Tag::All_, const T& t) : _simd(s_all(t)) {}
	AX_INLINE constexpr Vec_(const Num1& v) : _simd(v.e00) {}
	AX_INLINE constexpr Vec_(const T& x_) : _simd(x_) {}

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SIMD_Data::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SIMD_Data::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SIMD_Data::s_one(); } 

	template<VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
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
	
	template <class R, VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SIMD_Data::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
};

template<class T, VecSIMD SIMD>
class Vec_<2, T, SIMD> {
	static constexpr Int N = 2;
	using This = Vec_;
public:
	using _NumLimit = VecSIMD_NumLimit<This, T>;
	static constexpr Int elementCount = N;
	static constexpr VecSIMD cpuSIMD = SIMD;

	using Num2 = Num2_<T>;
		
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T x, y; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SIMD_Data & simd) : _simd(simd) {}
	AX_INLINE constexpr Vec_(Tag::All_, const T& vec) : _simd(SIMD_Data::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num2& v) : _simd(v.e00, v.e01) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_) : _simd(x_, y_) {}

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SIMD_Data::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SIMD_Data::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SIMD_Data::s_one(); } 

	template<VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
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
	
	template <class R, VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SIMD_Data::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
};

template<class T, VecSIMD SIMD>
class Vec_<3, T, SIMD> {
	static constexpr Int N = 3;
	using This = Vec_;
public:
	using _NumLimit = VecSIMD_NumLimit<This, T>;
	static constexpr Int elementCount = N;
	static constexpr VecSIMD cpuSIMD = SIMD;

	using Num3 = Num3_<T>;
		
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T x, y, z; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SIMD_Data & simd) : _simd(simd) {}
	AX_INLINE constexpr Vec_(Tag::All_, const T& vec) : _simd(SIMD_Data::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num3& v) : _simd(v.e00, v.e01, v.e02) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_, const T& z_) : _simd(x_, y_, z_) {}

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SIMD_Data::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SIMD_Data::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SIMD_Data::s_one(); } 

	template<VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
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
	
	template <class R, VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SIMD_Data::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
};

template<class T, VecSIMD SIMD>
class Vec_<4, T, SIMD> {
	static constexpr Int N = 4;
	using This = Vec_;
public:
	using _NumLimit = VecSIMD_NumLimit<This, T>;
	static constexpr Int elementCount = N;
	static constexpr VecSIMD cpuSIMD = SIMD;

	using Num4 = Num4_<T>;
		
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T x, y, z, w; };
	};
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SIMD_Data & simd) : _simd(simd) {}
	AX_INLINE constexpr Vec_(Tag::All_, const T& vec) : _simd(SIMD_Data::s_all(vec)) {}
	AX_INLINE constexpr Vec_(const Num4& v) : _simd(v.e00, v.e01, v.e02, v.e03) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_, const T& z_, const T& w_) : _simd(x_, y_, z_, w_) {}

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SIMD_Data::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SIMD_Data::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SIMD_Data::s_one(); } 

	template<VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
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
	
	template <class R, VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SIMD_Data::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
};

} // namespace 