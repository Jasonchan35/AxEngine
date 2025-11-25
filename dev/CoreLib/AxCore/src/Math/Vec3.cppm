module;
#include "AxCore-pch.h"
export module AxCore.Vec3;
export import AxCore.Vec2;

export namespace ax {

template<class T, CpuSIMD SIMD>
class Vec_<3, T, SIMD> : public VecBase3_<T, SIMD> {
	using This = Vec_;
	using Base = VecBase3_<T, SIMD>; 
public:
	using Data = VecBase3_<T, SIMD>;
	using Data::elementCount;
	using Data::x;
	using Data::y;
	using Data::z;
	using typename Data::Element;
	using typename Data::MFixedSpan;
	using typename Data::CFixedSpan;

	using Vec2   = Vec2_< T, SIMD>;
	using Vec3   = Vec3_< T, SIMD>;
	using Vec4   = Vec4_< T, SIMD>;
	using Mat4   = Mat4_< T, SIMD>;
	using Quat4  = Quat4_<T, SIMD>;
	using Rect2  = Rect2_<T, SIMD>;
	
	AX_INLINE explicit	constexpr Vec_() = default;
	AX_INLINE explicit	constexpr Vec_(const T& v) : This(v,v,v) {}
	AX_INLINE			constexpr Vec_(const T& x_, const T& y_, const T& z_) : Data(x_, y_, z_) {}
	AX_INLINE			constexpr Vec_(const Vec2&  v, T z_) : This(v.x, v.y, z_) {}
	AX_INLINE			constexpr Vec_(const Data&  t) : Data(t) {}

	AX_INLINE static constexpr This kZero() { return This(T(0), T(0), T(0)); }
	AX_INLINE static constexpr This kOne () { return This(T(1), T(1), T(1)); }

	// coordinates system  using right-handed system: same as OpenGL/Maya/Houdini
	// and z-axis is backward, so what we call z-buffer, z-order make more sense
	AX_INLINE static constexpr This kForward () { return This (T( 0), T( 0), T(-1)); }
	AX_INLINE static constexpr This kBackward() { return This (T( 0), T( 0), T( 1)); }
	AX_INLINE static constexpr This kUp      () { return This (T( 0), T( 1), T( 0)); }
	AX_INLINE static constexpr This kDown    () { return This (T( 0), T(-1), T( 0)); }
	AX_INLINE static constexpr This kLeft    () { return This (T(-1), T( 0), T( 0)); }
	AX_INLINE static constexpr This kRight   () { return This (T( 1), T( 0), T( 0)); }
	
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(Base::fixedSpan()); }
	
	AX_INLINE constexpr void set(T x_, T y_, T z_)		{ x = x_;  y = y_;  z = z_; }
	AX_INLINE constexpr void set(const Vec2& v, T z_)	{ set(v.x, v.y, z_); }

	AX_INLINE void operator=(const Data& t)				{ set(t.x, t.y, t.z); }

	AX_NODISCARD AX_INLINE constexpr Vec3 xyz() const	{ return Vec3(x, y, z); }
	AX_NODISCARD AX_INLINE constexpr Vec3 xzy() const	{ return Vec3(x, z, y); }
	AX_NODISCARD AX_INLINE constexpr Vec3 yxz() const	{ return Vec3(y, x, z); }
	AX_NODISCARD AX_INLINE constexpr Vec3 yzx() const	{ return Vec3(y, z, x); }
	AX_NODISCARD AX_INLINE constexpr Vec3 zxy() const	{ return Vec3(z, x, y); }
	AX_NODISCARD AX_INLINE constexpr Vec3 zyx() const	{ return Vec3(z, y, x); }

