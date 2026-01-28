#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

struct AxMeshlet {
	uint vertCount;
	uint vertOffset;
	uint primCount;
	uint primOffset;
};
RWStructuredBuffer<AxMeshlet> axMeshlet : register(u1000, AX_BindSpace_World);

struct AxMeshlet_Vertex {
	Vec3f   pos;
	u32     rawColor;
	Vec2f   uv0;
	Vec2f   uv1;
	Vec2f   rawNormal;

	float4  color() { return ax_u32_to_Color4f(rawColor); }
};
RWStructuredBuffer<AxMeshlet_Vertex> axMeshlet_vertices : register(u1001, AX_BindSpace_World);



#endif //  __AxMeshlet_hlsl__
