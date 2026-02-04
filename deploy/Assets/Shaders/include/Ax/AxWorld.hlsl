#ifndef __AxWorld_HLS__
#define __AxWorld_HLS__

#include "AxMeshlet.hlsl"

struct AxRenderGpuData_World {
	float	time;
	float	deltaTime;
	Vec4f	timeSin;
	Vec4f	timeSlowSin;
};
ConstantBuffer<AxRenderGpuData_World> axWorld : register(b100, AX_BindSpace_World);

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
ConstantBuffer<AxRenderGpuData_Camera> axCamera : register(b101, AX_BindSpace_World);

struct AxRenderGpuData_Light {
	Color4f	color;
};
StructuredBuffer<AxRenderGpuData_Light> axLights : register(t100, AX_BindSpace_World);

struct AxRenderGpuData_Object {
	Mat4f	worldMatrix;
	Vec2f	aabbMin;
	Vec2f	aabbMax;
};
StructuredBuffer<AxRenderGpuData_Object> axObjects : register(t101, AX_BindSpace_World);


#endif // __AxWorld_HLS__