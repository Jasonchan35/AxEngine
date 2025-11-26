module;
#include "AxCore-pch.h"
export module AxCore.VecBase;
export import AxCore.NumSIMD4;

export import AxCore.NormInt;

export namespace ax {

#if 0

inline constexpr CpuSIMD VecSIMD_default = CpuSIMD::SSE;

template<Int N, class T, CpuSIMD SIMD = VecSIMD_default> class VecBase_;
template<class T, CpuSIMD SIMD = VecSIMD_default> using VecBase1_  = VecBase_<1, T, SIMD>;
template<class T, CpuSIMD SIMD = VecSIMD_default> using VecBase2_  = VecBase_<2, T, SIMD>;
template<class T, CpuSIMD SIMD = VecSIMD_default> using VecBase3_  = VecBase_<3, T, SIMD>;
template<class T, CpuSIMD SIMD = VecSIMD_default> using VecBase4_  = VecBase_<4, T, SIMD>;

template<Int N, class T, CpuSIMD SIMD = VecSIMD_default> class Vec_;
template<       class T, CpuSIMD SIMD = VecSIMD_default> using Vec1_      = Vec_<1, T, SIMD>;
template<       class T, CpuSIMD SIMD = VecSIMD_default> using Vec2_      = Vec_<2, T, SIMD>;
template<       class T, CpuSIMD SIMD = VecSIMD_default> using Vec3_      = Vec_<3, T, SIMD>;
template<       class T, CpuSIMD SIMD = VecSIMD_default> using Vec4_      = Vec_<4, T, SIMD>;

template<Int N, class T, CpuSIMD SIMD = VecSIMD_default> class Quat_;
template<class T, CpuSIMD SIMD = VecSIMD_default> using Quat4_     = Quat_<4, T, SIMD>;

template<Int N, class T, CpuSIMD SIMD = VecSIMD_default> class Rect_;
template<class T, CpuSIMD SIMD = VecSIMD_default> using Rect2_     = Rect_<2, T, SIMD>;

template<Int ROW, Int Col, class T, CpuSIMD SIMD = VecSIMD_default> class Mat_;
// template<class T, VecSIMD SIMD = VecSIMD_default> using Mat3_   = Mat_<3, 3, T, SIMD>;
// template<class T, VecSIMD SIMD = VecSIMD_default> using Mat4x3_ = Mat_<4, 3, T, SIMD>;
template<class T, CpuSIMD SIMD = VecSIMD_default> using Mat4_      = Mat_<4, 4, T, SIMD>;

template<class T, CpuSIMD SIMD>
class VecBase_<1, T, SIMD> {
	using This = VecBase_;
public:
	using Element = T;
	static constexpr Int elementCount = 1;

	T	x;
	
	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_( const T& x_) : x(x_) {}

	AX_INLINE 		T*	data()		 { return &x; }
	AX_INLINE const	T*	data() const { return &x; }
	
	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};

template<class T, CpuSIMD SIMD>
class VecBase_<2, T, SIMD> {
	using This = VecBase_;
public:
	using Element = T;
	static constexpr Int elementCount = 2;

	T x, y;

	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_( const T& x_, const T& y_) : x(x_), y(y_) {}
	
			T*	data()		 { return &x; }
	const	T*	data() const { return &x; }

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};

template<class T, CpuSIMD SIMD>
class VecBase_<3, T, SIMD> {
	using This = VecBase_;
public:
	using Element = T;
	static constexpr Int elementCount = 3;

	union {
		struct { T x, y, z; };
		struct { T r, g, b; };
	};
	
	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_(const T& x_, const T& y_, const T& z_) : x(x_), y(y_), z(z_) {}

			T*	data()		 { return &x; }
	const	T*	data() const { return &x; }

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};

template<>
class VecBase_<3, f32, CpuSIMD::SSE> {
	using This = VecBase_;
	using T = f32;
public:
	using Element = T;
	static constexpr Int elementCount = 3;

	union {
		struct { T x, y, z, _unused_padding; };
		__m128 _m; // SSE
	};
	
	AX_INLINE			VecBase_() = default;
	AX_INLINE constexpr	VecBase_(const __m128& m) : _m(m) {}

			T*	data()		 { return &x; }
	const	T*	data() const { return &x; }
	
	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};

template<>
class VecBase_<3, f64, CpuSIMD::SSE> {
	using This = VecBase_;
	using T = f64;
public:
	using Element = T;
	static constexpr Int elementCount = 3;

	AX_INLINE			VecBase_() = default;
	AX_INLINE constexpr VecBase_(const __m256d& m) : _m(m) {}

