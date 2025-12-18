module;
export module AxCore.Rect;
export import AxCore.Margin;

export namespace ax {

template<class T, VecSimd SIMD = VecSimd_Default> class Rect2_;
using Rect2h = Rect2_<f16>;
using Rect2f = Rect2_<f32>;
using Rect2d = Rect2_<f64>;
using Rect2i = Rect2_<Int>;
using Rect2s = Rect2_<i16>;

template<class T, VecSimd SIMD>
class Rect2_ {
	AX_TYPE_INFO(Rect2_, NoBaseClass)
	static constexpr Int N = 4;
public:
	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Vec2		= Vec2_<T, SIMD>;
	using Vec4		= Vec4_<T, SIMD>;
	using Margin2	= Margin2_<T, SIMD>;
		
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { Vec2 pos, size; };
		struct { T x, y, w, h; };
	};	

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(x) {};
		AX_META_FIELD(y) {};
		AX_META_FIELD(w) {};
		AX_META_FIELD(h) {};
		using OwnFields = Tuple<x, y, w, h>;
	};

	template<class CH>
	void onFormat(Format_<CH>& ctx) const { _simd.onFormat(ctx); }

	AX_INLINE constexpr Rect2_() = default;
	AX_INLINE constexpr Rect2_(TagZero_T) : _simd(TagZero) {}
	AX_INLINE constexpr Rect2_(const SimdData & simd) : _simd(simd) {}
	AX_INLINE constexpr Rect2_(T x_, T y_, T w_, T h_) : _simd(x_,y_,w_,h_) {}
	AX_INLINE constexpr Rect2_(const Vec2& pos_, const Vec2& size_) : pos(pos_), size(size_) {}

	constexpr void	set			(T x_, T y_, T w_, T h_)				{ _simd = SimdData(x_,y_,w_,h_); }
	constexpr void	set			(const Vec2& pos_, const Vec2& size_)	{ pos = pos_; size = size_; }

	static constexpr This s_zero() { return SimdData::s_zero(); }

	AX_NODISCARD AX_INLINE    Span<T>	span() const	{ return    Span<T>(&x, kElementCount); }
	AX_NODISCARD AX_INLINE MutSpan<T>	span()			{ return MutSpan<T>(&x, kElementCount); }

	AX_NODISCARD constexpr Vec2		center		() const { return pos + size / 2; }
	AX_NODISCARD constexpr T		xCenter		() const { return x + w / 2; }
	AX_NODISCARD constexpr T		yCenter		() const { return y + h / 2; }
	AX_NODISCARD constexpr T		xMin		() const { return x; }
	AX_NODISCARD constexpr T		yMin		() const { return y; }
	AX_NODISCARD constexpr T		xMax		() const { return x + w; }
	AX_NODISCARD constexpr T		yMax		() const { return y + h; }

	AX_NODISCARD constexpr Vec2		xMin_yMin	() const { return Vec2(xMin(), yMin()); }
	AX_NODISCARD constexpr Vec2		xMax_yMin	() const { return Vec2(xMax(), yMin()); }
	AX_NODISCARD constexpr Vec2		xMin_yMax	() const { return Vec2(xMin(), yMax()); }
	AX_NODISCARD constexpr Vec2		xMax_yMax	() const { return Vec2(xMax(), yMax()); }
	AX_NODISCARD constexpr Vec2		xMin_yCenter() const { return Vec2(xMin(), yCenter()); }
	AX_NODISCARD constexpr Vec2		xMax_yCenter() const { return Vec2(xMax(), yCenter()); }
	AX_NODISCARD constexpr Vec2		xCenter_yMin() const { return Vec2(xCenter(), yMin()); }
	AX_NODISCARD constexpr Vec2		xCenter_yMax() const { return Vec2(xCenter(), yMax()); }

	AX_NODISCARD constexpr Vec2		topLeft		() const { return xMin_yMin(); }
	AX_NODISCARD constexpr Vec2		topRight	() const { return xMax_yMin(); }
	AX_NODISCARD constexpr Vec2		bottomLeft	() const { return xMin_yMax(); }
	AX_NODISCARD constexpr Vec2		bottomRight	() const { return xMax_yMax(); }

	constexpr void		set_xMin		(T v)	{ auto m = xMax(); x = v; set_xMax(m); }
	constexpr void		set_xMax		(T v)	{ w = v - x; }
	constexpr void		set_yMin		(T v)	{ auto m = yMax(); y = v; set_yMax(m); }
	constexpr void		set_yMax		(T v)	{ h = v - y; }

	constexpr void		set_xMin_yMin	(const Vec2& v) { set_xMin(v.x); set_yMin(v.y); }
	constexpr void		set_xMax_yMin	(const Vec2& v) { set_xMax(v.x); set_yMin(v.y); }
	constexpr void		set_xMin_yMax	(const Vec2& v) { set_xMin(v.x); set_yMax(v.y); }
	constexpr void		set_xMax_yMax	(const Vec2& v) { set_xMax(v.x); set_yMax(v.y); }

	AX_NODISCARD constexpr This		expand		(const Margin2& m) const { return Rect2_(x - m.left, y - m.top, w + m.left + m.right, h + m.top + m.bottom ); }
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

	AX_NODISCARD constexpr This		offset		(const Vec2& v) const				{ return Rect2_(pos + v, size); }

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(  const This& rhs) const { return _simd.almostZero(rhs._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	
	AX_NODISCARD constexpr This		operator+	(const Vec2& v) const				{ return offset( v); }
	AX_NODISCARD constexpr This		operator-	(const Vec2& v) const				{ return offset(-v); }

	AX_NODISCARD constexpr Margin2	diff		(const Rect2_& inner) const;
	AX_NODISCARD constexpr Margin2	operator-	(const Rect2_& inner) const			{ return diff(inner); }

	AX_NODISCARD constexpr This		operator+	(const Margin2& m) const			{ return expand( m); }
	AX_NODISCARD constexpr This		operator-	(const Margin2& m) const			{ return expand(-m); }
				 constexpr void		operator+=	(const Margin2& m)					{ *this = expand( m); }
				 constexpr void		operator-=	(const Margin2& m)					{ *this = expand(-m); }

	AX_NODISCARD constexpr This		operator*	(T s) const							{ return _simd * s; }
	AX_NODISCARD constexpr This		operator/	(T s) const							{ return _simd / s; }
	AX_NODISCARD constexpr This		operator*	(const Vec2& s) const				{ return _simd * s; }
	AX_NODISCARD constexpr This		operator/	(const Vec2& s) const				{ return _simd / s; }
				 constexpr void		operator*=	(T s)								{ _simd *= s; }
				 constexpr void		operator/=	(T s)								{ _simd /= s; }
				 constexpr void		operator*=	(const Vec2& s)						{ _simd *= s; }
				 constexpr void		operator/=	(const Vec2& s)						{ _simd /= s; }
	
	AX_NODISCARD constexpr bool		isValid			() const						{ return w > 0 && h > 0; }
	AX_NODISCARD constexpr This		unionWith		(const Rect2_& r) const;
	AX_NODISCARD constexpr This		intersects		(const Rect2_& r) const;
	AX_NODISCARD constexpr bool		isIntersected	(const Rect2_& r) const;
	
	AX_NODISCARD constexpr bool		containsPoint	(const Vec2& v) const	{ return v.x >= xMin() && v.x <= xMax() && v.y >= yMin() && v.y <= yMax(); }

	AX_NODISCARD constexpr Vec4		toVec4() const { return _simd; }

	AX_NODISCARD constexpr This		nonNegativeSize() const {
		return This(w<0 ? x + w : x,
					h<0 ? y + h : y,
					abs(w), abs(h));
	}

	template<class R, VecSimd R_SIMD> AX_NODISCARD AX_INLINE constexpr 
	static Rect2_ s_cast(const Rect2_<R, R_SIMD>& rhs) { return SimdData::s_cast(rhs._simd); }

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

template<class T, VecSimd SIMD> constexpr 
Margin2_<T, SIMD> Rect2_<T, SIMD>::diff(const Rect2_ & inner) const {
	return Margin2_<T, SIMD>(inner.yMin() - yMin(),
							xMax() - inner.xMax(),
							yMax() - inner.yMax(),
							inner.xMin() - xMin());
}

template<class T, VecSimd SIMD> constexpr 
bool Rect2_<T, SIMD>::isIntersected(const Rect2_& r) const {
	if (!isValid() || !r.isValid()) return false;

	if (r.x > xMax() || x > r.xMax()) return false;
	if (r.y > yMax() || y > r.yMax()) return false;
	return true;
}

template<class T, VecSimd SIMD> constexpr 
Rect2_<T, SIMD> Rect2_<T, SIMD>::unionWith(const Rect2_& r) const {
	if (!r.isValid()) return *this;
	if (!isValid()) return r;

	Vec2 topLeft	(min(xMin(), r.xMin()),
					 min(yMin(), r.yMin()));
	Vec2 bottomRight(max(xMax(), r.xMax()),
					 max(yMax(), r.yMax()));

	return This(topLeft, bottomRight - topLeft);
}

template<class T, VecSimd SIMD> constexpr 
Rect2_<T, SIMD> Rect2_<T, SIMD>::intersects(const Rect2_& r) const {
	if (!isIntersected(r)) return This(0,0,0,0);

	Vec2 topLeft	(max(xMin(), r.xMin()),
					 max(yMin(), r.yMin()));
	Vec2 bottomRight(min(xMax(), r.xMax()),
					 min(yMax(), r.yMax()));

	return This(topLeft, bottomRight - topLeft);
}

} // namespace