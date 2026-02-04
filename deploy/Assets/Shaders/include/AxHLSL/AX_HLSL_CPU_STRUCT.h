#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

struct AxVertexShaderDrawRootConst {
	Mat4f worldMatrix;
};

struct AxMeshShaderDrawRootConst {
	Mat4f worldMatrix;
	u32   meshId;
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
	u32 	_padding[1];
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
	u32 	_padding[16-2];
};

// #ifdef AX_RENDER_DX12
// struct AxIndirectDrawArgument_Dx12 {
// 	D3D12_DISPATCH_ARGUMENTS	args;
// 	AxMeshShaderDrawRootConst 	rootConst;
// };
// #endif

#endif // __AX_HLSL_CPU_STRUCT_h__
