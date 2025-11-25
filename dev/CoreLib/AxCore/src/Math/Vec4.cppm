module;
#include "AxCore-pch.h"
export module AxCore.Vec4;
export import AxCore.Vec3;

export namespace ax {

template<class T, CpuSIMD SIMD>
class Vec_<4, T, SIMD> : public VecBase4_<T, SIMD> {
	using This = Vec_;
	using Base = VecBase4_<T, SIMD>;
public:
	using Data = VecBase4_<T, SIMD>;
	using typename Data::Element;
	static constexpr Int elementCount = Data::elementCount;
	using Data::x;
	using Data::y;
	using Data::z;
	using Data::w;

	using Vec2   = Vec2_< T, SIMD>;
	using Vec3   = Vec3_< T, SIMD>;
	using Vec4   = Vec4_< T, SIMD>;
	using Mat4   = Mat4_< T, SIMD>;
	using Rect2  = Rect2_<T, SIMD>;
	using Quat4  = Quat4_<T, SIMD>;
	
	AX_INLINE explicit	constexpr Vec_() = default;
	AX_INLINE			constexpr Vec_(Tag::All_, const T& v) { setAll(v); }
	AX_INLINE			constexpr Vec_(const T& x_, const T& y_, const T& z_, const T& w_) { set(x_,y_,z_,w_); }
	AX_INLINE			constexpr Vec_(const Vec3& v, T w_) { set(v, w_); }
	AX_INLINE			constexpr Vec_(const Data& t) : Data(t) {}

	AX_INLINE constexpr void setAll(const T& v)											{ x = v;   y = v;   z = v;   w = v;   }
	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_)	{ x = x_;  y = y_;  z = z_;  w = w_;  }
	AX_INLINE constexpr void set(const Vec3& v, T w_)									{ x = v.x; y = v.y; z = v.z; w = w_;  }

	AX_INLINE constexpr void operator=(const Data& t)			{ Base::operator=(t); }

	AX_INLINE static constexpr This s_zero		() { return This(Tag::All, T(0)); }
	AX_INLINE static constexpr This s_one 		() { return This(Tag::All, T(1)); }
	AX_INLINE static constexpr This s_epsilon	() { return This(Tag::All, Math::epsilon<T>); }
	
	AX_INLINE constexpr		  T* data()			{ return &x; }
	AX_INLINE constexpr const T* data() const	{ return &x; }

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }	
	
	AX_INLINE constexpr Vec2	xy() const	{ return Vec2(x,y); }
	AX_INLINE constexpr Vec2	xz() const	{ return Vec2(x,z); }
	AX_INLINE constexpr Vec2	yz() const	{ return Vec2(y,z); }
	AX_INLINE constexpr Vec3	xyz() const	{ return Vec3(x,y,z); }

	AX_INLINE constexpr Vec3	homogenize () const { return Math::safeDiv(*this, w).xyz(); }
	AX_INLINE constexpr Vec3	toVec3() const		{ return homogenize(); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const This& r) const	{ return This(x + r.x, y + r.y, z + r.z, w + r.w); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& r) const	{ return This(x - r.x, y - r.y, z - r.z, w - r.w); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& r) const	{ return This(x * r.x, y * r.y, z * r.z, w * r.w); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& r) const	{ return This(x / r.x, y / r.y, z / r.z, w / r.w); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const T& s) const { return This(x + s, y + s, z + s, w + s); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& s) const { return This(x - s, y - s, z - s, w - s); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& s) const { return This(x * s, y * s, z * s, w * s); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& s) const { return This(x / s, y / s, z / s, w / s); }

	AX_INLINE  This abs() const { return This(Math::abs(x), Math::abs(y), Math::abs(z), Math::abs(w)); }
	AX_INLINE constexpr This operator-() const	{ return This(-x, -y, -z, -w); }
	
	AX_INLINE void operator+=(const T& s)		{ x += s; y += s; z += s; w += s; }
	AX_INLINE void operator-=(const T& s)		{ x -= s; y -= s; z -= s; w -= s; }
	AX_INLINE void operator*=(const T& s)		{ x *= s; y *= s; z *= s; w *= s; }
	AX_INLINE void operator/=(const T& s)		{ x /= s; y /= s; z /= s; w /= s; }

	AX_INLINE void operator+=(const This& r)	{ x += r.x; y += r.y; z += r.z; w += r.w; }
	AX_INLINE void operator-=(const This& r)	{ x -= r.x; y -= r.y; z -= r.z; w -= r.w; }
	AX_INLINE void operator*=(const This& r)	{ x *= r.x; y *= r.y; z *= r.z; w *= r.w; }
	AX_INLINE void operator/=(const This& r)	{ x /= r.x; y /= r.y; z /= r.z; w /= r.w; }

	AX_NODISCARD AX_INLINE bool operator==(const This& r) const	{ return x == r.x && y == r.y && z == r.z && w == r.w; }
	AX_NODISCARD AX_INLINE bool operator!=(const This& r) const	{ return x != r.x || y != r.y || z != r.z || w != r.w; }
	AX_NODISCARD AX_INLINE bool operator< (const This& r) const	{ return x <  r.x && y <  r.y && z <  r.z && w <  r.w; }
	AX_NODISCARD AX_INLINE bool operator> (const This& r) const	{ return x >  r.x && y >  r.y && z >  r.z && w >  r.w; }
	AX_NODISCARD AX_INLINE bool operator<=(const This& r) const	{ return x <= r.x && y <= r.y && z <= r.z && w <= r.w; }
	AX_NODISCARD AX_INLINE bool operator>=(const This& r) const	{ return x >= r.x && y >= r.y && z >= r.z && w >= r.w; }
	
	AX_NODISCARD AX_INLINE bool almostEqual(const This& r, const T& ep = Math::epsilon<T>) const	{ return Math::almostEqual(*this, r, ep); }
	AX_NODISCARD AX_INLINE bool almostZero (const T& ep = Math::epsilon<T>) const					{ return Math::almostZero(ep); }

	AX_NODISCARD AX_INLINE This operator *  (const Mat4& m) const;

	template<class R, CpuSIMD R_SIMD> AX_NODISCARD AX_INLINE constexpr 
	static Vec4 s_cast(const Vec4_<R, R_SIMD>& v)	 { return This( static_cast<T>(v.x), 
																	static_cast<T>(v.y),
																	static_cast<T>(v.z),
																	static_cast<T>(v.w)); }

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const {
		fmt << Fmt("({},{},{},{})", x, y, z, w);
	}
};

