module;
#include "AxCore-pch.h"
export module AxCore.Vec2;
export import AxCore.Vec1;

export namespace ax {

template<class T, VecSIMD SIMD>
class Vec_<2, T, SIMD> : public VecBase2_<T, SIMD> {
	using This = Vec_;
	using Base = VecBase2_<T, SIMD>;
public:
	using Data = VecBase2_<T, SIMD>;
	using Data::kElementCount;
	using Data::x;
	using Data::y;
	using typename Data::Element;

	using Vec2   = Vec2_< T, SIMD>;
	using Vec3   = Vec3_< T, SIMD>;
	using Vec4   = Vec4_< T, SIMD>;
	using Mat4   = Mat4_< T, SIMD>;
	using Quat4  = Quat4_<T, SIMD>;
	using Rect2  = Rect2_<T, SIMD>;

	AX_INLINE explicit	constexpr Vec_() = default;
	AX_INLINE explicit	constexpr Vec_(const T& v) : Data(v, v) {}
	AX_INLINE 			constexpr Vec_(const T& x_, const T& y_) : Data(x_, y_) {}
	AX_INLINE			constexpr Vec_(const Data& data) : Data(data) {}

	AX_INLINE constexpr void set(T x_, T y_) { x = x_;  y = y_; }

	AX_INLINE static constexpr This kZero() { return This(T(0), T(0)); }
	AX_INLINE static constexpr This kOne () { return This(T(1), T(1)); }

	AX_NODISCARD AX_INLINE constexpr This yx() const { return Vec2(y,x); }

	AX_NODISCARD AX_INLINE constexpr Vec3 xy0() const { return Vec3(x,y,0); }
	AX_NODISCARD AX_INLINE constexpr Vec3 x0y() const { return Vec3(x,0,y); }

	AX_NODISCARD AX_INLINE constexpr Vec4 xy01() const { return Vec4(x,y,0,1); }

	AX_INLINE constexpr This operator + (T s) const				{ return This(x + s, y + s); }
	AX_INLINE constexpr This operator - (T s) const				{ return This(x - s, y - s); }
	AX_INLINE constexpr This operator * (T s) const				{ return This(x * s, y * s); }
	AX_INLINE constexpr This operator / (T s) const				{ return This(x / s, y / s); }

	AX_INLINE constexpr This operator + (const This& r) const	{ return This(x + r.x, y + r.y); }
	AX_INLINE constexpr This operator - (const This& r) const	{ return This(x - r.x, y - r.y); }
	AX_INLINE constexpr This operator * (const This& r) const	{ return This(x * r.x, y * r.y); }
	AX_INLINE constexpr This operator / (const This& r) const	{ return This(x / r.x, y / r.y); }

	AX_INLINE constexpr void operator += (T s)					{ x += s; y += s; }
	AX_INLINE constexpr void operator -= (T s)					{ x -= s; y -= s; }
	AX_INLINE constexpr void operator *= (T s)					{ x *= s; y *= s; }
	AX_INLINE constexpr void operator /= (T s)					{ x /= s; y /= s; }

	AX_INLINE constexpr void operator += (const This& r)		{ x += r.x; y += r.y; }
	AX_INLINE constexpr void operator -= (const This& r)		{ x -= r.x; y -= r.y; }
	AX_INLINE constexpr void operator *= (const This& r)		{ x *= r.x; y *= r.y; }
	AX_INLINE constexpr void operator /= (const This& r)		{ x /= r.x; y /= r.y; }

	AX_NODISCARD AX_INLINE constexpr bool operator == (const This& r) const	{ return x == r.x && y == r.y; }
	AX_NODISCARD AX_INLINE constexpr bool operator != (const This& r) const	{ return x != r.x || y != r.y; }

	AX_NODISCARD AX_INLINE constexpr T	sqrLen			() const	{ return x * x + y * y; }
	AX_NODISCARD AX_INLINE constexpr T	len				() const	{ return sqrt(sqrLen()); }
	AX_NODISCARD AX_INLINE constexpr T	invLen			() const	{ return rsqrt(sqrLen()); }

	AX_NODISCARD AX_INLINE constexpr T	distance		(const Vec2 &r) const	{ return (*this - r).len();    }
	AX_NODISCARD AX_INLINE constexpr T	sqrDistance		(const Vec2 &r) const	{ return (*this - r).sqrLen(); }

	AX_NODISCARD AX_INLINE constexpr This normal		() const				{ return *this * invLen(); }

	AX_NODISCARD AX_INLINE constexpr bool almostEqual	(const This& r, const T& ep = Math::epsilon<T>) const	{ return Math::almostEqual(*this, r, ep); }
	AX_NODISCARD AX_INLINE constexpr bool almostZero	(const T& ep = Math::epsilon<T>) const					{ return Math::almostZero(ep); }

	template<class R, VecSIMD R_SIMD>
	static This s_cast(const Vec2_<R, R_SIMD>& r) { return Vec2(static_cast<T>(r.x), static_cast<T>(r.y)); }

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

} // namespace ax

