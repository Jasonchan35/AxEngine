module;
#include "AxCore-pch.h"
export module AxCore.Vec1;
export import AxCore.VecBase;

export namespace ax {

template<class T, CpuSIMD SIMD>
class Vec_<1, T, SIMD> : public VecBase1_<T, SIMD> {
	using This = Vec_;
public:
	using Data = VecBase1_<T, SIMD>;
	using Data::ElementCount;
	using Data::x;
	using typename Data::Element;

	using Vec2   = Vec2_< T, SIMD>;
	using Vec3   = Vec3_< T, SIMD>;
	using Vec4   = Vec4_< T, SIMD>;
	using Mat4   = Mat4_< T, SIMD>;
	using Quat4  = Quat4_<T, SIMD>;
	using Rect2  = Rect2_<T, SIMD>;

	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const T& x_) : Data(x_) {}
	AX_INLINE constexpr Vec_(const Data& data) : Data(data) {}

	AX_INLINE constexpr void set(const T& v) { x = v; }

	AX_INLINE static constexpr This kZero() { return This(T(0)); }
	AX_INLINE static constexpr This kOne () { return This(T(1)); }

	AX_INLINE constexpr This operator + (T s) const				{ return This(x + s); }
	AX_INLINE constexpr This operator - (T s) const				{ return This(x - s); }
	AX_INLINE constexpr This operator * (T s) const				{ return This(x * s); }
	AX_INLINE constexpr This operator / (T s) const				{ return This(x / s); }

	AX_INLINE constexpr This operator + (const This& r) const	{ return This(x + r.x); }
	AX_INLINE constexpr This operator - (const This& r) const	{ return This(x - r.x); }
	AX_INLINE constexpr This operator * (const This& r) const	{ return This(x * r.x); }
	AX_INLINE constexpr This operator / (const This& r) const	{ return This(x / r.x); }

	AX_INLINE constexpr void operator += (T s)					{ x += s; }
	AX_INLINE constexpr void operator -= (T s)					{ x -= s; }
	AX_INLINE constexpr void operator *= (T s)					{ x *= s; }
	AX_INLINE constexpr void operator /= (T s)					{ x /= s; }

	AX_INLINE constexpr void operator += (const This& r)		{ x += r.x; }
	AX_INLINE constexpr void operator -= (const This& r)		{ x -= r.x; }
	AX_INLINE constexpr void operator *= (const This& r)		{ x *= r.x; }
	AX_INLINE constexpr void operator /= (const This& r)		{ x /= r.x; }

	AX_NODISCARD AX_INLINE constexpr bool operator == (const This& r) const	{ return x == r.x; }
	AX_NODISCARD AX_INLINE constexpr bool operator != (const This& r) const	{ return x != r.x; }

	AX_NODISCARD AX_INLINE constexpr bool almostEqual		(const This& r, const T& ep = Math::epsilon<T>) const	{ return Math::almostEqual(*this, r, ep); }
	AX_NODISCARD AX_INLINE constexpr bool almostEqualZero	(const T& ep = Math::epsilon<T>) const					{ return Math::almostZero(ep); }
};

//-------

} // namespace


