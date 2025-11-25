module;
#include "AxCore-pch.h"
export module AxCore.VecBase;
export import AxCore._PCH;

export namespace ax {

#define AX_CpuSIMD_ENUM_LIST(E) \
	E(None,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_CpuSIMD_ENUM_LIST, CpuSIMD, u8)

constexpr CpuSIMD VecSIMD_Default = CpuSIMD::SSE;

// forward declare
template<Int N, class T, CpuSIMD SIMD = VecSIMD_Default> class VecBase_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> using VecBase1_ = VecBase_<1, T, SIMD>;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> using VecBase2_ = VecBase_<2, T, SIMD>;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> using VecBase3_ = VecBase_<3, T, SIMD>;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> using VecBase4_ = VecBase_<4, T, SIMD>;

template<Int N, class T, CpuSIMD SIMD = VecSIMD_Default> class Vec_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Vec1_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Vec2_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Vec3_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Vec4_;

template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Rect2_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Mat3_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Mat4_;
template<       class T, CpuSIMD SIMD = VecSIMD_Default> class Quat4_;


template<class T, CpuSIMD SIMD>
class VecBase_<1, T, SIMD> {
	using This = VecBase_;
public:
	using ElementType = T;
	static constexpr Int ElementCount = 1;

	T	x;
	
	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_(const T& x_) : x(x_) {}
	AX_INLINE constexpr void set(const T& x_) { x = x_; }

	AX_INLINE 		T*	data()		 { return &x; }
	AX_INLINE const	T*	data() const { return &x; }
	
	AX_NODISCARD AX_INLINE constexpr bool operator==(const This& r) const	{ return x == r.x; }
	AX_NODISCARD AX_INLINE constexpr bool operator!=(const This& r) const	{ return x != r.x; }

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, ElementCount>;
	using MFixedSpan = MutFixedSpan<T, ElementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

} // namespace

