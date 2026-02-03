#ifndef __AX_HLSL_CPU_STRUCT_h__
#define __AX_HLSL_CPU_STRUCT_h__

struct AxMeshletDraw {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;
};

struct AxMeshlet {
	AxMeshletDraw draw;
	// AABB
};


struct AxMeshletVert {
	Vec3f   pos;
	u32     rawColor;
	Vec2f   uv0;
	Vec2f   uv1;
//	Vec2f   rawNormal;
	Vec3f   normal;
	u32 unused0;
	u32 unused1;
	u32 unused2;
	u32 unused3;
	u32 unused4;
};

struct AxMeshletPrim {
	u32x3 	tri;
	u32 	_unused0;
};


struct AxMeshInfo {
	u32 meshletOffset;
	u32 meshletCount;
	u32 totalVertCount;
	u32 totalPrimCount;
};

#endif // __AX_HLSL_CPU_STRUCT_h__
