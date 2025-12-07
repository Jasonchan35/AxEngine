#pragma once

#include "AxCore.h"

#if AX_OS_WINDOWS
	#define AX_RENDERER_DX12	1
	#define AX_RENDERER_VK		1
#endif

#define AX_RENDER_BINDLESS		1

#define AX_RENDER_StockTextureId_SolidColor_ENUM_LIST(E) \
	E(Zero			,) \
	E(Black			,) \
	E(White			,) \
	E(Red			,) \
	E(Green			,) \
	E(Blue			,) \
	E(Yellow		,) \
	E(Cyan			,) \
	E(Magenta		,) \
	E(Gray			,) \
	E(DarkRed		,) \
	E(DarkGreen		,) \
	E(DarkBlue		,) \
	E(DarkYellow	,) \
	E(DarkCyan		,) \
	E(DarkMagenta	,) \
//-----