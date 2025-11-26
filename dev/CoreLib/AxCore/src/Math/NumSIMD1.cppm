module;
#include "AxCore-pch.h"
export module AxCore.NumSIMD1;
export import AxCore._PCH;

export namespace ax {

#define AX_NumSIMD_ENUM_LIST(E) \
	E(NoSIMD,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_NumSIMD_ENUM_LIST, CpuSIMD, u8)

template<Int N, class VEC, class STORAGE> class NumSIMD_;
template<       class VEC, class STORAGE> using NumSIMD1_  = NumSIMD_<1, VEC, STORAGE>;
template<       class VEC, class STORAGE> using NumSIMD2_  = NumSIMD_<2, VEC, STORAGE>;
template<       class VEC, class STORAGE> using NumSIMD3_  = NumSIMD_<3, VEC, STORAGE>;
template<       class VEC, class STORAGE> using NumSIMD4_  = NumSIMD_<4, VEC, STORAGE>;

template<class VEC, class T>
struct NumSIMD_NumLimit {
	using T_NumLimit = NumLimit<T>;

	static constexpr bool isExactType   =  T_NumLimit::isExactType;
	static constexpr bool hasInfinity   =  T_NumLimit::hasInfinity;
	static constexpr VEC  infinity      =  VEC(Tag::All, T_NumLimit::infinity);
	static constexpr VEC  negInfinity   =  VEC(Tag::All, T_NumLimit::negInfinity);
	static constexpr VEC  lowest        =  VEC(Tag::All, T_NumLimit::lowest);
	static constexpr VEC  min           =  VEC(Tag::All, T_NumLimit::min);
	static constexpr VEC  max           =  VEC(Tag::All, T_NumLimit::max);
	static constexpr VEC  epsilon       =  VEC(Tag::All, T_NumLimit::epsilon);
	static constexpr VEC  NaN           =  VEC(Tag::All, T_NumLimit::NaN);
};


//----------------


} // namespace
