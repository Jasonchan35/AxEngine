#ifndef __AxWorld_HLS__
#define __AxWorld_HLS__

#include "AxBasicType.hlsl"

struct AxCameraData {
	Mat4f	viewMatrix;
	Mat4f	projMatrix;
	Mat4f	viewProjMatrix;
};
RWStructuredBuffer<AxCameraData> gAxCameraData : register(u0, AX_BindSpace_Object);

struct AxLightData {
	Color4f	color;
};
RWStructuredBuffer<AxLightData> gAxLightData : register(u1, AX_BindSpace_Object);

struct AxObjectData {
	Mat4f	objectToWorld;
	Vec2f	aabbMin;
	Vec2f	aabbMax;
};
RWStructuredBuffer<AxObjectData> gAxObjectData : register(u2, AX_BindSpace_Object);

struct AxDrawCallRootConst {
	Mat4f AX_MATRIX_MVP;
	Mat4f AX_MATRIX_M;
	u32   AX_OBJECT_ID;
	u32   AX_MESH_CLUSTER_ID;
	u16   AX_CAMERA_ID;
	u16   _usedPadding0;
	u32   _usedPadding1;
};

#endif // __AxWorld_HLS__