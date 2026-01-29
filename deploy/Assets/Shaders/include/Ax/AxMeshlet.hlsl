#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

struct AxMeshlet {
	u32 vertCount;
	u32 vertOffset;
	u32 primCount;
	u32 primOffset;
};
StructuredBuffer<AxMeshlet> axMeshlet : register(t200, AX_BindSpace_World);

struct AxMeshletVert {
	Vec3f   pos;
	u32     rawColor;
	Vec2f   uv0;
	Vec2f   uv1;
//	Vec2f   rawNormal;
	Vec3f   normal;

	float4  color() { return ax_u32_to_Color4f(rawColor); }
};
StructuredBuffer<AxMeshletVert> axMeshletVert : register(t201, AX_BindSpace_World);

struct AxMeshletPrim {
	uint3 tri;
};
StructuredBuffer<AxMeshletPrim> axMeshletPrim : register(t202, AX_BindSpace_World);


#endif //  __AxMeshlet_hlsl__