	AX_NODISCARD AX_INLINE constexpr Vec2 xy() const	{ return Vec2(x, y); }
	AX_NODISCARD AX_INLINE constexpr Vec2 xz() const	{ return Vec2(x, z); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yx() const	{ return Vec2(y, x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yz() const	{ return Vec2(y, z); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zx() const	{ return Vec2(z, x); }
	AX_NODISCARD AX_INLINE constexpr Vec2 zy() const	{ return Vec2(z, y); }

	AX_NODISCARD AX_INLINE constexpr Vec4 xyz1() const	{ return Vec4(x, y, z, 1); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const This& r) const	{ return This(x + r.x, y + r.y, z + r.z); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& r) const	{ return This(x - r.x, y - r.y, z - r.z); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& r) const	{ return This(x * r.x, y * r.y, z * r.z); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& r) const	{ return This(x / r.x, y / r.y, z / r.z); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const T& s) const		{ return This(x + s, y + s, z + s); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& s) const		{ return This(x - s, y - s, z - s); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& s) const		{ return This(x * s, y * s, z * s); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& s) const		{ return This(x / s, y / s, z / s); }

	AX_INLINE void operator+=(const T& s)		{ x += s; y += s; z += s; }
	AX_INLINE void operator-=(const T& s)		{ x -= s; y -= s; z -= s; }
	AX_INLINE void operator*=(const T& s)		{ x *= s; y *= s; z *= s; }
	AX_INLINE void operator/=(const T& s)		{ x /= s; y /= s; z /= s; }

	AX_INLINE void operator+=(const This& r)	{ x += r.x; y += r.y; z += r.z; }
	AX_INLINE void operator-=(const This& r)	{ x -= r.x; y -= r.y; z -= r.z; }
	AX_INLINE void operator*=(const This& r)	{ x *= r.x; y *= r.y; z *= r.z; }
	AX_INLINE void operator/=(const This& r)	{ x /= r.x; y /= r.y; z /= r.z; }

	AX_NODISCARD AX_INLINE	constexpr This operator* (const Quat4& q) const;
				 AX_INLINE	constexpr void operator*=(const Quat4& q)				{ *this = *this * q; }

	AX_NODISCARD AX_INLINE	constexpr T	 sqrLen			() const					{ return x * x + y * y + z * z; }
	AX_NODISCARD AX_INLINE	constexpr T	 len			() const					{ return sqrt (sqrLen()); }
	AX_NODISCARD AX_INLINE	constexpr T	 invLen			() const					{ return rsqrt(sqrLen()); }

	AX_NODISCARD AX_INLINE	constexpr T	 distance		(const This &r) const		{ return (*this - r).len(); }
	AX_NODISCARD AX_INLINE	constexpr T	 sqrDistance	(const This &r) const		{ return (*this - r).sqrLen(); }
									 
	AX_NODISCARD AX_INLINE constexpr bool almostEqual	(const This& r, const T& ep = Math::epsilon<T>) const	{ return Math::almostEqual(*this, r, ep); }
	AX_NODISCARD AX_INLINE constexpr bool almostZero	(const T& ep = Math::epsilon<T>) const					{ return Math::almostZero(ep); }
									 
	AX_NODISCARD AX_INLINE	constexpr bool isParallel	(const This& r) const		{ return cross(r).almostEqualZero(); }
									 
	AX_NODISCARD AX_INLINE	constexpr This normal		() const					{ return *this * invLen(); }
									 
	AX_NODISCARD AX_INLINE	constexpr This cross		(const This& v) const		{ return This(y*v.z - z*v.y, z*v.x - x*v.z,x*v.y - y*v.x); }
	AX_NODISCARD AX_INLINE	constexpr T		dot			(const This& v) const		{ return (x*v.x) + (y*v.y) + (z*v.z); }

	AX_NODISCARD AX_INLINE	constexpr bool operator==	(const This& r) const		{ return x == r.x && y == r.y && z == r.z; }
	AX_NODISCARD AX_INLINE	constexpr bool operator!=	(const This& r) const		{ return x != r.x || y != r.y || z != r.z; }

	template<class R, CpuSIMD R_SIMD> AX_INLINE constexpr 
	static Vec3 s_cast(const Vec3_<R, R_SIMD>& v)	 { return This( static_cast<T>(v.x), 
																	static_cast<T>(v.y),
																	static_cast<T>(v.z)); }

	static constexpr This s_unprojectInvMatrix(const Vec3& screenPos, const Mat4& invMat, const Rect2& viewport);
private:
};

//---- Vec3f_SSE
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator+(const Vec3f_SSE& r) const { return Vec3f_SSE(_mm_add_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator-(const Vec3f_SSE& r) const { return Vec3f_SSE(_mm_sub_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator*(const Vec3f_SSE& r) const { return Vec3f_SSE(_mm_mul_ps(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator/(const Vec3f_SSE& r) const { return Vec3f_SSE(_mm_div_ps(_m, r._m)); }

template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator+(const f32& r) const { return Vec3f_SSE(_mm_add_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator-(const f32& r) const { return Vec3f_SSE(_mm_sub_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator*(const f32& r) const { return Vec3f_SSE(_mm_mul_ps(_m, _mm_set1_ps(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3f_SSE Vec3f_SSE::operator/(const f32& r) const { return Vec3f_SSE(_mm_div_ps(_m, _mm_set1_ps(r))); }

template<> AX_INLINE void Vec3f_SSE::operator+=(const Vec3f_SSE& r) { _m = _mm_add_ps(_m, r._m); }
template<> AX_INLINE void Vec3f_SSE::operator-=(const Vec3f_SSE& r) { _m = _mm_sub_ps(_m, r._m); }
template<> AX_INLINE void Vec3f_SSE::operator*=(const Vec3f_SSE& r) { _m = _mm_mul_ps(_m, r._m); }
template<> AX_INLINE void Vec3f_SSE::operator/=(const Vec3f_SSE& r) { _m = _mm_div_ps(_m, r._m); }

template<> AX_INLINE void Vec3f_SSE::operator+=(const f32& r) { _m = _mm_add_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec3f_SSE::operator-=(const f32& r) { _m = _mm_sub_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec3f_SSE::operator*=(const f32& r) { _m = _mm_mul_ps(_m, _mm_set1_ps(r)); }
template<> AX_INLINE void Vec3f_SSE::operator/=(const f32& r) { _m = _mm_div_ps(_m, _mm_set1_ps(r)); }

AX_NODISCARD AX_INLINE constexpr Vec3f_SSE operator+(const f32& s, const Vec3f_SSE& r) { return Vec3f_SSE(_mm_add_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec3f_SSE operator-(const f32& s, const Vec3f_SSE& r) { return Vec3f_SSE(_mm_sub_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec3f_SSE operator*(const f32& s, const Vec3f_SSE& r) { return Vec3f_SSE(_mm_mul_ps(_mm_set1_ps(s), r._m)); }
AX_NODISCARD AX_INLINE constexpr Vec3f_SSE operator/(const f32& s, const Vec3f_SSE& r) { return Vec3f_SSE(_mm_div_ps(_mm_set1_ps(s), r._m)); }

//---- Vec3d_SSE
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator+(const Vec3d_SSE& r) const { return Vec3d_SSE(_mm256_add_pd(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator-(const Vec3d_SSE& r) const { return Vec3d_SSE(_mm256_sub_pd(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator*(const Vec3d_SSE& r) const { return Vec3d_SSE(_mm256_mul_pd(_m, r._m)); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator/(const Vec3d_SSE& r) const { return Vec3d_SSE(_mm256_div_pd(_m, r._m)); }

template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator+(const f64& r) const { return Vec3d_SSE(_mm256_add_pd(_m, _mm256_set1_pd(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator-(const f64& r) const { return Vec3d_SSE(_mm256_sub_pd(_m, _mm256_set1_pd(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator*(const f64& r) const { return Vec3d_SSE(_mm256_mul_pd(_m, _mm256_set1_pd(r))); }
template<> AX_NODISCARD AX_INLINE constexpr Vec3d_SSE Vec3d_SSE::operator/(const f64& r) const { return Vec3d_SSE(_mm256_div_pd(_m, _mm256_set1_pd(r))); }

template<> AX_INLINE void Vec3d_SSE::operator+=(const Vec3d_SSE& r) { _m = _mm256_add_pd(_m, r._m); }
template<> AX_INLINE void Vec3d_SSE::operator-=(const Vec3d_SSE& r) { _m = _mm256_sub_pd(_m, r._m); }
template<> AX_INLINE void Vec3d_SSE::operator*=(const Vec3d_SSE& r) { _m = _mm256_mul_pd(_m, r._m); }
template<> AX_INLINE void Vec3d_SSE::operator/=(const Vec3d_SSE& r) { _m = _mm256_div_pd(_m, r._m); }

template<> AX_INLINE void Vec3d_SSE::operator+=(const f64& r) { _m = _mm256_add_pd(_m, _mm256_set1_pd(r)); }
template<> AX_INLINE void Vec3d_SSE::operator-=(const f64& r) { _m = _mm256_sub_pd(_m, _mm256_set1_pd(r)); }
template<> AX_INLINE void Vec3d_SSE::operator*=(const f64& r) { _m = _mm256_mul_pd(_m, _mm256_set1_pd(r)); }
template<> AX_INLINE void Vec3d_SSE::operator/=(const f64& r) { _m = _mm256_div_pd(_m, _mm256_set1_pd(r)); }

AX_NODISCARD AX_INLINE Vec3d_SSE operator+(const f64& s, const Vec3d_SSE& r) { return Vec3d_SSE(_mm256_add_pd(_mm256_set1_pd(s), r._m)); }
AX_NODISCARD AX_INLINE Vec3d_SSE operator-(const f64& s, const Vec3d_SSE& r) { return Vec3d_SSE(_mm256_sub_pd(_mm256_set1_pd(s), r._m)); }
AX_NODISCARD AX_INLINE Vec3d_SSE operator*(const f64& s, const Vec3d_SSE& r) { return Vec3d_SSE(_mm256_mul_pd(_mm256_set1_pd(s), r._m)); }
AX_NODISCARD AX_INLINE Vec3d_SSE operator/(const f64& s, const Vec3d_SSE& r) { return Vec3d_SSE(_mm256_div_pd(_mm256_set1_pd(s), r._m)); }

} // namespace