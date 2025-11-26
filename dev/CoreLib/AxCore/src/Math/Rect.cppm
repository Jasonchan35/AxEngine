module;
#include "AxCore-pch.h"
export module AxCore.Rect;
export import AxCore.Margin;

export namespace ax {

template<Int N, class T, CpuSIMD SIMD> class Rect_Storage_;
template<Int N, class T, CpuSIMD SIMD> class Rect_;
template<Int N, class T, CpuSIMD SIMD> using RectBase_ = NumSIMD_<N,  Rect_<N, T, SIMD>, Rect_Storage_<N, T, SIMD> >;


template<class T, CpuSIMD SIMD>
class Rect_Storage_<4, T, SIMD> {
	using REG = CpuSIMD_REG_<4, T, SIMD>;
	using MM  = typename REG::Type;
	using Vec2 = Vec2_<T, SIMD>;
public:
	using Element = T;
	static constexpr Int elementCount = 4;
	static constexpr CpuSIMD cpuSIMD = SIMD;
	union {
		MM _m;
		T _data[elementCount];
		struct { T _e0, _e1, _e2, _e3; };
		struct { T x, y, w, h; };
		struct { Vec2 pos, extents; };
	};
	
	AX_NODISCARD AX_INLINE			constexpr Rect_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Rect_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Rect_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

template<class T, CpuSIMD SIMD>
class Rect_<4, T, SIMD> : public RectBase_<4, T, SIMD> {
	using Base = RectBase_<4, T, SIMD>;
	using This = Rect_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);

	using Storage::x;
	using Storage::y;
	using Storage::w;
	using Storage::h;
	using Storage::pos;
	using Storage::extents;
	using Vec2 = Vec2_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
	using Margin4 = Margin_<4, T, SIMD>;
	
	AX_INLINE constexpr Rect_() = default;
	AX_INLINE constexpr Rect_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Rect_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Rect_(const T& x_,   const T& y_, const T& w_, const T& h_) : Base(x_, y_, w_, h_) {}
	AX_INLINE constexpr Rect_(const T& pos_, const T& extents_) : Base(pos_.x, pos_.y, extents_.x, extents_.y) {}
	
	constexpr Vec2		center		() const { return pos + extents / 2; }
	constexpr T			xCenter		() const { return x + w / 2; }
	constexpr T			yCenter		() const { return y + h / 2; }

	constexpr T			xMin		() const { return x; }
	constexpr T			yMin		() const { return y; }
	constexpr T			xMax		() const { return x + w; }
	constexpr T			yMax		() const { return y + h; }

	constexpr Vec2		xMin_yMin	() const { return Vec2(xMin(), yMin()); }
	constexpr Vec2		xMax_yMin	() const { return Vec2(xMax(), yMin()); }

	constexpr Vec2		xMin_yMax	() const { return Vec2(xMin(), yMax()); }
	constexpr Vec2		xMax_yMax	() const { return Vec2(xMax(), yMax()); }

	constexpr Vec2		xMin_yCenter() const { return Vec2(xMin(), yCenter()); }
	constexpr Vec2		xMax_yCenter() const { return Vec2(xMax(), yCenter()); }

	constexpr Vec2		xCenter_yMin() const { return Vec2(xCenter(), yMin()); }
	constexpr Vec2		xCenter_yMax() const { return Vec2(xCenter(), yMax()); }

	constexpr Vec2		topLeft		() const { return xMin_yMin(); }
	constexpr Vec2		topRight	() const { return xMax_yMin(); }
	constexpr Vec2		bottomLeft	() const { return xMin_yMax(); }
	constexpr Vec2		bottomRight	() const { return xMax_yMax(); }

	constexpr void		set_xMin		(T v)	{ auto m = xMax(); x = v; set_xMax(m); }
	constexpr void		set_xMax		(T v)	{ w = v - x; }
	constexpr void		set_yMin		(T v)	{ auto m = yMax(); y = v; set_yMax(m); }
	constexpr void		set_yMax		(T v)	{ h = v - y; }

	constexpr void		set_xMin_yMin	(const Vec2& v) { set_xMin(v.x); set_yMin(v.y); }
	constexpr void		set_xMax_yMin	(const Vec2& v) { set_xMax(v.x); set_yMin(v.y); }
	constexpr void		set_xMin_yMax	(const Vec2& v) { set_xMin(v.x); set_yMax(v.y); }
	constexpr void		set_xMax_yMax	(const Vec2& v) { set_xMax(v.x); set_yMax(v.y); }


	AX_NODISCARD constexpr This		expand		(const Margin4& m) const { return This(x - m.left, y - m.top, w + m.left + m.right, h + m.top + m.bottom ); }
	AX_NODISCARD constexpr This		expand		(T x_, T y_) const	{ return expand(Margin2(x_,y_)); }
	AX_NODISCARD constexpr This		expand		(T v) const			{ return expand(Margin2(v)); }

