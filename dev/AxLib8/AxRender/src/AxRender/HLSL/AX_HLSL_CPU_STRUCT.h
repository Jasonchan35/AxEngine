#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

// HLSL alignment is 16

struct AxVertexShaderDrawRootConst {
	Mat4f worldMatrix;
};

struct AxMeshShaderDrawRootConst {
	Mat4f worldMatrix;
	u32   meshId;
	u32   meshRendererId;
	u32   _padding0;
	u32   _padding1;
};

struct AxGpuMeshletDraw {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;
	
	u32 lod;
	u32 _padding0;
	u32 _padding1;
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


struct AxGpuMeshObject {
	u32 meshletOffset;
	u32 meshletCount;
	u32 totalVertCount;
	u32 totalPrimCount;
	BBox3f bounds;
	u32 _padding0;
	u32 _padding1;
};

struct AxGpuMeshObjectRenderer {
	Mat4f	worldMatrix;
	u32 	meshObjectId;
	u32 	materialId;
	u32     _padding0;
	u32     _padding1;
};

#endif // __AX_HLSL_CPU_STRUCT_h__
