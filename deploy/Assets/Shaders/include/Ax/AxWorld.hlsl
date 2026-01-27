#ifndef __AxWorld_HLS__
#define __AxWorld_HLS__

#include "AxBasicType.hlsl"

struct AxRenderGpuData_World {
	float	time;
	float	deltaTime;
	Vec4f	timeSin;
	Vec4f	timeSlowSin;
};
ConstantBuffer<AxRenderGpuData_World> axWorld : register(b0, AX_BindSpace_World);

struct AxRenderGpuData_Camera {
	Vec3f	worldPos;
	Vec2f	viewportMin;
	Vec2f	viewportMax;
	Mat4f	projMatrix;
	Mat4f	projMatrixInv;
	Mat4f	viewMatrix;
	Mat4f	viewMatrixInv;
	Mat4f	viewProjMatrix;
	Mat4f	viewProjMatrixInv;
};
ConstantBuffer<AxRenderGpuData_Camera> axCamera : register(b1, AX_BindSpace_World);

struct AxRenderGpuData_Light {
	Color4f	color;
};
RWStructuredBuffer<AxRenderGpuData_Light> axLights : register(u1, AX_BindSpace_World);

struct AxRenderGpuData_Object {
	Mat4f	worldMatrix;
	Vec2f	aabbMin;
	Vec2f	aabbMax;
};
RWStructuredBuffer<AxRenderGpuData_Object> axObjects : register(u2, AX_BindSpace_World);

struct AxDrawCallRootConst {
	Mat4f AX_MATRIX_MVP;
	Mat4f AX_MATRIX_M;
	u32   AX_OBJECT_ID;
	u32   AX_MESH_CLUSTER_ID;
	u32   _usedPadding0;
	u32   _usedPadding1;
};

AX_ROOT_CONST_STRUCT(AxDrawCallRootConst, axDrawCallRootConst) 
#define AX_MATRIX_MVP        axDrawCallRootConst.AX_MATRIX_MVP
#define AX_MATRIX_M          axDrawCallRootConst.AX_MATRIX_M
#define AX_OBJECT_ID         axDrawCallRootConst.AX_OBJECT_ID
#define AX_MESH_CLUSTER_ID   axDrawCallRootConst.AX_MESH_CLUSTER_ID

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


#endif // __AxWorld_HLS__