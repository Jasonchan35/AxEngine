#pragma once

#include "AxRender.h"

#if AX_OS_WINDOWS
	#include "AxCore-OS_Windows.h"
#endif

#if AX_RENDERER_VK
	#include "spirv_reflect.h"
#endif

#if AX_RENDERER_DX12
	#if AX_OS_WINDOWS
		using AX_WinBOOL = BOOL;
		#include <dxcapi.h> // from Windows SDK - require this for signed shader
	//	#include <dxc/dxcapi.h> // from Vulkan SDK
		#include <d3d12shader.h>
		#pragma comment(lib, "dxcompiler.lib")
	#else
		#define BOOL axWinBOOL
		using AX_WinBOOL = bool;
		#include <dxc/dxcapi.h>
		AX_STATIC_ASSERT(std::is_same_v<BOOL, AX_WinBOOL>);
		#undef BOOL
	#endif
#endif
