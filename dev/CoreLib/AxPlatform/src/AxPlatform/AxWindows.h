#pragma once

#if AX_USE_PRECOMPILE_HEADER && AX_OS_WINDOWS

#include <conio.h>
#include <windows.h>

#ifdef small
#undef small // char
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#endif // AX_USE_PRECOMPILE_HEADER
