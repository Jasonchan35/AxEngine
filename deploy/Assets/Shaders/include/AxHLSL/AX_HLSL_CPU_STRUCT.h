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
	u32x4 _usedPadding[3];
};

struct AxGpuMeshletDraw {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;
};

struct AxGpuMeshlet {
	AxGpuMeshletDraw draw;
	// AABB
};

struct AxGpuMeshletVert {
	Vec3f   pos;
	u32     rawColor;
	Vec2f   uv0;
	Vec2f   uv1;
//	Vec2f   rawNormal;
	Vec3f   normal;
	u32 _padding[5];
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
};

struct AxGpuMeshObjectRenderer {
	Mat4f	worldMatrix;
	u32 	meshObjectId;
	u32 	materialId;
	u32     _padding0;
	u32     _padding1;
	u32x4 	_padding[3];
};

#ifdef AX_RENDER_DX12
struct AxIndirectDrawWorld_Dx12 {
	AxMeshShaderDrawRootConst 	rootConst;
	D3D12_DISPATCH_ARGUMENTS	args;
	u32x4 	_padding[7];
};

struct AxIndirectDrawWorldRootConst_Dx12 {
	u32 worldId;
}; 

#endif


#endif // __AX_HLSL_CPU_STRUCT_h__
