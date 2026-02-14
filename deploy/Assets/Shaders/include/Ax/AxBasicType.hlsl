#ifndef __AxBasicType_HLSL__
#define __AxBasicType_HLSL__

// typedef int16_t	i16;
typedef int			i32;
// typedef int64_t	i64;

// typedef uint16_t	u16;
typedef uint		u32;
// typedef uint64_t	u64;

typedef half		f16;
typedef float 		f32;
typedef double		f64;

typedef float2		Vec2f;
typedef float3		Vec3f;
typedef float4		Vec4f;

typedef float2x2	Mat2f;
typedef float3x3	Mat3f;
typedef float4x4	Mat4f;

typedef float3		Color3f;
typedef float4		Color4f;

typedef uint2		u32x2;
typedef uint3		u32x3;
typedef uint4		u32x4;

typedef int2		i32x2;
typedef int3		i32x3;
typedef int4		i32x4;

struct BBox3f {
	Vec3f min;
	Vec3f max;
};


// HLSL: resource : register(x, space) | DX: D3D12_SHADER_INPUT_BIND_DESC |  spirv-cross reflection
// HLSL: shader register               | DX: "BindPoint"                  |  Vulkan "binding"
// HLSL: register space                | DX: "Space"                      |  Vulkan "set"
#define AX_BindSpace_Default    space0
#define AX_BindSpace_World      space1
#define AX_BindSpace_Bindless   space2
#define AX_BindSpace_RootConst  space3

#include "AxBase_Dx12.hlsl"
#include "AxBase_Vk.hlsl"

#include "../../../../dev/AxLib8/AxRender/src/AxRender/HLSL/AX_HLSL_CPU_MACRO.h"
#include "../../../../dev/AxLib8/AxRender/src/AxRender/HLSL/AX_HLSL_CPU_STRUCT.h"

#ifdef AX_RENDER_VK
	#define AX_ROOT_CONST_STRUCT(TYPE, NAME) \
		[[vk::push_constant]] \
		ConstantBuffer<TYPE> NAME; \
	//----
#else
	#define AX_ROOT_CONST_STRUCT(TYPE, NAME) \
		ConstantBuffer<TYPE> NAME : register(b1, AX_BindSpace_RootConst); \
	//----
#endif


#ifdef AX_RENDER_BINDLESS
	SamplerState AxBindless_SamplerState[1000] : register(s1000,  AX_BindSpace_Bindless);
	Texture2D    AxBindless_Texture2D[10000]   : register(t10000, AX_BindSpace_Bindless);
	Texture3D    AxBindless_Texture3D[1000]    : register(t20000, AX_BindSpace_Bindless);
	
	// $Global UniformBuffer should be register(x, space0)

	#define Sampler_Texture2D(NAME)					uniform u32 AxSamplerState_##NAME; uniform u32 AxTexture2D_##NAME; 
	#define Sampler_Texture3D(NAME)					uniform u32 AxSamplerState_##NAME; uniform u32 AxTexture3D_##NAME; 

	#define Sampler_Texture2D_(NAME, REG, SPACE)	uniform u32 AxSamplerState_##NAME; uniform u32 AxTexture2D_##NAME; 
	#define Sampler_Texture3D_(NAME, REG, SPACE)	uniform u32 AxSamplerState_##NAME; uniform u32 AxTexture3D_##NAME; 

	#define tex2D(NAME, UV) AxBindless_Texture2D[AxTexture2D_##NAME].Sample(AxBindless_SamplerState[AxSamplerState_##NAME], UV)
	#define tex3D(NAME, UV) AxBindless_Texture3D[AxTexture3D_##NAME].Sample(AxBindless_SamplerState[AxSamplerState_##NAME], UV)

	#define tex2Dlod(NAME, UV, LOD) AxBindless_Texture2D[AxTexture2D_##NAME].SampleLevel(AxBindless_SamplerState[AxSamplerState_##NAME], UV, LOD)
	#define tex3Dlod(NAME, UV, LOD) AxBindless_Texture3D[AxTexture3D_##NAME].SampleLevel(AxBindless_SamplerState[AxSamplerState_##NAME], UV, LOD)

