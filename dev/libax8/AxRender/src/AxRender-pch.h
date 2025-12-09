#pragma once

#include "AxNativeUI.h"

#if AX_OS_WINDOWS
//	#define AX_RENDERER_DX12	1
	#define AX_RENDERER_VK		1
#endif

#define AX_RENDER_BINDLESS		1

#include "AxRender/Backend/VK/AX_Vulkan_HEADER.h"
#include "AXRender/AxRender_MACRO.h"