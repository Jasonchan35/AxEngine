module;
#include "AxCore-pch.h"

export module AxCore.CpuSIMD;

import AxCore.StrView;
import AxCore.Enum;

export namespace ax {

#define AX_CpuSIMD_ENUM_LIST(E) \
	E(None,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_CpuSIMD_ENUM_LIST, CpuSIMD, u8)

} // namespace