#else
	// HLSL can auto pick register, and "spirv-cross -fauto-bind-uniforms" can auto gen for SPIR-V
	#define Sampler_Texture2D(NAME) 				SamplerState AxSamplerState_##NAME; Texture2D NAME;
	#define Sampler_Texture3D(NAME) 				SamplerState AxSamplerState_##NAME; Texture3D NAME;

	#define Sampler_Texture2D_(NAME, REG, SPACE) 	SamplerState AxSamplerState_##NAME : register(s ## REG, SPACE); Texture2D NAME : register(t ## REG, SPACE);
	#define Sampler_Texture3D_(NAME, REG, SPACE) 	SamplerState AxSamplerState_##NAME : register(s ## REG, SPACE); Texture3D NAME : register(t ## REG, SPACE);

	#define tex2D(NAME, UV) NAME.Sample(AxSamplerState_##NAME, UV)
	#define tex3D(NAME, UV) NAME.Sample(AxSamplerState_##NAME, UV)

	#define tex2Dlod(NAME, UV, LOD) NAME.SampleLevel(AxSamplerState_##NAME, UV, LOD)
	#define tex3Dlod(NAME, UV, LOD) NAME.SampleLevel(AxSamplerState_##NAME, UV, LOD)

#endif // else AX_RENDER_BINDLESS

// those defines ensure variable name and semantic as pair, since OpenGL/Vulkan DO NOT have semantic but relie on variable name
#define SEM_pos(					TYPE)	TYPE	pos					: POSITION
#define SEM_worldPos(				TYPE)	TYPE	worldPos			: POSITION1
#define SEM_screenPos(				TYPE)	TYPE	screenPos			: POSITION2

#define SEM_sv_pos(					TYPE)	TYPE	sv_pos				: SV_POSITION
#define SEM_sv_depth(				TYPE)	TYPE	sv_depth			: SV_DEPTH
#define SEM_sv_coverage(			TYPE)	TYPE	sv_coverage			: SV_COVERAGE
#define SEM_sv_vertexId(			TYPE)	TYPE	sv_vertexId			: SV_VERTEXID
#define SEM_sv_primitiveId(			TYPE)	TYPE	sv_primitiveId		: SV_PRIMITIVEID
#define SEM_sv_instanceId(			TYPE)	TYPE	sv_instanceId		: SV_INSTANCEID
#define SEM_sv_gsInstanceId(		TYPE)	TYPE	sv_gsInstanceId		: SV_GSINSTANCEID

#define SEM_sv_dispatchThreadId(	TYPE)	TYPE	sv_dispatchThreadId	: SV_DISPATCHTHREADID
#define SEM_sv_groupId(				TYPE)	TYPE	sv_groupId			: SV_GROUPID
#define SEM_sv_groupIndex(			TYPE)	TYPE	sv_groupIndex		: SV_GROUPINDEX
#define SEM_sv_groupThreadId(		TYPE)	TYPE	sv_groupThreadId	: SV_GROUPTHREADID

#define SEM_dtid(					TYPE)	TYPE	dtid				: SV_DISPATCHTHREADID
#define SEM_gid(					TYPE)	TYPE	gid					: SV_GROUPID
#define SEM_gtid(					TYPE)	TYPE	gtid				: SV_GROUPTHREADID

#define SEM_color( TYPE)	TYPE	color		: COLOR
#define SEM_color0(TYPE)	TYPE	color0		: COLOR0
#define SEM_color1(TYPE)	TYPE	color1		: COLOR1
#define SEM_color2(TYPE)	TYPE	color2		: COLOR2
#define SEM_color3(TYPE)	TYPE	color3		: COLOR3
#define SEM_color4(TYPE)	TYPE	color4		: COLOR4
#define SEM_color5(TYPE)	TYPE	color5		: COLOR5
#define SEM_color6(TYPE)	TYPE	color6		: COLOR6
#define SEM_color7(TYPE)	TYPE	color7		: COLOR7

