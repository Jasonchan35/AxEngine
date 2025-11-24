module;
#include "AxPlatform-pch.h"

export module AxPlatform.CpuSIMD;

import AxPlatform.StrView;
import AxPlatform.Enum;

export namespace ax {

#define AX_CpuSIMD_ENUM_LIST(E) \
	E(None,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_CpuSIMD_ENUM_LIST, CpuSIMD, u8)

} // namespace
