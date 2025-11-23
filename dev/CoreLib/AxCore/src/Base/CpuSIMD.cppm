export module AxCore.CpuSIMD;

import "AxBase.h";
// SSE / AVX
import <xmmintrin.h>;

#if AX_COMPILER_CLANG && AX_OS_WINDOWS
	import <avxintrin.h>;
#else
	import <immintrin.h>;
#endif

import AxCore.StrView;
import AxCore.Enum;

export namespace ax {

#define AX_CpuSIMD_ENUM_LIST(E) \
	E(None,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_CpuSIMD_ENUM_LIST, CpuSIMD, u8)

} // namespace
