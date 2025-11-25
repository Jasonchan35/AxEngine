#pragma once

#if AX_OS_WINDOWS

#if AX_USE_PRECOMPILE_HEADER
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

#endif // else AX_USE_PRECOMPILE_HEADER

#endif // AX_OS_WINDOWS
