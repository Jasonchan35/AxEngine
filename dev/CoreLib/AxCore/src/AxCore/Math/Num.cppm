module;
export module AxCore.Num;
export import AxCore.MetaType;
export import AxCore.Float16;
export import AxCore.NormInt;

export namespace ax {

template<Int M, Int N, class T> struct Num_;

template<class T> using Num1_ = Num_<1,1,T>;
template<class T> using Num2_ = Num_<2,1,T>;
template<class T> using Num3_ = Num_<3,1,T>;
template<class T> using Num4_ = Num_<4,1,T>;

template<class T> using Num1x1_ = Num_<1,1,T>;
template<class T> using Num2x2_ = Num_<2,2,T>;
template<class T> using Num3x3_ = Num_<3,3,T>;
template<class T> using Num4x4_ = Num_<4,4,T>;

using Num1h = Num1_<f16>;
using Num2h = Num2_<f16>;
using Num3h = Num3_<f16>;
using Num4h = Num4_<f16>;

using Num1f = Num1_<f32>;
using Num2f = Num2_<f32>;
using Num3f = Num3_<f32>;
using Num4f = Num4_<f32>;

using Num1d = Num1_<f64>;
using Num2d = Num2_<f64>;
using Num3d = Num3_<f64>;
using Num4d = Num4_<f64>;

using Num1i = Num1_<Int>;
using Num2i = Num2_<Int>;
using Num3i = Num3_<Int>;
using Num4i = Num4_<Int>;

template<class T>
struct Num_<1,1,T> {
	T e00;
};

template<class T>
struct Num_<2,1,T> {
	T e00, e01;
};

template<class T>
struct Num_<3,1,T> {
	T e00, e01, e02;
};

template<class T>
struct Num_<4,1,T> {
	T e00, e01, e02, e03;
};

template<class T>
struct Num_<2,2,T> {
	T e00, e01;
	T e10, e11;
};

template<class T>
struct Num_<3,3,T> {
	T e00, e01, e02;
	T e10, e11, e12;
	T e20, e21, e22;
};

template<class T>
struct Num_<4,4,T> {
	T e00, e01, e02, e03;
	T e10, e11, e12, e13;
	T e20, e21, e22, e23;
	T e30, e31, e32, e33;
};

//----------
using Intx1	= Num1_<Int>;
using Intx2	= Num2_<Int>;
using Intx3	= Num3_<Int>;
using Intx4	= Num4_<Int>;

using i8x1	= Num1_<i8>;
using i8x2	= Num2_<i8>;
using i8x3	= Num3_<i8>;
using i8x4	= Num4_<i8>;

using i16x1	= Num1_<i16>;
using i16x2	= Num2_<i16>;
using i16x3	= Num3_<i16>;
using i16x4	= Num4_<i16>;

using i32x1	= Num1_<i32>;
using i32x2	= Num2_<i32>;
using i32x3	= Num3_<i32>;
using i32x4	= Num4_<i32>;

using i64x1	= Num1_<i64>;
using i64x2	= Num2_<i64>;
using i64x3	= Num3_<i64>;
using i64x4	= Num4_<i64>;

using u8x1	= Num1_<u8>;
using u8x2	= Num2_<u8>;
using u8x3	= Num3_<u8>;
using u8x4	= Num4_<u8>;

using u16x1	= Num1_<u16>;
using u16x2	= Num2_<u16>;
using u16x3	= Num3_<u16>;
using u16x4	= Num4_<u16>;

using u32x1	= Num1_<u32>;
using u32x2	= Num2_<u32>;
using u32x3	= Num3_<u32>;
using u32x4	= Num4_<u32>;

using u64x1	= Num1_<u64>;
using u64x2	= Num2_<u64>;
using u64x3	= Num3_<u64>;
using u64x4	= Num4_<u64>;

using f16x1	= Num1_<f16>;
using f16x2	= Num2_<f16>;
using f16x3	= Num3_<f16>;
using f16x4	= Num4_<f16>;

using f32x1	= Num1_<f32>;
using f32x2	= Num2_<f32>;
using f32x3	= Num3_<f32>;
using f32x4	= Num4_<f32>;

using f64x1	= Num1_<f64>;
using f64x2	= Num2_<f64>;
using f64x3	= Num3_<f64>;
using f64x4	= Num4_<f64>;

using UNorm8x1	= Num1_<UNorm8>;
using UNorm8x2	= Num2_<UNorm8>;
using UNorm8x3	= Num3_<UNorm8>;
using UNorm8x4	= Num4_<UNorm8>;

using SNorm8x1	= Num1_<SNorm8>;
using SNorm8x2	= Num2_<SNorm8>;
using SNorm8x3	= Num3_<SNorm8>;
using SNorm8x4	= Num4_<SNorm8>;

using UNorm16x1	= Num1_<UNorm16>;
using UNorm16x2	= Num2_<UNorm16>;
using UNorm16x3	= Num3_<UNorm16>;
using UNorm16x4	= Num4_<UNorm16>;

using SNorm16x1	= Num1_<SNorm16>;
using SNorm16x2	= Num2_<SNorm16>;
using SNorm16x3	= Num3_<SNorm16>;
using SNorm16x4	= Num4_<SNorm16>;

using UNorm32x1	= Num1_<UNorm32>;
using UNorm32x2	= Num2_<UNorm32>;
using UNorm32x3	= Num3_<UNorm32>;
using UNorm32x4	= Num4_<UNorm32>;

using SNorm32x1	= Num1_<SNorm32>;
using SNorm32x2	= Num2_<SNorm32>;
using SNorm32x3	= Num3_<SNorm32>;
using SNorm32x4	= Num4_<SNorm32>;


} // namespace
