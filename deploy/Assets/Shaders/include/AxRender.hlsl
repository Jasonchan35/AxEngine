#ifndef __ax_common_hlsl__
#define __ax_common_hlsl__

typedef int		i32;
typedef uint	u32;

typedef half	f16;
typedef half2	f16x2;
typedef half3	f16x3;
typedef half4	f16x4;

typedef float  	f32;
typedef float2 	f32x2;
typedef float3 	f32x3;
typedef float4 	f32x4;

typedef double	f64;
typedef double2	f64x2;
typedef double3	f64x3;
typedef double4	f64x4;

typedef float3 	Color3f;
typedef float4 	Color4f;

// HLSL: resource : register(x, space) | DX: D3D12_SHADER_INPUT_BIND_DESC |  spirv-cross reflection
// HLSL: shader register               | DX: "BindPoint"                  |  Vulkan "binding"
// HLSL: register space                | DX: "Space"                      |  Vulkan "set"
#define AX_BindSpace_Default		space0
#define AX_BindSpace_World   		space1
#define AX_BindSpace_Object 		space2
#define AX_BindSpace_Bindless		space3

#define f32   float
#define Vec4f float4
#define Mat4f float4x4

cbuffer AX_ConstBuffer_World : register(b0, AX_BindSpace_World) {
	float	ax_g_test;

	float	ax_g_time;
	Vec4f	ax_g_timeSin;
	Vec4f	ax_g_timeSlowSin;
	Vec4f	ax_g_deltaTime;
	Vec4f	ax_g_worldSpaceCameraPos;
	Mat4f	ax_g_cameraProjection;
	Mat4f	ax_g_cameraInvProjection;
}

cbuffer AX_ConstBuffer_Object : register(b0, AX_BindSpace_Object) {
	Mat4f	ax_object_mvp;
	Mat4f	ax_object_mv;
	Mat4f	ax_object_mv_t;
	Mat4f	ax_object_mv_it;
	Mat4f	ax_object_v;
	Mat4f	ax_object_vp;
	Mat4f	ax_object_m;		// Object To World
	Mat4f	ax_object_m_i;		// world To Object
}

struct AxObjectData {
	Mat4f objectToWorld;
};
StructuredBuffer<AxObjectData> gAxObjectData : register(t0, AX_BindSpace_Object);

#if AX_RENDER_BINDLESS
	SamplerState AxBindless_SamplerState[1000] : register(s0,     AX_BindSpace_Bindless);
	Texture2D    AxBindless_Texture2D[10000]   : register(t10000, AX_BindSpace_Bindless);
	Texture3D    AxBindless_Texture3D[10000]   : register(t20000, AX_BindSpace_Bindless);
	
	// $Global UniformBuffer should be register(x, space0)

	#define Sampler_Texture2D(NAME)					u32 AxSamplerState_##NAME; u32 AxTexture2D_##NAME; 
	#define Sampler_Texture3D(NAME)					u32 AxSamplerState_##NAME; u32 AxTexture3D_##NAME; 

	#define Sampler_Texture2D_(NAME, REG, SPACE)	u32 AxSamplerState_##NAME; u32 AxTexture2D_##NAME; 
	#define Sampler_Texture3D_(NAME, REG, SPACE)	u32 AxSamplerState_##NAME; u32 AxTexture3D_##NAME; 

	#define tex2D(NAME, UV) AxBindless_Texture2D[AxTexture2D_##NAME].Sample(AxBindless_SamplerState[AxSamplerState_##NAME], UV)
	#define tex3D(NAME, UV) AxBindless_Texture3D[AxTexture3D_##NAME].Sample(AxBindless_SamplerState[AxSamplerState_##NAME], UV)

	#define tex2Dlod(NAME, UV, LOD) AxBindless_Texture2D[AxTexture2D_##NAME].SampleLevel(AxBindless_SamplerState[AxSamplerState_##NAME], UV, LOD)
	#define tex3Dlod(NAME, UV, LOD) AxBindless_Texture3D[AxTexture3D_##NAME].SampleLevel(AxBindless_SamplerState[AxSamplerState_##NAME], UV, LOD)

	AxObjectData getAxObjectData() { return gAxObjectData[0]; }

#else
	// HLSL can auto pick register, and "spirv-cross -fauto-bind-uniforms" can auto gen for SPIR-V
	#define Sampler_Texture2D(NAME) 				SamplerState AxSamplerState_##NAME; Texture2D NAME;
	#define Sampler_Texture3D(NAME) 				SamplerState AxSamplerState_##NAME; Texture3D NAME;

	#define Sampler_Texture2D_(NAME, REG, SPACE) 	SamplerState AxSamplerState_##NAME : register(s ## REG, SPACE); Texture2D NAME : register(t ## REG, SPACE);
	#define Sampler_Texture3D_(NAME, REG, SPACE) 	SamplerState AxSamplerState_##NAME : register(s ## REG, SPACE); Texture3D NAME : register(t ## REG, SPACE);

	#define tex2D(NAME, UV) NAME.Sample(AxSamplerState_##NAME, UV)
	#define tex3D(NAME, UV) NAME.Sample(AxSamplerState_##NAME, UV)

	AxObjectData getAxObjectData() { return gAxObjectData[0]; }

#endif // else AX_RENDER_BINDLESS

// those defines ensure variable name and semantic as pair, since OpenGL/Vulkan DO NOT have semantic but relie on variable name
#define sem_pos(					TYPE)	TYPE	pos					: POSITION;

