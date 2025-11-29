module;
#include "AxCore-pch.h"
export module AxCore.BBox; // Bounding Box
export import AxCore.Vec;

export namespace ax {
template<Int N, class T, VecSIMD SIMD> class BBox_;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using BBox2_ = BBox_<2, T, SIMD>;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using BBox3_ = BBox_<3, T, SIMD>;

using BBox2h		= BBox2_<f16>;
using BBox2h_SSE	= BBox2_<f16, VecSIMD::SSE>;
using BBox2h_Basic	= BBox2_<f16, VecSIMD::None>;
using BBox2f		= BBox2_<f32>;
using BBox2f_SSE	= BBox2_<f32, VecSIMD::SSE>;
using BBox2f_Basic	= BBox2_<f32, VecSIMD::None>;
using BBox2d		= BBox2_<f64>;
using BBox2d_SSE	= BBox2_<f64, VecSIMD::SSE>;
using BBox2d_Basic	= BBox2_<f64, VecSIMD::None>;

using BBox3h		= BBox3_<f16>;
using BBox3h_SSE	= BBox3_<f16, VecSIMD::SSE>;
using BBox3h_Basic	= BBox3_<f16, VecSIMD::None>;
using BBox3f		= BBox3_<f32>;
using BBox3f_SSE	= BBox3_<f32, VecSIMD::SSE>;
using BBox3f_Basic	= BBox3_<f32, VecSIMD::None>;
using BBox3d		= BBox3_<f64>;
using BBox3d_SSE	= BBox3_<f64, VecSIMD::SSE>;
using BBox3d_Basic	= BBox3_<f64, VecSIMD::None>;

template<Int N, class T, VecSIMD SIMD>
class BBox_ {
	using This = BBox_;
public:
	using Element = T;
	static constexpr Int     elementCount = N;
	static constexpr VecSIMD cpuSIMD = SIMD;

	using Vec = Vec_<N, T, SIMD>;
	union {
		struct { Vec min, max; };
	};
	
	AX_INLINE constexpr BBox_() = default;
	AX_INLINE constexpr BBox_(Tag::All_, const T& v) : min(Tag::All, v), max(Tag::All, v) {}
	AX_INLINE constexpr BBox_(const T& min_, const T& max_) : min(min_), max(max_) {}
	AX_INLINE constexpr bool isValid() { return (max - min).isAllPositive(); }

	constexpr void includePoint(const Vec& pt);

	void getCornerPoints(FixedArray<Vec, Math::pow3(2, N)>& outPoints) const;	
};

template<Int N, class T, VecSIMD SIMD> AX_INLINE constexpr 
void BBox_<N,T,SIMD>::includePoint(const Vec& pt) {
	if (!isValid()) {
		min = pt;
		max = pt;
	} else {
		Math::min_itself(min, pt);
		Math::min_itself(max, pt);
	}
}


} // namespace 