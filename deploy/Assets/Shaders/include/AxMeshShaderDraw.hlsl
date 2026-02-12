#ifndef __AxMeshShaderDraw_hlsl__
#define __AxMeshShaderDraw_hlsl__

#include "Ax/AxWorld.hlsl"

AX_ROOT_CONST_STRUCT(AxMeshShaderDrawRootConst, axMeshShaderDrawRootConst) 
#define AX_MATRIX_M				axMeshShaderDrawRootConst.worldMatrix
#define AX_MESH_OBJECT_ID		axMeshShaderDrawRootConst.meshObjectId
#define AX_MATRIX_VP			axCamera.viewProjMatrix

Vec3f axObjectToWorldPos(Vec4f inPos) { return mul(AX_MATRIX_M,  inPos).xyz; }
Vec4f axWorldToClipPos  (Vec4f inPos) { return mul(AX_MATRIX_VP, inPos); }
Vec4f axObjectToClipPos (Vec4f inPos) { return mul(AX_MATRIX_VP, mul(AX_MATRIX_M, inPos)); }

Vec3f axObjectToWorldPos(Vec3f inPos) { return axObjectToWorldPos(Vec4f(inPos, 1)); }
Vec4f axWorldToClipPos  (Vec3f inPos) { return axWorldToClipPos  (Vec4f(inPos, 1)); }
Vec4f axObjectToClipPos (Vec3f inPos) { return axObjectToClipPos (Vec4f(inPos, 1)); }

Vec3f axObjectToWorldNormal(Vec3f inNormal) { return mul(transpose((Mat3f)AX_MATRIX_M  ), inNormal); }
Vec3f axWorldToClipNormal  (Vec3f inNormal) { return mul(transpose((Mat3f)AX_MATRIX_VP ), inNormal); }
Vec3f axObjectToClipNormal (Vec3f inNormal) { return axWorldToClipNormal(axObjectToWorldNormal(inNormal)); }

#define AS_GROUP_SIZE AX_HLSL_THREADS_PER_WAVE
#define MS_GROUP_SIZE ROUNDUP(MAX(AX_HLSL_MESH_SHADER_MAX_VERT_COUNT, AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT), AX_HLSL_THREADS_PER_WAVE)

struct Payload {
	u32 meshObjectId;
	u32 meshletCount;
	u32 meshletIds[AS_GROUP_SIZE];
};

groupshared Payload s_payload;

#define MAX(x, y) (x > y ? x : y)
#define ROUNDUP(x, y) ((x + y - 1) & ~(y - 1))

struct VS_Input {
	SEM_pos(float4);
	SEM_color0(float4);
	SEM_normal(float3);
};

struct VS_Output {
	SEM_sv_pos(float4);
	SEM_color0(float4);
	SEM_normal(float3);
	SEM_worldPos(float3);
	SEM_uv0(float2);
};

struct VS_PrimId_Out {
	uint primitiveId : SV_PrimitiveID;
};

static float boundsError(AxGpuMeshletGroup group) {
	float camProj = 1.f / tan( radians(axCamera.fieldOfView * 0.5));
	float camDistance = distance(axObjectToWorldPos(group.center), axCamera.worldPos);
	float d = camDistance - group.radius;
	float viewportH = axCamera.viewportMax.y - axCamera.viewportMin.y;
	return viewportH * group.clusterError / (d > axCamera.nearClip ? d : axCamera.nearClip) * (camProj * 0.5f);
}

