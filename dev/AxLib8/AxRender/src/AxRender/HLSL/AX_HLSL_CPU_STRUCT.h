#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

// HLSL alignment is 16

struct AxGpuData_Debug {
	float drawNormalLength;
	float drawCluster;
	float showAllLodDistance;
	i32   debugColorCode; // AxGpuDebugColorCode_None
	u32   flags;
	float _padding1;
};

struct AxGpuData_World {
	Vec4f	timeSin;
	Vec4f	timeSlowSin;
	
	f32		time;
	f32		deltaTime;
	u32		_padding0;
	u32		_padding1;
};

struct AxGpuData_TileLighting {
	u32 cameraId;
	u32 _padding0;
	u32 _padding1;
	u32 lightCount;
	u32 lightIds[AX_HLSL_MAX_ACTIVE_LIGHTS];
};

struct AxGpuData_LightObject {
	Vec3f	worldPos;
	float	radius;
	Color3f	color;
	float	intensity;
};

struct AxGpuData_CameraObject {
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

struct AxGpuData_Camera {
	//TODO - remove this struct
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

	Vec3f	cullingCameraPos;
	float	_padding1;
	Vec4f	cullingPlanes[6]; // xyz is normal, w is distance
};

struct AxVertexShaderDraw_RootConst {
	Mat4f worldMatrix;
};

struct AxMeshShaderDraw_RootConst {
	Mat4f worldMatrix;
	u32   meshObjectId;
	u32   cameraId;
	u32   _padding0;
	u32   _padding1;
};

struct AxGpuData_MeshletCluster {
	u32 vertOffset;
	u32 primOffset;

	u32 vert_prim_count;  // u8=vertCount u8=primCount
	u32 cone_axis_cutoff_packed; // packed axis=(u8x3), cutoff=s8:cos(angle/2) 
	
//	Vec4f cone_axis_cutoff;
	
	Vec3f center;
	f32   radius;

	u32   groupId;
	i32   refinedGroupId;
	u32   meshObjectId; // debug
	u32   lod;  // debug
};

struct AxGpuData_MeshletVert {
	Vec3f   pos;
	u32     color_packed;
//	Vec3f   normal;
//	float   _padding0;
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
	float error;
	float radius;
	Vec3f center;
	u32   meshObjectId; // debug
};

struct AxGpuData_MeshObject {
	u32   meshletGroupOffset;
	u32   meshletGroupCount;
	u32   meshletClusterOffset;
	u32   meshletClusterCount;

	Vec3f boundsMin;	u32 vertOffset;
	Vec3f boundsMax;	u32 primOffset;
};

struct AxGpuData_MeshRenderer {
	Mat4f worldMatrix;
	u32   meshObjectId;
	u32   materialId;
	u32   _padding0;
	u32   _padding1;
};

#endif // __AX_HLSL_CPU_STRUCT_h__