#define SEM_normal( TYPE)	TYPE	normal		: NORMAL
#define SEM_normal0(TYPE)	TYPE	normal0		: NORMAL0
#define SEM_normal1(TYPE)	TYPE	normal1		: NORMAL1
#define SEM_normal2(TYPE)	TYPE	normal2		: NORMAL2
#define SEM_normal3(TYPE)	TYPE	normal3		: NORMAL3
#define SEM_normal4(TYPE)	TYPE	normal4		: NORMAL4
#define SEM_normal5(TYPE)	TYPE	normal5		: NORMAL5
#define SEM_normal6(TYPE)	TYPE	normal6		: NORMAL6
#define SEM_normal7(TYPE)	TYPE	normal7		: NORMAL7

#define SEM_tangent( TYPE)	TYPE	tangent		: TANGENT
#define SEM_tangent0(TYPE)	TYPE	tangent0	: TANGENT0
#define SEM_tangent1(TYPE)	TYPE	tangent1	: TANGENT1
#define SEM_tangent2(TYPE)	TYPE	tangent2	: TANGENT2
#define SEM_tangent3(TYPE)	TYPE	tangent3	: TANGENT3
#define SEM_tangent4(TYPE)	TYPE	tangent4	: TANGENT4
#define SEM_tangent5(TYPE)	TYPE	tangent5	: TANGENT5
#define SEM_tangent6(TYPE)	TYPE	tangent6	: TANGENT6
#define SEM_tangent7(TYPE)	TYPE	tangent7	: TANGENT7

#define SEM_binormal( TYPE)	TYPE	binormal	: BINORMAL
#define SEM_binormal0(TYPE)	TYPE	binormal0	: BINORMAL0
#define SEM_binormal1(TYPE)	TYPE	binormal1	: BINORMAL1
#define SEM_binormal2(TYPE)	TYPE	binormal2	: BINORMAL2
#define SEM_binormal3(TYPE)	TYPE	binormal3	: BINORMAL3
#define SEM_binormal4(TYPE)	TYPE	binormal4	: BINORMAL4
#define SEM_binormal5(TYPE)	TYPE	binormal5	: BINORMAL5
#define SEM_binormal6(TYPE)	TYPE	binormal6	: BINORMAL6
#define SEM_binormal7(TYPE)	TYPE	binormal7	: BINORMAL7

#define SEM_uv( TYPE)		TYPE	uv		: TEXCOORD
#define SEM_uv0(TYPE)		TYPE	uv0		: TEXCOORD0
#define SEM_uv1(TYPE)		TYPE	uv1		: TEXCOORD1
#define SEM_uv2(TYPE)		TYPE	uv2		: TEXCOORD2
#define SEM_uv3(TYPE)		TYPE	uv3		: TEXCOORD3
#define SEM_uv4(TYPE)		TYPE	uv4		: TEXCOORD4
#define SEM_uv5(TYPE)		TYPE	uv5		: TEXCOORD5
#define SEM_uv6(TYPE)		TYPE	uv6		: TEXCOORD6
#define SEM_uv7(TYPE)		TYPE	uv7		: TEXCOORD7
#define SEM_uv8(TYPE)		TYPE	uv8		: TEXCOORD8
#define SEM_uv9(TYPE)		TYPE	uv9		: TEXCOORD9

#define SEM_uv10(TYPE)		TYPE	uv10	: TEXCOORD10
#define SEM_uv11(TYPE)		TYPE	uv11	: TEXCOORD11
#define SEM_uv12(TYPE)		TYPE	uv12	: TEXCOORD12
#define SEM_uv13(TYPE)		TYPE	uv13	: TEXCOORD13
#define SEM_uv14(TYPE)		TYPE	uv14	: TEXCOORD14
#define SEM_uv15(TYPE)		TYPE	uv15	: TEXCOORD15
#define SEM_uv16(TYPE)		TYPE	uv16	: TEXCOORD16
#define SEM_uv17(TYPE)		TYPE	uv17	: TEXCOORD17
#define SEM_uv18(TYPE)		TYPE	uv18	: TEXCOORD18
#define SEM_uv19(TYPE)		TYPE	uv19	: TEXCOORD19

