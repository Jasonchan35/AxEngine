#pragma once

AX_GCC_WARNING_PUSH_AND_DISABLE("-Wmicrosoft-enum-value") // INVALID_VALUE  = 0xFFFFFFFF,

AX_VC_WARNING_PUSH_AND_DISABLE(5039)
// Warning C5039 : 'qsort': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc.
// Undefined behavior may occur if this function throws an exception.

AX_VC_WARNING_PUSH_AND_DISABLE(5262)
// Warning C5262 : implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]]
// when a break statement is intentionally omitted between cases

#include "spirv_reflect.c"

AX_VC_WARNING_POP()
AX_VC_WARNING_POP()
AX_GCC_WARNING_POP()