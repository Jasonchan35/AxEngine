#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

struct AxMeshlet {
	u32 vertOffset;
	u32 vertCount;
	u32 primOffset;
	u32 primCount;
};
StructuredBuffer<AxMeshlet> axMeshlet : register(t200, AX_BindSpace_World);

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

	float4  color() { return ax_u32_to_Color4f(rawColor); }
};
StructuredBuffer<AxMeshletVert> axMeshletVert : register(t201, AX_BindSpace_World);

struct AxMeshletPrim {
	uint3 tri;
	uint padding;
};
StructuredBuffer<AxMeshletPrim> axMeshletPrim : register(t202, AX_BindSpace_World);


#endif //  __AxMeshlet_hlsl__