#define sem_sv_pos(					TYPE)	TYPE	sv_pos				: SV_POSITION;
#define sem_sv_depth(				TYPE)	TYPE	sv_depth			: SV_DEPTH;
#define sem_sv_coverage(			TYPE)	TYPE	sv_coverage			: SV_COVERAGE;
#define sem_sv_vertexId(			TYPE)	TYPE	sv_vertexId			: SV_VERTEXID;
#define sem_sv_primitiveId(			TYPE)	TYPE	sv_primitiveId		: SV_PRIMITIVEID;
#define sem_sv_instanceId(			TYPE)	TYPE	sv_instanceId		: SV_INSTANCEID;
#define sem_sv_dispatchThreadId(	TYPE)	TYPE	sv_dispatchThreadId	: SV_DISPATCHTHREADID;
#define sem_sv_groupId(				TYPE)	TYPE	sv_groupId			: SV_GROUPID;
#define sem_sv_groupIndex(			TYPE)	TYPE	sv_groupIndex		: SV_GROUPINDEX;
#define sem_sv_groupThreadId(		TYPE)	TYPE	sv_groupThreadId	: SV_GROUPTHREADID;
#define sem_sv_gsInstanceId(		TYPE)	TYPE	sv_gsInstanceId		: SV_GSINSTANCEID;

#define sem_color0(TYPE)	TYPE	color0		: COLOR0;
#define sem_color1(TYPE)	TYPE	color1		: COLOR1;
#define sem_color2(TYPE)	TYPE	color2		: COLOR2;
#define sem_color3(TYPE)	TYPE	color3		: COLOR3;
#define sem_color4(TYPE)	TYPE	color4		: COLOR4;
#define sem_color5(TYPE)	TYPE	color5		: COLOR5;
#define sem_color6(TYPE)	TYPE	color6		: COLOR6;
#define sem_color7(TYPE)	TYPE	color7		: COLOR7;

#define sem_normal0(TYPE)	TYPE	normal0		: NORMAL0;
#define sem_normal1(TYPE)	TYPE	normal1		: NORMAL1;
#define sem_normal2(TYPE)	TYPE	normal2		: NORMAL2;
#define sem_normal3(TYPE)	TYPE	normal3		: NORMAL3;
#define sem_normal4(TYPE)	TYPE	normal4		: NORMAL4;
#define sem_normal5(TYPE)	TYPE	normal5		: NORMAL5;
#define sem_normal6(TYPE)	TYPE	normal6		: NORMAL6;
#define sem_normal7(TYPE)	TYPE	normal7		: NORMAL7;

#define sem_tangent0(TYPE)	TYPE	tangent0	: TANGENT0;
#define sem_tangent1(TYPE)	TYPE	tangent1	: TANGENT1;
#define sem_tangent2(TYPE)	TYPE	tangent2	: TANGENT2;
#define sem_tangent3(TYPE)	TYPE	tangent3	: TANGENT3;
#define sem_tangent4(TYPE)	TYPE	tangent4	: TANGENT4;
#define sem_tangent5(TYPE)	TYPE	tangent5	: TANGENT5;
#define sem_tangent6(TYPE)	TYPE	tangent6	: TANGENT6;
#define sem_tangent7(TYPE)	TYPE	tangent7	: TANGENT7;

#define sem_binormal0(TYPE)	TYPE	binormal0	: BINORMAL0;
#define sem_binormal1(TYPE)	TYPE	binormal1	: BINORMAL1;
#define sem_binormal2(TYPE)	TYPE	binormal2	: BINORMAL2;
#define sem_binormal3(TYPE)	TYPE	binormal3	: BINORMAL3;
#define sem_binormal4(TYPE)	TYPE	binormal4	: BINORMAL4;
#define sem_binormal5(TYPE)	TYPE	binormal5	: BINORMAL5;
#define sem_binormal6(TYPE)	TYPE	binormal6	: BINORMAL6;
#define sem_binormal7(TYPE)	TYPE	binormal7	: BINORMAL7;

#define sem_uv0(TYPE)		TYPE	uv0		: TEXCOORD0;
#define sem_uv1(TYPE)		TYPE	uv1		: TEXCOORD1;
#define sem_uv2(TYPE)		TYPE	uv2		: TEXCOORD2;
#define sem_uv3(TYPE)		TYPE	uv3		: TEXCOORD3;
#define sem_uv4(TYPE)		TYPE	uv4		: TEXCOORD4;
#define sem_uv5(TYPE)		TYPE	uv5		: TEXCOORD5;
#define sem_uv6(TYPE)		TYPE	uv6		: TEXCOORD6;
#define sem_uv7(TYPE)		TYPE	uv7		: TEXCOORD7;
#define sem_uv8(TYPE)		TYPE	uv8		: TEXCOORD8;
#define sem_uv9(TYPE)		TYPE	uv9		: TEXCOORD9;

#define sem_uv10(TYPE)		TYPE	uv10	: TEXCOORD10;
#define sem_uv11(TYPE)		TYPE	uv11	: TEXCOORD11;
#define sem_uv12(TYPE)		TYPE	uv12	: TEXCOORD12;
#define sem_uv13(TYPE)		TYPE	uv13	: TEXCOORD13;
#define sem_uv14(TYPE)		TYPE	uv14	: TEXCOORD14;
#define sem_uv15(TYPE)		TYPE	uv15	: TEXCOORD15;
#define sem_uv16(TYPE)		TYPE	uv16	: TEXCOORD16;
#define sem_uv17(TYPE)		TYPE	uv17	: TEXCOORD17;
#define sem_uv18(TYPE)		TYPE	uv18	: TEXCOORD18;
#define sem_uv19(TYPE)		TYPE	uv19	: TEXCOORD19;

#endif // __ax_common_hlsl__