	AX_NODISCARD constexpr This		expandTop	(T v) const			{ return expand({v,0,0,0}); }
	AX_NODISCARD constexpr This		expandRight	(T v) const			{ return expand({0,v,0,0}); }
	AX_NODISCARD constexpr This		expandBottom(T v) const			{ return expand({0,0,v,0}); }
	AX_NODISCARD constexpr This		expandLeft	(T v) const			{ return expand({0,0,0,v}); }

	AX_NODISCARD constexpr This		trimTop		(T v) const { v = min(v, h); This o(x  , y+v, w  , h-v); return o; }
	AX_NODISCARD constexpr This		trimRight	(T v) const { v = min(v, w); This o(x  , y  , w-v, h  ); return o; }
	AX_NODISCARD constexpr This		trimBottom	(T v) const { v = min(v, h); This o(x  , y  , w  , h-v); return o; }
	AX_NODISCARD constexpr This		trimLeft	(T v) const { v = min(v, w); This o(x+v, y  , w-v, h  ); return o; }

	AX_NODISCARD constexpr This		flipXWithinContainer	(T containerWidth ) const { return This(containerWidth - xMax(),  y, w, h); }
	AX_NODISCARD constexpr This		flipYWithinContainer	(T containerHeight) const { return This(x, containerHeight - yMax(), w, h); }

	AX_NODISCARD constexpr T		area		() const { return w * h; }
	AX_NODISCARD constexpr T		perimeter	() const { return w + w + h + h; }

	AX_NODISCARD constexpr This		offset		(const Vec2& v) const				{ return Rect2_(pos + v, extents); }

	AX_NODISCARD constexpr This		operator+	(const Vec2& v) const				{ return offset( v); }
	AX_NODISCARD constexpr This		operator-	(const Vec2& v) const				{ return offset(-v); }

	AX_NODISCARD constexpr Margin4	diff		(const This& inner) const;
	AX_NODISCARD constexpr Margin4	operator-	(const This& inner) const			{ return diff(inner); }

	AX_NODISCARD constexpr This		operator+	(const Margin4& m) const			{ return expand( m); }
	AX_NODISCARD constexpr This		operator-	(const Margin4& m) const			{ return expand(-m); }

				 constexpr void		operator+=	(const Margin4& m)					{ *this = expand( m); }
				 constexpr void		operator-=	(const Margin4& m)					{ *this = expand(-m); }

	AX_NODISCARD constexpr This		operator*	(T s) const							{ return This(x * s,   y * s,   w * s,   h * s); }
	AX_NODISCARD constexpr This		operator/	(T s) const							{ return This(x / s,   y / s,   w / s,   h / s); }

	AX_NODISCARD constexpr This		operator*	(const Vec2& s) const				{ return This(x * s.x, y * s.y, w * s.x, h * s.y); }
	AX_NODISCARD constexpr This		operator/	(const Vec2& s) const				{ return This(x / s.x, y / s.y, w / s.x, h / s.y); }

				 constexpr void		operator*=	(T s)								{ x *= s;   y *= s;   w *= s;   h *= s; }
				 constexpr void		operator/=	(T s)								{ x /= s;   y /= s;   w /= s;   h /= s; }

				 constexpr void		operator*=	(const Vec2& s)						{ x *= s.x; y *= s.y; w *= s.x; h *= s.y; }
				 constexpr void		operator/=	(const Vec2& s)						{ x /= s.x; y /= s.y; w /= s.x; h /= s.y; }

				 constexpr bool		operator==	(const This& r) const				{ return pos == r.pos && extents == r.size; }
				 constexpr bool		operator!=	(const This& r) const				{ return pos != r.pos || extents != r.size; }

				 constexpr bool		isValid			() const						{ return w > 0 && h >0; }
				 constexpr This		unionWith		(const This& r) const;
				 constexpr This		intersects		(const This& r) const;
				 constexpr bool		isIntersected	(const This& r) const;
	
	AX_NODISCARD constexpr bool		containsPoint	(const Vec2& v) const	{ return v.x >= xMin() && v.x <= xMax() && v.y >= yMin() && v.y <= yMax(); }

	AX_NODISCARD constexpr Vec4		toVec4() const { return Vec4(x,y,w,h); }

	AX_NODISCARD constexpr This		nonNegativeSize() const {
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
		o.left = static_cast<LONG>(xMin());	o.right  = static_cast<LONG>(xMax());
		o.top  = static_cast<LONG>(yMin());	o.bottom = static_cast<LONG>(yMax());
		return o;
	}
#endif	
};



} // namespace

