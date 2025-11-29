module;
#include "AxCore-pch.h"
export module AxCore.Rect;
export import AxCore.Margin;

export namespace ax {

template<Int N, class T, VecSIMD SIMD> class Box_;
template<class T, VecSIMD SIMD = CpuSIMD_Default> using Box2_ = Box_<2, T, SIMD>;

using Box2h			= Box2_<f16>;
using Box2h_SSE		= Box2_<f16, VecSIMD::SSE>;
using Box2h_Basic	= Box2_<f16, VecSIMD::None>;
using Box2f			= Box2_<f32>;
using Box2f_SSE		= Box2_<f32, VecSIMD::SSE>;
using Box2f_Basic	= Box2_<f32, VecSIMD::None>;
using Box2d			= Box2_<f64>;
using Box2d_SSE		= Box2_<f64, VecSIMD::SSE>;
using Box2d_Basic	= Box2_<f64, VecSIMD::None>;

template<class T, VecSIMD SIMD>
class Box_<2, T, SIMD> {
	static constexpr Int N = 2;
	using This = Box_;
public:
	using Element = T;
	static constexpr Int     elementCount = N;
	static constexpr VecSIMD cpuSIMD = SIMD;
	static_assert(SIMD == cpuSIMD);

	using Vec2    = Vec2_<T, SIMD>;
	using Vec4    = Vec4_<T, SIMD>;
	using Margin4 = Margin_<4, T, SIMD>;
	using Range   = Range_<T>;

	using SIMD_Data = VecSIMD_Data_<N * 2,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { Vec2 pos, extents; };
		struct { T x, y, w, h; };
	};
	
	AX_INLINE constexpr Box_() = default;
	AX_INLINE constexpr Box_(Tag::All_, const T& v) : _simd(Tag::All, v) {}
	AX_INLINE constexpr Box_(const T& x_,   const T& y_, const T& w_, const T& h_) : _simd(x_, y_, w_, h_) {}
	AX_INLINE constexpr Box_(const T& pos_, const T& extents_) : _simd(pos_.x, pos_.y, extents_.x, extents_.y) {}

	AX_NODISCARD static constexpr This s_zero() { return _simd.s_zero(); } 

