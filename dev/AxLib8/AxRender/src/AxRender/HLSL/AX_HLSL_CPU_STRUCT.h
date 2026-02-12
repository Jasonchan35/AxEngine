#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

// HLSL alignment is 16

struct AxRenderGpuData_Debug {
	float drawNormalLength;
	float _padding0;
	float _padding1;
	float _padding2;
};

struct AxRenderGpuData_World {
	float	time;
	float	deltaTime;
	Vec4f	timeSin;
	Vec4f	timeSlowSin;
};

struct AxRenderGpuData_Light {
	Color3f	color;
	Vec3f   worldPos;
};

struct AxRenderGpuData_Object {
	Mat4f	worldMatrix;
	Vec2f	aabbMin;
	Vec2f	aabbMax;
};

struct AxRenderGpuData_Camera {
	Vec3f	worldPos;
	float   fieldOfView; // vertical

	float   nearClip;
	float   farClip;
	float   maxMeshletErrorInPixels;
	float   _padding0;
	
	Vec2f	viewportMin;
	Vec2f	viewportMax;
	
	Mat4f	projMatrix;
	Mat4f	projMatrixInv;
	Mat4f	viewMatrix;
	Mat4f	viewMatrixInv;
	Mat4f	viewProjMatrix;
	Mat4f	viewProjMatrixInv;
};

struct AxVertexShaderDrawRootConst {
	Mat4f worldMatrix;
};

struct AxMeshShaderDrawRootConst {
	Mat4f worldMatrix;
	u32   meshObjectId;
	u32   meshletGroupOffset;
	u32   meshletGroupCount;
	u32   meshRendererId;
};

struct AxGpuMeshlet {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;

	u32 groupId;
	i32 refinedGroupId;
	u32 meshObjectId; // debug
	u32 lod;  // debug
};

struct AxGpuMeshletVert {
	Vec3f   pos;
	u32     rawColor;

	Vec3f   normal;
	u32     _padding;

	Vec2f   uv0;
	Vec2f   uv1;
};

struct AxGpuMeshletPrim {
	u32x3 	tri;
	u32 	_padding;
};

struct AxGpuMeshletGroup {
	u32   meshletOffset;
	u32   meshletCount;
	float clusterError;
	float radius;
	Vec3f center;
	u32   meshObjectId; // debug
};

struct AxGpuMeshObject {
	Vec3f boundsMin;
	u32   vertOffset;
	Vec3f boundsMax;
	u32   primOffset;

	u32   meshletGroupOffset;
	u32   meshletGroupCount;
	u32   meshletOffset;
	u32   meshletCount;
};

struct AxGpuMeshObjectRenderer {
	Mat4f worldMatrix;
	u32   meshObjectId;
	u32   materialId;
	u32   _padding0;
	u32   _padding1;
};

#endif // __AX_HLSL_CPU_STRUCT_h__
