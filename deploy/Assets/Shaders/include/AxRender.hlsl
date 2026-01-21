#ifndef __AxRender_HLSL__
#define __AxRender_HLSL__

#include "Ax/AxWorld.hlsl"

AX_ROOT_CONST_VAR(AxDrawCallRootConst, gAxDrawCallRootConst) 
#define AX_MATRIX_MVP gAxDrawCallRootConst.AX_MATRIX_MVP

cbuffer AX_ConstBuffer_World : register(b0, AX_BindSpace_World) {
	float	ax_g_test;

	float	ax_g_time;
	Vec4f	ax_g_timeSin;
	Vec4f	ax_g_timeSlowSin;
	Vec4f	ax_g_deltaTime;
	Vec4f	ax_g_worldSpaceCameraPos;
	Mat4f	ax_g_cameraProjection;
	Mat4f	ax_g_cameraInvProjection;
}

#endif // __AxRender_HLSL__