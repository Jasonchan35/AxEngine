module;

export module AxCore.LinearMath:BBox; // Bounding Box
export import :Vec;
export import AxCore.Array;
export import AxCore.Math;

export namespace ax {

template<Int N, class T, VecSimd SIMD> class BBox_;
template<class T, VecSimd SIMD = VecSimd_Default> using BBox2_ = BBox_<2, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using BBox3_ = BBox_<3, T, SIMD>;

using BBox2h		= BBox2_<f16>;
using BBox2h_SSE	= BBox2_<f16, VecSimd::SSE>;
using BBox2h_Basic	= BBox2_<f16, VecSimd::Basic>;
using BBox2f		= BBox2_<f32>;
using BBox2f_SSE	= BBox2_<f32, VecSimd::SSE>;
using BBox2f_Basic	= BBox2_<f32, VecSimd::Basic>;
using BBox2d		= BBox2_<f64>;
using BBox2d_SSE	= BBox2_<f64, VecSimd::SSE>;
using BBox2d_Basic	= BBox2_<f64, VecSimd::Basic>;

using BBox3h		= BBox3_<f16>;
using BBox3h_SSE	= BBox3_<f16, VecSimd::SSE>;
using BBox3h_Basic	= BBox3_<f16, VecSimd::Basic>;
using BBox3f		= BBox3_<f32>;
using BBox3f_SSE	= BBox3_<f32, VecSimd::SSE>;
using BBox3f_Basic	= BBox3_<f32, VecSimd::Basic>;
using BBox3d		= BBox3_<f64>;
using BBox3d_SSE	= BBox3_<f64, VecSimd::SSE>;
using BBox3d_Basic	= BBox3_<f64, VecSimd::Basic>;

template<Int N, class T, VecSimd SIMD>
class BBox_ {
	using This = BBox_;
public:
	using Element = T;
	static constexpr Int     kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Vec = Vec_<N, T, SIMD>;
	union {
		struct { Vec min, max; };
	};
	
	AX_INLINE constexpr BBox_() = default;
//	AX_INLINE constexpr BBox_(Tag::All_, const T& v) : min(Tag::All, v), max(Tag::All, v) {}
	AX_INLINE constexpr BBox_(const Vec& min_, const Vec& max_) : min(min_), max(max_) {}
	AX_INLINE constexpr bool isValid() const { return !(max - min).hasAnyNegative(); }

	AX_INLINE static constexpr This s_all (const T& v)	{ return This(Vec::s_all(v), Vec::s_all(v)); }
	AX_INLINE static constexpr This s_zero()			{ return This(Vec::s_zero(), Vec::s_zero()); }
	AX_INLINE static constexpr This s_empty()			{ return This(Vec::s_all(1), Vec::s_all(-1)); }
	
	constexpr void includePoint(const Vec& pt);

	constexpr void getCornerPoints(FixedArray<Vec, Math::pow_<N>(2)>& outPoints) const;
};

template <Int N, class T, VecSimd SIMD> constexpr
void BBox_<N, T, SIMD>::getCornerPoints(FixedArray<Vec, Math::pow_<N>(2)>& outPoints) const {
	auto& a = min;
	auto& b = max;

	if constexpr (N == 1) {
		outPoints[0] = Vec(a.x);
		outPoints[1] = Vec(b.x);
		
	} else if constexpr (N == 2) {
		outPoints[0] = Vec(a.x, a.y);
		outPoints[1] = Vec(b.x, a.y);
		outPoints[2] = Vec(b.x, b.y);
		outPoints[3] = Vec(a.x, b.y);
		
	} else if constexpr (N == 3) {
		outPoints[0] = Vec(a.x, a.y, a.z);
		outPoints[1] = Vec(b.x, a.y, a.z);
		outPoints[2] = Vec(b.x, b.y, a.z);
		outPoints[3] = Vec(a.x, b.y, a.z);

		outPoints[4] = Vec(a.x, a.y, b.z);
		outPoints[5] = Vec(b.x, a.y, b.z);
		outPoints[6] = Vec(b.x, b.y, b.z);
		outPoints[7] = Vec(a.x, b.y, b.z);
	} else {
		static_assert(false);
	}
}

template<Int N, class T, VecSimd SIMD> AX_INLINE constexpr 
void BBox_<N,T,SIMD>::includePoint(const Vec& pt) {
	if (!isValid()) {
		min = pt;
		max = pt;
	} else {
		Math::min_itself(min, pt);
		Math::max_itself(max, pt);
	}
}


} // namespace 