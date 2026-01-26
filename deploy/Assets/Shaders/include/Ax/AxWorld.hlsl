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

#endif // __AxWorld_HLS__