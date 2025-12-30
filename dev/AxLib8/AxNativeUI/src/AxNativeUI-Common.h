#pragma once

#include "AxCore.h"

#if AX_OS_WINDOWS
	#define AX_NATIVE_UI_WIN32	1

#elif AX_OS_MACOSX
	#define AX_NATIVE_UI_MACOSX	1
#elif AX_OS_LINUX
	#define AX_NATIVE_UI_X11	1
#endif

#include "AxNativeUI/NativeUI_MACRO.h"