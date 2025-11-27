module;
#include "AxCore-pch.h"
export module AxCore.Margin;
export import AxCore.Vec;

export namespace ax {

template<Int N, class T, CpuSIMD SIMD> class Margin_;
template<Int N, class T, CpuSIMD SIMD> class Margin_Storage_;
template<Int N, class T, CpuSIMD SIMD> using MarginBase_ = NumSIMD_<N,  Margin_<N, T, SIMD>, Margin_Storage_<N, T, SIMD> >;

template<class T, CpuSIMD SIMD = CpuSIMD_Default> using Margin4_ = Margin_<4, T, SIMD>;

using Margin4h			= Margin4_<f16>;
using Margin4h_SSE		= Margin4_<f16, CpuSIMD::SSE>;
using Margin4h_Basic	= Margin4_<f16, CpuSIMD::None>;
using Margin4f			= Margin4_<f32>;
using Margin4f_SSE		= Margin4_<f32, CpuSIMD::SSE>;
using Margin4f_Basic	= Margin4_<f32, CpuSIMD::None>;
using Margin4d			= Margin4_<f64>;
using Margin4d_SSE		= Margin4_<f64, CpuSIMD::SSE>;
using Margin4d_Basic	= Margin4_<f64, CpuSIMD::None>;


template<class T, CpuSIMD SIMD>
class Margin_Storage_<4, T, SIMD> {
	using REG = CpuSIMD_Register_<4, T, SIMD>;
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
		struct { T top, right, bottom, left;}; // clock-wise, same as CSS Margin order
	};
	
	AX_NODISCARD AX_INLINE			constexpr Margin_Storage_() = default;
	AX_NODISCARD AX_INLINE explicit	constexpr Margin_Storage_(const MM& m) : _m(m) {}
	AX_NODISCARD AX_INLINE			constexpr Margin_Storage_(const T& e0, const T& e1, const T& e2, const T& e3) : _e0(e0), _e1(e1), _e2(e2), _e3(e3) {}
};

template<class T, CpuSIMD SIMD>
class Margin_<4, T, SIMD> : public MarginBase_<4, T, SIMD> {
	using Base = MarginBase_<4, T, SIMD>;
	using This = Margin_;
public:
	using Storage = typename Base::Storage;
	using Element = typename Base::Element;
	static_assert(Type_IsSame<T, Element>);
	static constexpr Int     elementCount = Base::elementCount;
	static constexpr CpuSIMD cpuSIMD      = Base::cpuSIMD;
	static_assert(SIMD == cpuSIMD);

	using Storage::top;
	using Storage::right;
	using Storage::bottom;
	using Storage::left;

	using Vec2 = Vec2_<T, SIMD>;
	
	AX_INLINE constexpr Margin_() = default;
	AX_INLINE constexpr Margin_(Tag::All_, const T& v) : Base(Tag::All, v) {}
	AX_INLINE constexpr Margin_(const Storage & storage) : Base(storage) {}
	AX_INLINE constexpr Margin_(const T& top_, const T& right_, const T& bottom_, const T& left_) : Base(top_, right_, bottom_, left_) {}
	
	AX_NODISCARD Vec2	total	() const { return Vec2(totalX(), totalY()); }
	AX_NODISCARD T		totalX	() const { return left + right;  }
	AX_NODISCARD T		totalY	() const { return top  + bottom; }

	AX_NODISCARD Vec2	topLeft		() const { return Vec2(left,  top   ); }
	AX_NODISCARD Vec2	topRight	() const { return Vec2(right, top   ); }
	AX_NODISCARD Vec2	bottomLeft	() const { return Vec2(left,  bottom); }
	AX_NODISCARD Vec2	bottomRight	() const { return Vec2(right, bottom); }
};

} // namespace 