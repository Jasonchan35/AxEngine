#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

// HLSL alignment is 16

struct AxGpuData_Debug {
	float drawNormalLength;
	i32   debugColorCode; // AxGpuDebugColorCode_None
	float showAllLodDistance;
	float _padding0;
	float _padding1;
};

struct AxGpuData_World {
	Vec4f	timeSin;
	Vec4f	timeSlowSin;
	
	float	time;
	float	deltaTime;
	u32		_padding0;
	u32		_padding1;
};

struct AxGpuData_TileLighting {
	u32 camera;
	u32 _padding0;
	u32 _padding1;
	u32 lightCount;
	u32 lights[AX_HLSL_MAX_ACTIVE_LIGHTS];
};

struct AxGpuData_LightObject {
	Color3f	color;
	Vec3f   worldPos;
};

struct AxGpuData_Camera {
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

struct AxVertexShaderDraw_RootConst {
	Mat4f worldMatrix;
};

struct AxMeshShaderDraw_RootConst {
	Mat4f worldMatrix;
	u32   meshObjectId;
	u32   meshRendererId;
	u32   _padding0;
	u32   _padding1;
};

struct AxGpuData_MeshletCluster {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;

	u32 groupId;
	i32 refinedGroupId;
	u32 meshObjectId; // debug
	u32 lod;  // debug
};

struct AxGpuData_MeshletVert {
	Vec3f   pos;
	u32     color_packed;
	Vec2f   normal_octahedral;
	u32     uv0_packed;
	u32     uv1_packed;
};

struct AxGpuData_MeshletPrim {
	u32 	packedTriIndices;
};

struct AxGpuData_MeshletGroup {
	u32   clusterOffset;
	u32   clusterCount;
	float clusterError;
	float radius;
	Vec3f center;
	u32   meshObjectId; // debug
};

struct AxGpuData_MeshObject {
	Vec3f boundsMin;
	u32   vertOffset;
	Vec3f boundsMax;
	u32   primOffset;

	u32   meshletGroupOffset;
	u32   meshletGroupCount;
	u32   meshletClusterOffset;
	u32   meshletClusterCount;
};

struct AxGpuData_MeshObjectRenderer {
	Mat4f worldMatrix;
	u32   meshObjectId;
	u32   materialId;
	u32   _padding0;
	u32   _padding1;
};

#endif // __AX_HLSL_CPU_STRUCT_h__
