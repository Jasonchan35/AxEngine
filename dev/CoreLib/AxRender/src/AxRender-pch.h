#pragma once

#include "AxCore.h"
#include "AxRender/Color/ColorType_MACRO.h"

#if AX_OS_WINDOWS
//	#define AX_RENDERER_DX12	1
//	#define AX_RENDERER_VK		1
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

#define AX_ShaderPropType_Numbers_EnumList(E) \
	E(i32,	)	E(i32x2,	)	E(i32x3,	)	E(i32x4,	)	\
	E(u32,	)	E(u32x2,	)	E(u32x3,	)	E(u32x4,	)	\
	E(f16,	)	E(f16x2,	)	E(f16x3,	)	E(f16x4,	)	\
	E(f32,	)	E(f32x2,	)	E(f32x3,	)	E(f32x4,	)	\
	E(f64,	)	E(f64x2,	)	E(f64x3,	)	E(f64x4,	)	\
//---

#define AX_ShaderPropType_EnumList_WITHOUT_NONE(E) \
	AX_ShaderPropType_Numbers_EnumList(E) \
	E(Mat4f, )\
	E(Color3f,  ) E(Color4f,  ) \
	E(Sampler,  ) \
	E(Texture2D,) E(Texture3D,) E(TextureCube,)\
//----

#define AX_ShaderPropType_EnumList(E) \
	E(None,     ) \
	AX_ShaderPropType_EnumList_WITHOUT_NONE(E) \
//----

