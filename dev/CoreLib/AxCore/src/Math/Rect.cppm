module;
#include "AxCore-pch.h"
export module AxCore.Rect;
export import AxCore.Vec;

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

template<Int N, class T, CpuSIMD SIMD> class Rect_;
template<Int N, class T, CpuSIMD SIMD> using RectBase_ = NumSIMD_<N,  Rect_<N, T, SIMD>, Rect_Storage_<N, T, SIMD> >;

template<class T, CpuSIMD SIMD>
class Rect_<4, T, SIMD> : public VecBase_<4, T, SIMD> {
	using Base = VecBase_<4, T, SIMD>;
	using This = Vec_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);

	using Vec2 = Vec2_<T, SIMD>;
	using Storage::x;
	using Storage::y;
	using Storage::w;
	using Storage::h;
	using Storage::pos;
	using Storage::extents;
	
	AX_INLINE constexpr Rect_() = default;
	AX_INLINE constexpr Rect_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Rect_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Rect_(const T& x_, const T& y_, const T& w_, const T& h_) : Base(x_, y_, w_, h_) {}
	
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
};



} // namespace

