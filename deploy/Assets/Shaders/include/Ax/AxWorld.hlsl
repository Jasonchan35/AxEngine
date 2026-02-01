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

struct AxDrawCallRootConst {
	Mat4f AX_MATRIX_MVP;
	Mat4f AX_MATRIX_M;
	u32   AX_OBJECT_ID;
	u32   AX_MESH_ID;
	u32   AX_MESHLET_ID;
	u32   _usedPadding0;
};

AX_ROOT_CONST_STRUCT(AxDrawCallRootConst, axDrawCallRootConst) 
#define AX_OBJECT_ID		axDrawCallRootConst.AX_OBJECT_ID
#define AX_MESH_ID			axDrawCallRootConst.AX_MESH_ID
#define AX_MESHLET_ID		axDrawCallRootConst.AX_MESHLET_ID
#define AX_MATRIX_M			axDrawCallRootConst.AX_MATRIX_M
#define AX_MATRIX_MVP		axDrawCallRootConst.AX_MATRIX_MVP
#define AX_MATRIX_VP		axCamera.viewProjMatrix

Vec3f axObjectToWorldPos(Vec4f inPos) { Vec4f t = mul(AX_MATRIX_M, inPos); return t.xyz / t.w; }
Vec4f axWorldToClipPos  (Vec4f inPos) { return mul(axCamera.viewProjMatrix, inPos); }
Vec4f axObjectToClipPos (Vec4f inPos) { return mul(axCamera.viewProjMatrix, mul(AX_MATRIX_M, inPos)); }

Vec3f axObjectToWorldPos(Vec3f inPos) { return axObjectToWorldPos(Vec4f(inPos, 1)); }
Vec4f axWorldToClipPos  (Vec3f inPos) { return axWorldToClipPos  (Vec4f(inPos, 1)); }
Vec4f axObjectToClipPos (Vec3f inPos) { return axObjectToClipPos (Vec4f(inPos, 1)); }

Vec3f axObjectToWorldNormal(Vec3f inNormal) { return mul(transpose((Mat3f)AX_MATRIX_M  ), inNormal); }
Vec3f axWorldToClipNormal  (Vec3f inNormal) { return mul(transpose((Mat3f)AX_MATRIX_VP ), inNormal); }
Vec3f axObjectToClipNormal (Vec3f inNormal) { return mul(transpose((Mat3f)AX_MATRIX_MVP), inNormal); }


#endif // __AxWorld_HLS__