//---- Vec4f_SSE

template<> AX_NODISCARD AX_INLINE constexpr void Vec4f_SSE::setAll(const f32& v) {
	if (std::is_constant_evaluated()) {
		x = v; y = v; z = v; w = v;	
	} else {
		_m = _mm_set1_ps(v);
	}
}

template<> AX_NODISCARD AX_INLINE constexpr void Vec4d_SSE::setAll(const f64& v) {
	if (std::is_constant_evaluated()) {
		x = v; y = v; z = v; w = v;	
	} else {
		_m = _mm256_set1_pd(v);
	}
}

template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator+(const Vec4f_SSE& r) const { return Vec4f_SSE(_mm_add_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator-(const Vec4f_SSE& r) const { return Vec4f_SSE(_mm_sub_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator*(const Vec4f_SSE& r) const { return Vec4f_SSE(_mm_mul_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator/(const Vec4f_SSE& r) const { return Vec4f_SSE(_mm_div_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator+(const Vec4d_SSE& r) const { return Vec4d_SSE(_mm256_add_pd(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator-(const Vec4d_SSE& r) const { return Vec4d_SSE(_mm256_sub_pd(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator*(const Vec4d_SSE& r) const { return Vec4d_SSE(_mm256_mul_pd(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator/(const Vec4d_SSE& r) const { return Vec4d_SSE(_mm256_div_pd(_m, r._m)); }

template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator+(const f32& r) const { return Vec4f_SSE(_mm_add_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator-(const f32& r) const { return Vec4f_SSE(_mm_sub_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator*(const f32& r) const { return Vec4f_SSE(_mm_mul_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4f_SSE Vec4f_SSE::operator/(const f32& r) const { return Vec4f_SSE(_mm_div_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator+(const f64& r) const { return Vec4d_SSE(_mm256_add_pd(_m, _mm256_set1_pd(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator-(const f64& r) const { return Vec4d_SSE(_mm256_sub_pd(_m, _mm256_set1_pd(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator*(const f64& r) const { return Vec4d_SSE(_mm256_mul_pd(_m, _mm256_set1_pd(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec4d_SSE Vec4d_SSE::operator/(const f64& r) const { return Vec4d_SSE(_mm256_div_pd(_m, _mm256_set1_pd(r))); }

AX_NODISCARD AX_INLINE constexpr Vec4f_SSE operator+(const f32& s, const Vec4f_SSE& r) { return Vec4f_SSE(_mm_add_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4f_SSE operator-(const f32& s, const Vec4f_SSE& r) { return Vec4f_SSE(_mm_sub_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4f_SSE operator*(const f32& s, const Vec4f_SSE& r) { return Vec4f_SSE(_mm_mul_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4f_SSE operator/(const f32& s, const Vec4f_SSE& r) { return Vec4f_SSE(_mm_div_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4d_SSE operator+(const f64& s, const Vec4d_SSE& r) { return Vec4d_SSE(_mm256_add_pd(_mm256_set1_pd(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4d_SSE operator-(const f64& s, const Vec4d_SSE& r) { return Vec4d_SSE(_mm256_sub_pd(_mm256_set1_pd(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4d_SSE operator*(const f64& s, const Vec4d_SSE& r) { return Vec4d_SSE(_mm256_mul_pd(_mm256_set1_pd(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec4d_SSE operator/(const f64& s, const Vec4d_SSE& r) { return Vec4d_SSE(_mm256_div_pd(_mm256_set1_pd(s), r._m)); }

template<> AX_INLINE void Vec4f_SSE::operator+=(const Vec4f_SSE& r) { _m = _mm_add_ps(_m, r._m); }
template<> AX_INLINE void Vec4f_SSE::operator-=(const Vec4f_SSE& r) { _m = _mm_sub_ps(_m, r._m); }
template<> AX_INLINE void Vec4f_SSE::operator*=(const Vec4f_SSE& r) { _m = _mm_mul_ps(_m, r._m); }
template<> AX_INLINE void Vec4f_SSE::operator/=(const Vec4f_SSE& r) { _m = _mm_div_ps(_m, r._m); }
template<> AX_INLINE void Vec4d_SSE::operator+=(const Vec4d_SSE& r) { _m = _mm256_add_pd(_m, r._m); }
template<> AX_INLINE void Vec4d_SSE::operator-=(const Vec4d_SSE& r) { _m = _mm256_sub_pd(_m, r._m); }
template<> AX_INLINE void Vec4d_SSE::operator*=(const Vec4d_SSE& r) { _m = _mm256_mul_pd(_m, r._m); }
template<> AX_INLINE void Vec4d_SSE::operator/=(const Vec4d_SSE& r) { _m = _mm256_div_pd(_m, r._m); }

template<> AX_INLINE void Vec4f_SSE::operator+=(const float& r) { _m = _mm_add_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec4f_SSE::operator-=(const float& r) { _m = _mm_sub_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec4f_SSE::operator*=(const float& r) { _m = _mm_mul_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec4f_SSE::operator/=(const float& r) { _m = _mm_div_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec4d_SSE::operator+=(const double& r) { _m = _mm256_add_pd(_m, _mm256_set1_pd(r)); }
template<> AX_INLINE void Vec4d_SSE::operator-=(const double& r) { _m = _mm256_sub_pd(_m, _mm256_set1_pd(r)); }
template<> AX_INLINE void Vec4d_SSE::operator*=(const double& r) { _m = _mm256_mul_pd(_m, _mm256_set1_pd(r)); }
template<> AX_INLINE void Vec4d_SSE::operator/=(const double& r) { _m = _mm256_div_pd(_m, _mm256_set1_pd(r)); }

// Check if ALL lanes resulted in TRUE (0xFFFFFFFF)
// Extract a 4-bit mask (one bit per lane) from the result vector R.
// If all 4 bits are set (0b1111 or 0x0F), then all elements are true.
template<> AX_INLINE bool Vec4f_SSE::operator> (const Vec4f_SSE& r) const { auto cmp = _mm_cmp_ps(_m, r._m, _CMP_GT_OQ); return 0x0F == _mm_movemask_ps(cmp); }
template<> AX_INLINE bool Vec4f_SSE::operator< (const Vec4f_SSE& r) const {	auto cmp = _mm_cmp_ps(_m, r._m, _CMP_LT_OQ); return 0x0F == _mm_movemask_ps(cmp); }
template<> AX_INLINE bool Vec4f_SSE::operator>=(const Vec4f_SSE& r) const { auto cmp = _mm_cmp_ps(_m, r._m, _CMP_GE_OQ); return 0x0F == _mm_movemask_ps(cmp); }
template<> AX_INLINE bool Vec4f_SSE::operator<=(const Vec4f_SSE& r) const { auto cmp = _mm_cmp_ps(_m, r._m, _CMP_LE_OQ); return 0x0F == _mm_movemask_ps(cmp); }
template<> AX_INLINE bool Vec4d_SSE::operator> (const Vec4d_SSE& r) const { auto cmp = _mm256_cmp_pd(_m, r._m, _CMP_GT_OQ); return 0x0F == _mm256_movemask_pd(cmp); }
template<> AX_INLINE bool Vec4d_SSE::operator< (const Vec4d_SSE& r) const {	auto cmp = _mm256_cmp_pd(_m, r._m, _CMP_LT_OQ); return 0x0F == _mm256_movemask_pd(cmp); }
template<> AX_INLINE bool Vec4d_SSE::operator>=(const Vec4d_SSE& r) const { auto cmp = _mm256_cmp_pd(_m, r._m, _CMP_GE_OQ); return 0x0F == _mm256_movemask_pd(cmp); }
template<> AX_INLINE bool Vec4d_SSE::operator<=(const Vec4d_SSE& r) const { auto cmp = _mm256_cmp_pd(_m, r._m, _CMP_LE_OQ); return 0x0F == _mm256_movemask_pd(cmp); }

template<> AX_NODISCARD AX_INLINE Vec4f_SSE Vec4f_SSE::abs() const { return Vec4f_SSE(_mm_andnot_ps(_m, _mm_set1_ps(-0.0f))); }
template<> AX_NODISCARD AX_INLINE Vec4d_SSE Vec4d_SSE::abs() const { return Vec4d_SSE(_mm256_andnot_pd(_m, _mm256_set1_pd(-0.0f))); }

} // namespace