	union {
		struct { T x, y, z; T _unused_padding; };
		__m256d _m; // SSE
	};
			T*	data()		 { return &x; }
	const	T*	data() const { return &x; }

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};


template<class T, CpuSIMD SIMD>
class VecBase_<4, T, SIMD> {
	using This = VecBase_;
public:
	using Element = T;
	static constexpr Int elementCount = 4;

	AX_INLINE constexpr VecBase_() = default;
	AX_INLINE constexpr VecBase_(const T& x_, const T& y_, const T& z_, const T& w_) : x(x_), y(y_), z(z_), w(w_) {}

	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) { x = x_; y = y_; z = z_; w = w_; }

	union {
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
	};
	
			T*	data()		 { return &x; }
	const	T*	data() const { return &x; }

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};

template<>
class VecBase_<4, f32, CpuSIMD::SSE> {
	using This = VecBase_;
	using T = f32;
public:
	using Element = f32;
	static constexpr Int elementCount = 4;

	union {
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		__m128 _m; // SSE
	};
	
	AX_INLINE			VecBase_() = default;
	AX_INLINE constexpr VecBase_(const __m128& m) : _m(m) {}
	AX_INLINE constexpr VecBase_(const T& x_, const T& y_, const T& z_, const T& w_) : x(x_), y(y_), z(z_), w(w_) {}
	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) { x = x_; y = y_; z = z_; w = w_; }

			T*	data()		 { return &x; }
	const	T*	data() const { return &x; }

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
};

template<>
class VecBase_<4, f64, CpuSIMD::SSE> {
	using This = VecBase_;
	using T = f64;
public:
	using Element = T;
	static constexpr Int elementCount = 4;

	union {
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		__m256d _m; // AVX
	};
	
	AX_INLINE			VecBase_() = default;
	AX_INLINE constexpr VecBase_(const __m256d& m) : _m(m) {}
	AX_INLINE constexpr VecBase_(const T& x_, const T& y_, const T& z_, const T& w_) : x(x_), y(y_), z(z_), w(w_) {}
};

template<Int N, class T, CpuSIMD SIMD>
struct NumLimit<Vec_<N, T, SIMD>> {
	using VEC = Vec_<N, T, SIMD>;
	using ElemLimit = NumLimit<T>;

	static constexpr bool isExactType   =  ElemLimit::isExactType;
	static constexpr bool hasInfinity   =  ElemLimit::hasInfinity;
	static constexpr VEC  infinity      =  VEC(Tag::All, ElemLimit::infinity);
	static constexpr VEC  negInfinity   =  VEC(Tag::All, ElemLimit::negInfinity);
	static constexpr VEC  lowest        =  VEC(Tag::All, ElemLimit::lowest);
	static constexpr VEC  min           =  VEC(Tag::All, ElemLimit::min);
	static constexpr VEC  max           =  VEC(Tag::All, ElemLimit::max);
	static constexpr VEC  epsilon       =  VEC(Tag::All, ElemLimit::epsilon);
	static constexpr VEC  NaN           =  VEC(Tag::All, ElemLimit::NaN);
};

//----------
using Intx1	= VecBase1_<Int>;
using Intx2	= VecBase2_<Int>;
using Intx3	= VecBase3_<Int>;
using Intx4	= VecBase4_<Int>;

using i8x1	= VecBase1_<i8>;
using i8x2	= VecBase2_<i8>;
using i8x3	= VecBase3_<i8>;
using i8x4	= VecBase4_<i8>;

using i16x1	= VecBase1_<i16>;
using i16x2	= VecBase2_<i16>;
using i16x3	= VecBase3_<i16>;
using i16x4	= VecBase4_<i16>;

using i32x1	= VecBase1_<i32>;
using i32x2	= VecBase2_<i32>;
using i32x3	= VecBase3_<i32>;
using i32x4	= VecBase4_<i32>;

using i64x1	= VecBase1_<i64>;
using i64x2	= VecBase2_<i64>;
using i64x3	= VecBase3_<i64>;
using i64x4	= VecBase4_<i64>;

using u8x1	= VecBase1_<u8>;
using u8x2	= VecBase2_<u8>;
using u8x3	= VecBase3_<u8>;
using u8x4	= VecBase4_<u8>;

using u16x1	= VecBase1_<u16>;
using u16x2	= VecBase2_<u16>;
using u16x3	= VecBase3_<u16>;
using u16x4	= VecBase4_<u16>;

using u32x1	= VecBase1_<u32>;
using u32x2	= VecBase2_<u32>;
using u32x3	= VecBase3_<u32>;
using u32x4	= VecBase4_<u32>;

