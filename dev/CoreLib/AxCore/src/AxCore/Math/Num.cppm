module;
#include "AxCore-pch.h"
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


} // namespace
