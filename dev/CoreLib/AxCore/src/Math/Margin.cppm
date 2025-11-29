module;
#include "AxCore-pch.h"
export module AxCore.Margin;
export import AxCore.Vec;

export namespace ax {

template<Int N, class T, VecSIMD SIMD> class Margin_;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using Margin2_ = Margin_<2, T, SIMD>;

using Margin2h			= Margin2_<f16>;
using Margin2h_SSE		= Margin2_<f16, VecSIMD::SSE>;
using Margin2h_Basic	= Margin2_<f16, VecSIMD::None>;
using Margin2f			= Margin2_<f32>;
using Margin2f_SSE		= Margin2_<f32, VecSIMD::SSE>;
using Margin2f_Basic	= Margin2_<f32, VecSIMD::None>;
using Margin2d			= Margin2_<f64>;
using Margin2d_SSE		= Margin2_<f64, VecSIMD::SSE>;
using Margin2d_Basic	= Margin2_<f64, VecSIMD::None>;

template<class T, VecSIMD SIMD>
class Margin_<2, T, SIMD> {
	static constexpr Int N = 2;
	using This = Margin_;
public:
	using Element = T;
	static constexpr Int     elementCount = N;
	static constexpr VecSIMD cpuSIMD      = SIMD;

	using Vec2 = Vec2_<T, SIMD>;
	using SIMD_Data = VecSIMD_Data_<N * 2,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T top, right, bottom, left;}; // clock-wise, same as CSS Margin order
	};	
	
	AX_INLINE constexpr Margin_() = default;
	AX_INLINE constexpr Margin_(Tag::All_, const T& v) : _simd(Tag::All, v) {}
	AX_INLINE constexpr Margin_(const T& top_, const T& right_, const T& bottom_, const T& left_) : _simd(top_, right_, bottom_, left_) {}
	
	AX_NODISCARD Vec2	total	() const { return Vec2(totalX(), totalY()); }
	AX_NODISCARD T		totalX	() const { return left + right;  }
	AX_NODISCARD T		totalY	() const { return top  + bottom; }

	AX_NODISCARD Vec2	topLeft		() const { return Vec2(left,  top   ); }
	AX_NODISCARD Vec2	topRight	() const { return Vec2(right, top   ); }
	AX_NODISCARD Vec2	bottomLeft	() const { return Vec2(left,  bottom); }
	AX_NODISCARD Vec2	bottomRight	() const { return Vec2(right, bottom); }

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

	AX_NODISCARD AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_NODISCARD AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_NODISCARD AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_NODISCARD AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_NODISCARD AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_NODISCARD AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_NODISCARD AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_NODISCARD AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	template <class R, VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SIMD_Data::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }	
};

} // namespace 