	template<VecSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }

	
	AX_NODISCARD constexpr Vec2		center			() const { return pos + extents / 2; }
	AX_NODISCARD constexpr T		x_center		() const { return x + w / 2; }
	AX_NODISCARD constexpr T		y_center		() const { return y + h / 2; }
	AX_NODISCARD constexpr T		x_min			() const { return x; }
	AX_NODISCARD constexpr T		y_min			() const { return y; }
	AX_NODISCARD constexpr T		x_max			() const { return x + w; }
	AX_NODISCARD constexpr T		y_max			() const { return y + h; }
	AX_NODISCARD constexpr Range	x_range			() const { return Range(x, w); }
	AX_NODISCARD constexpr Range	y_range			() const { return Range(y, h); }

	AX_NODISCARD constexpr Vec2		x_min_y_min		() const { return Vec2(x_min(),    y_min()); }
	AX_NODISCARD constexpr Vec2		x_max_y_min		() const { return Vec2(x_max(),    y_min()); }
	AX_NODISCARD constexpr Vec2		x_min_y_max		() const { return Vec2(x_min(),    y_max()); }
	AX_NODISCARD constexpr Vec2		x_max_y_max		() const { return Vec2(x_max(),    y_max()); }
	AX_NODISCARD constexpr Vec2		x_min_y_center	() const { return Vec2(x_min(),    y_center()); }
	AX_NODISCARD constexpr Vec2		x_max_y_center	() const { return Vec2(x_max(),    y_center()); }
	AX_NODISCARD constexpr Vec2		x_center_y_min	() const { return Vec2(x_center(), y_min()); }
	AX_NODISCARD constexpr Vec2		x_center_y_max	() const { return Vec2(x_center(), y_max()); }

	AX_NODISCARD constexpr Vec2		top_left		() const { return x_min_y_min(); }
	AX_NODISCARD constexpr Vec2		top_right		() const { return x_max_y_min(); }
	AX_NODISCARD constexpr Vec2		bottom_left		() const { return x_min_y_max(); }
	AX_NODISCARD constexpr Vec2		bottom_right	() const { return x_max_y_max(); }

	constexpr void	set_x_min		(T v)	{ auto m = x_max(); x = v; set_x_max(m); }
	constexpr void	set_x_max		(T v)	{ w = v - x; }
	constexpr void	set_y_min		(T v)	{ auto m = y_max(); y = v; set_y_max(m); }
	constexpr void	set_y_max		(T v)	{ h = v - y; }
	constexpr void	set_x_min_y_min	(const Vec2& v) { set_x_min(v.x); set_y_min(v.y); }
	constexpr void	set_x_max_y_min	(const Vec2& v) { set_x_max(v.x); set_y_min(v.y); }
	constexpr void	set_x_min_y_max	(const Vec2& v) { set_x_min(v.x); set_y_max(v.y); }
	constexpr void	set_x_max_y_max	(const Vec2& v) { set_x_max(v.x); set_y_max(v.y); }
	
	AX_NODISCARD constexpr This		expand			(const Margin4& m) const { return This(x - m.left, y - m.top, w + m.left + m.right, h + m.top + m.bottom ); }
	AX_NODISCARD constexpr This		expand			(T x_, T y_) const	{ return expand(Margin2(x_,y_)); }
	AX_NODISCARD constexpr This		expand			(T v) const			{ return expand(Margin2(v)); }

	AX_NODISCARD constexpr This		expand_top		(T v) const		{ return expand({v,0,0,0}); }
	AX_NODISCARD constexpr This		expand_right	(T v) const		{ return expand({0,v,0,0}); }
	AX_NODISCARD constexpr This		expand_bottom	(T v) const		{ return expand({0,0,v,0}); }
	AX_NODISCARD constexpr This		expand_left		(T v) const		{ return expand({0,0,0,v}); }

	AX_NODISCARD constexpr This		trim_top		(T v) const { v = min(v, h); This o(x  , y+v, w  , h-v); return o; }
	AX_NODISCARD constexpr This		trim_right		(T v) const { v = min(v, w); This o(x  , y  , w-v, h  ); return o; }
	AX_NODISCARD constexpr This		trim_bottom		(T v) const { v = min(v, h); This o(x  , y  , w  , h-v); return o; }
	AX_NODISCARD constexpr This		trim_left		(T v) const { v = min(v, w); This o(x+v, y  , w-v, h  ); return o; }

	AX_NODISCARD constexpr This		flipInContainerX	(T containerWidth ) const { return This(containerWidth - x_max(),  y, w, h); }
	AX_NODISCARD constexpr This		flipInContainerY	(T containerHeight) const { return This(x, containerHeight - y_max(), w, h); }

	AX_NODISCARD constexpr T		area		() const { return w * h; }
	AX_NODISCARD constexpr T		perimeter	() const { return w + w + h + h; }

	AX_NODISCARD constexpr This		operator+	(const Vec2& v) const				{ return Box2_(pos + v, extents); }
	AX_NODISCARD constexpr This		operator-	(const Vec2& v) const				{ return Box2_(pos - v, extents); }
	
	AX_NODISCARD constexpr This		operator+	(const Margin4& m) const			{ return expand( m); }
	AX_NODISCARD constexpr This		operator-	(const Margin4& m) const			{ return expand(-m); }

				 constexpr void		operator+=	(const Margin4& m)					{ *this = expand( m); }
				 constexpr void		operator-=	(const Margin4& m)					{ *this = expand(-m); }
	
	AX_NODISCARD constexpr This		operator*	(const Vec2& s) const				{ return _simd * s._simd; }
	AX_NODISCARD constexpr This		operator/	(const Vec2& s) const				{ return _simd / s._simd; }

				 constexpr void		operator*=	(const Vec2& s)						{ _simd *= s._simd; }
				 constexpr void		operator/=	(const Vec2& s)						{ _simd /= s._simd; }

				 constexpr bool		isValid			() const						{ return w > 0 && h >0; }


	AX_NODISCARD constexpr Margin4	marginTo	(const This& inner) const {
		return Margin4(inner.y_min() - y_min(), x_max() - inner.x_max(), y_max() - inner.y_max(), inner.x_min() - x_min());
	}
	
	AX_NODISCARD constexpr This		unionTo	(const This& r) const {
		if (!r.isValid()) return *this;
		if (!isValid()) return r;
		Vec2 top_left	 (min(x_min(), r.x_min()), min(x_min(), r.y_min()));
		Vec2 bottom_right(max(x_max(), r.x_max()), max(x_max(), r.y_max()));
		return This(top_left, bottom_right - top_left);
	}

	AX_NODISCARD constexpr This		intersects	(const This& r) const {
		if (!isIntersected(r)) return s_zero();
		Vec2 top_left	 (max(x_min(), r.x_min()), max(y_min(), r.y_min()));
		Vec2 bottom_right(min(x_max(), r.x_max()), min(y_max(), r.y_max()));
		return This(top_left, bottom_right - top_left);
	}
	
	AX_NODISCARD  constexpr bool	isIntersected	(const This& r) const {
		if (!isValid() || !r.isValid()) return false;
		if (r.x > x_max() || x > r.x_max()) return false;
		if (r.y > y_max() || y > r.y_max()) return false;
		return true;
	 }
	
	AX_NODISCARD constexpr bool		isInside		(const Vec2& v) const	{ return v.x >= x_min() && v.x <= x_max() && v.y >= y_min() && v.y <= y_max(); }

	AX_NODISCARD constexpr Vec4		toVec4() const { return Vec4(x,y,w,h); }

	AX_NODISCARD constexpr This		nonNegExtents() const {
		return This(w<0 ? x + w : x,
					h<0 ? y + h : y,
					abs(w), abs(h));
	}

#if AX_OS_WINDOWS
	AX_NODISCARD AX_INLINE static constexpr This s_from(const ::RECT& r) {
		return This(static_cast<T>(r.left),
					static_cast<T>(r.top),
					static_cast<T>(r.right  - r.left),
					static_cast<T>(r.bottom - r.top));
	}

	AX_NODISCARD AX_INLINE constexpr RECT to_RECT() const {
		RECT o;
		o.left = static_cast<LONG>(x_min());	o.right  = static_cast<LONG>(x_max());
		o.top  = static_cast<LONG>(y_min());	o.bottom = static_cast<LONG>(y_max());
		return o;
	}
#endif	
};

} // namespace

