#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

// HLSL alignment is 16

struct AxVertexShaderDrawRootConst {
	Mat4f worldMatrix;
};

struct AxMeshShaderDrawRootConst {
	Mat4f worldMatrix;
	u32   meshLodGroupId;
	u32   meshRendererId;
	u32   _padding0;
	u32   _padding1;
};

struct AxGpuMeshletDraw {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;
	
	// TODO move to AxGpuMeshlet
	u32 meshObjectId;
	u32 lod;
	u32 refined;
	u32 _padding2;
	
};

struct AxGpuMeshlet {
	AxGpuMeshletDraw draw;
	// AABB
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

struct AxGpuMeshLodGroup {
	u32 meshletOffset;
	u32 meshletCount;
	u32 totalVertCount;
	u32 totalPrimCount;
};

struct AxGpuMeshObject {
	BBox3f bounds;
	u32 vertOffset;
	u32 primOffset;
	
	u32 meshletOffset;
	u32 meshletCount;
	
	u32 lodGroupOffset;
	u32 lodGroupCount;
};

struct AxGpuMeshObjectRenderer {
	Mat4f	worldMatrix;
	u32 	meshObjectId;
	u32 	materialId;
	u32     _padding0;
	u32     _padding1;
};

#endif // __AX_HLSL_CPU_STRUCT_h__