#define AX_DEBUG_COLOR_TABLE_SIZE 27
static const float4 axDebugColorTable[AX_DEBUG_COLOR_TABLE_SIZE] = {
    float4(1.0, 0.0, 0.0, 1), // Red
    float4(0.0, 1.0, 0.0, 1), // Green
    float4(1.0, 1.0, 0.0, 1),
    float4(0.0, 0.0, 1.0, 1),  // Blue
    float4(1.0, 0.0, 1.0, 1),
    float4(0.0, 1.0, 1.0, 1),
    float4(1.0, 1.0, 1.0, 1),

    float4(0.5, 0.0, 0.0, 1),
    float4(0.0, 0.5, 0.0, 1),
    float4(0.5, 0.5, 0.0, 1),
    float4(0.0, 0.0, 0.5, 1),
    float4(0.5, 0.0, 0.5, 1),
    float4(0.0, 0.5, 0.5, 1),
    float4(0.5, 0.5, 0.5, 1),

    float4(1.0, 0.5, 0.0, 1),
    float4(1.0, 0.0, 0.5, 1),
    float4(0.0, 1.0, 0.5, 1),

    float4(0.5, 1.0, 0.0, 1),
    float4(0.5, 0.0, 1.0, 1),
    float4(0.0, 0.5, 1.0, 1),

    float4(0.5, 0.5, 0.5, 1),
    float4(1.0, 0.5, 0.5, 1),
    float4(0.5, 1.0, 0.5, 1),
    float4(1.0, 1.0, 0.5, 1),
    float4(0.5, 0.5, 1.0, 1),
    float4(1.0, 0.5, 1.0, 1),
    float4(0.5, 1.0, 1.0, 1),
};

Color4f ax_debug_color(uint i) { return axDebugColorTable[i % AX_DEBUG_COLOR_TABLE_SIZE]; }

uint ax_Color4f_to_u32(float4 value) {
	uint4 uvalue = uint4(value * 255.0 + 0.5);
	return (uvalue.a << 24) | (uvalue.b << 16) | (uvalue.g << 8) | uvalue.r; 
}

float4 ax_u32_to_Color4f(uint packed) {
	uint ri = (packed      ) & 0xff;
	uint gi = (packed >> 8 ) & 0xff;
	uint bi = (packed >> 16) & 0xff;
	uint ai = (packed >> 24);
	return float4(ri, gi, bi, ai) / 255.0;
}

float2 ax_unpack_uv_u32(u32 packed) {
	const float kMax = 4.0f;
	float x = (packed >> 16   ) / (65535.0 / kMax);
	float y = (packed & 0xffff) / (65535.0 / kMax);
	return float2(x,y);
}

uint ax_align_div_to(uint value, uint alignment) {
	return (value + (alignment - 1)) / alignment; 
}

uint ax_align_to(uint value, uint alignment) {
	return (value + (alignment - 1)) / alignment * alignment;
}

	
u32x3 ax_unpack_tri_indices(u32 packed) {
	return u32x3((packed >> 16) & 0xFF,
				 (packed >> 8 ) & 0xFF,
				 (packed      ) & 0xFF);
}
	
u32 ax_pack_tri_indices(u32 v0, u32 v1, u32 v2) {
	return (v0 << 16) | (v1 << 8) | v2;
}

float2 ax_sign_not_zero(float2 v) {
	return float2(	v.x >= 0 ? 1 : -1,
					v.y >= 0 ? 1 : -1);
}

float3 ax_unpack_normal_xy(float2 packedNormal) {
    float2 xy = packedNormal * 2.0 - 1.0;
    float z = sqrt(saturate(1.0 - dot(xy, xy)));
    return float3(xy, z);
}

float2 ax_octahedral_wrap(float2 v) {
    return (1.0 - abs(v.yx)) * ax_sign_not_zero(v.xy);
}
 
float2 ax_pack_normal_octahedral(float3 n) {
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = n.z >= 0.0 ? n.xy : ax_octahedral_wrap(n.xy);
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}
 
float3 ax_unpack_normal_octahedral(float2 f) {
    f = f * 2.0 - 1.0;
    // https://twitter.com/Stubbesaurus/status/937994790553227264
    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += ax_sign_not_zero(n.xy) * t;
    return normalize(n);
}


#endif // __AxBasicType_HLSL__