[numthreads(AS_GROUP_SIZE, 1, 1)]
void axMeshlet_AmplificationMain(
	SEM_gid(u32),
	SEM_gtid(u32),
	SEM_dtid(u32)
) {
	AxGpuMeshObject meshObject = axGpuMeshObject[AX_MESH_OBJECT_ID];

	if (gtid == 0) {
		s_payload.meshObjectId = AX_MESH_OBJECT_ID;
	}

	u32 meshletOffset = meshObject.meshletOffset;
	u32 meshletCount  = meshObject.meshletCount;
	u32 groupOffset   = meshObject.meshletGroupOffset;

	bool visibleResult = false;

	if (dtid < meshletCount) {
		u32 meshletId = meshletOffset + dtid;
		AxGpuMeshlet meshlet = axGpuMeshlet[meshletId];

		Vec3f camPos           = axCamera.worldPos;
		float camTanFov        = tan(radians(axCamera.fieldOfView * 0.5)) * 2.0;
		float maxErrorInPixels = axCamera.maxMeshletErrorInPixels;

		// when requesting DAG cut from a viewpoint, we need to check if each cluster is the least detailed cluster that passes the error threshold
		bool lodMatch = (meshlet.refinedGroupId < 0 || boundsError(axGpuMeshletGroup[groupOffset + meshlet.refinedGroupId]) <= maxErrorInPixels) 
					 && (                              boundsError(axGpuMeshletGroup[groupOffset + meshlet.groupId       ]) >  maxErrorInPixels);

		bool insideFrustum = true;
		bool coneCulling   = true; 

		visibleResult = lodMatch && coneCulling && insideFrustum;

		if (axDebug.showAllLodDistance > 0) {
			visibleResult = true;
		}

		if (visibleResult) {
			uint outIndex = WavePrefixCountBits(visibleResult);
			s_payload.meshletIds[outIndex] = meshletId;
		}
	}

	uint visibleResultCount = WaveActiveCountBits(visibleResult);
	DispatchMesh(visibleResultCount, 1, 1, s_payload);
}


[numthreads(MS_GROUP_SIZE, 1, 1)]
[outputtopology("triangle")]
void axMeshlet_MeshMain(
	SEM_gid(u32),
	SEM_gtid(u32),
	SEM_dtid(u32),
	in payload Payload payload,
	out vertices   VS_Output     outVert[AX_HLSL_MESH_SHADER_MAX_VERT_COUNT],
	out indices    u32x3         outPrim[AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT],
	out primitives VS_PrimId_Out outPrimId[AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT]
) {
	u32 meshletId = payload.meshletIds[gid];
	AxGpuMeshlet meshlet = axGpuMeshlet[meshletId];

	SetMeshOutputCounts(meshlet.vertCount, meshlet.primCount);

	if (gtid < meshlet.primCount) {
		outPrim[gtid] = axGpuMeshletPrim[meshlet.primOffset + gtid].tri;
		outPrimId[gtid].primitiveId = gtid;
	}

	if (gtid < meshlet.vertCount) {
		VS_Input i;

		AxGpuMeshletVert mv = axGpuMeshletVert[meshlet.vertOffset + gtid];
		i.pos    = Vec4f(mv.pos, 1);

		if (axDebug.showAllLodDistance > 0) {
			i.pos = Vec4f(mv.pos + ax_debug_lod_offset(meshlet.lod), 1); // debug
		}

		i.color0 = float4(1,1,1,1);
		i.normal = mv.normal;

		switch (axDebug.debugColorCode) {
			case AxGpuDebugColorCode_Meshlet:             i.color0 = ax_debug_color(meshletId             ); break;
			case AxGpuDebugColorCode_MeshletGroup:        i.color0 = ax_debug_color(meshlet.groupId       ); break;
			case AxGpuDebugColorCode_MeshletRefinedGroup: i.color0 = ax_debug_color(meshlet.refinedGroupId); break;
			case AxGpuDebugColorCode_MeshletLod:          i.color0 = ax_debug_color(meshlet.lod           ); break;
		}

		VS_Output o;
		o.worldPos = axObjectToWorldPos(i.pos);
		o.sv_pos   = axObjectToClipPos(i.pos);
		o.normal   = axObjectToClipNormal(i.normal);
		o.color0   = i.color0;

		outVert[gtid] = o;
	}
}



#endif // __AxMeshShaderDraw_hlsl__