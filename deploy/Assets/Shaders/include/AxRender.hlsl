#ifndef __AxRender_HLSL__
#define __AxRender_HLSL__

#include "Ax/AxWorld.hlsl"

AX_ROOT_CONST_VAR(AxDrawCallRootConst, gAxDrawCallRootConst) 
#define AX_MATRIX_MVP        gAxDrawCallRootConst.AX_MATRIX_MVP
#define AX_MATRIX_M          gAxDrawCallRootConst.AX_MATRIX_M
#define AX_OBJECT_ID         gAxDrawCallRootConst.AX_OBJECT_ID
#define AX_MESH_CLUSTER_ID   gAxDrawCallRootConst.AX_MESH_CLUSTER_ID

Vec4f AxVertex_ProjPos(Vec4f inPos) {
	// return mul(AX_MATRIX_MVP, inPos);
	return mul(axCamera.viewProjMatrix, mul(AX_MATRIX_M, inPos));
}

Vec3f AxVertex_WorldPos(Vec4f inPos) {
	Vec4f wpos = mul(AX_MATRIX_M, inPos);
	return wpos.xyz / wpos.w;
}

Vec3f AxVertex_Normal(Vec3f inNormal) {
	return mul(transpose((Mat3f)AX_MATRIX_MVP), inNormal);
}

#endif // __AxRender_HLSL__