using u64x1	= VecBase1_<u64>;
using u64x2	= VecBase2_<u64>;
using u64x3	= VecBase3_<u64>;
using u64x4	= VecBase4_<u64>;

using f16x1	= VecBase1_<f16>;
using f16x2	= VecBase2_<f16>;
using f16x3	= VecBase3_<f16>;
using f16x4	= VecBase4_<f16>;

using f32x1	= VecBase1_<f32>;
using f32x2	= VecBase2_<f32>;
using f32x3	= VecBase3_<f32>;
using f32x4	= VecBase4_<f32>;

using f64x1	= VecBase1_<f64>;
using f64x2	= VecBase2_<f64>;
using f64x3	= VecBase3_<f64>;
using f64x4	= VecBase4_<f64>;

using UNorm8x1	= VecBase1_<UNorm8>;
using UNorm8x2	= VecBase2_<UNorm8>;
using UNorm8x3	= VecBase3_<UNorm8>;
using UNorm8x4	= VecBase4_<UNorm8>;

using SNorm8x1	= VecBase1_<SNorm8>;
using SNorm8x2	= VecBase2_<SNorm8>;
using SNorm8x3	= VecBase3_<SNorm8>;
using SNorm8x4	= VecBase4_<SNorm8>;

using UNorm16x1	= VecBase1_<UNorm16>;
using UNorm16x2	= VecBase2_<UNorm16>;
using UNorm16x3	= VecBase3_<UNorm16>;
using UNorm16x4	= VecBase4_<UNorm16>;

using SNorm16x1	= VecBase1_<SNorm16>;
using SNorm16x2	= VecBase2_<SNorm16>;
using SNorm16x3	= VecBase3_<SNorm16>;
using SNorm16x4	= VecBase4_<SNorm16>;

using UNorm32x1	= VecBase1_<UNorm32>;
using UNorm32x2	= VecBase2_<UNorm32>;
using UNorm32x3	= VecBase3_<UNorm32>;
using UNorm32x4	= VecBase4_<UNorm32>;

using SNorm32x1	= VecBase1_<SNorm32>;
using SNorm32x2	= VecBase2_<SNorm32>;
using SNorm32x3	= VecBase3_<SNorm32>;
using SNorm32x4	= VecBase4_<SNorm32>;

//---- SSE ----
using f32x1_SSE = VecBase1_<f32, CpuSIMD::SSE>;
using f32x2_SSE = VecBase2_<f32, CpuSIMD::SSE>;
using f32x3_SSE = VecBase3_<f32, CpuSIMD::SSE>;
using f32x4_SSE = VecBase4_<f32, CpuSIMD::SSE>;

using f64x1_SSE = VecBase1_<f64, CpuSIMD::SSE>;
using f64x2_SSE = VecBase2_<f64, CpuSIMD::SSE>;
using f64x3_SSE = VecBase3_<f64, CpuSIMD::SSE>;
using f64x4_SSE = VecBase4_<f64, CpuSIMD::SSE>;

//--------

using Num1h = f16x1;
using Num2h = f16x2;
using Num3h = f16x3;
using Num4h = f16x4;

using Num1f = f32x1;
using Num2f = f32x2;
using Num3f = f32x3;
using Num4f = f32x4;

using Num1d = f64x1;
using Num2d = f64x2;
using Num3d = f64x3;
using Num4d = f64x4;

using Num1i = Intx1;
using Num2i = Intx2;
using Num3i = Intx3;
using Num4i = Intx4;

using Vec1i		= Vec1_<Int>;
using Vec1h		= Vec1_<f16>;
using Vec1f		= Vec1_<f32>;
using Vec1d		= Vec1_<f64>;

using Vec2i		= Vec2_<Int>;
using Vec2h		= Vec2_<f16>;
using Vec2f		= Vec2_<f32>;
using Vec2d		= Vec2_<f64>;

using Vec3i		= Vec3_<Int>;
using Vec3h		= Vec3_<f16>;
using Vec3f		= Vec3_<f32>;
using Vec3d		= Vec3_<f64>;

using Vec4i		= Vec4_<Int>;
using Vec4h		= Vec4_<f16>;
using Vec4f		= Vec4_<f32>;
using Vec4d		= Vec4_<f64>;

using Vec3f_SSE	= Vec3_<f32, CpuSIMD::SSE>;
using Vec3d_SSE	= Vec3_<f64, CpuSIMD::SSE>;

using Vec4f_SSE	= Vec4_<f32, CpuSIMD::SSE>;
using Vec4d_SSE	= Vec4_<f64, CpuSIMD::SSE>;

#endif
} // namespace

