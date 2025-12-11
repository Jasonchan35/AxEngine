#pragma once

#include "AxNativeUI.h"

#define AX_RENDERER_NULL	1

#if AX_NATIVE_UI_WIN32
//	#define AX_RENDERER_DX12	1
	#define AX_RENDERER_VK		1
#endif

#define AX_RENDER_BINDLESS		1



#include "AXRender/AxRender_MACRO.h"