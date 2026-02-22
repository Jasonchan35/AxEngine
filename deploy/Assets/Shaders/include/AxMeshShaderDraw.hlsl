#ifndef __AxMeshShaderDraw_hlsl__
#define __AxMeshShaderDraw_hlsl__

#include "Ax/AxWorld.hlsl"

AX_ROOT_CONST_STRUCT(AxMeshShaderDraw_RootConst, axMeshShaderDraw_RootConst) 
#define AX_MATRIX_M			axMeshShaderDraw_RootConst.worldMatrix
#define AX_MESH_OBJECT_ID	axMeshShaderDraw_RootConst.meshObjectId
#define AX_MATRIX_VP		axCamera.viewProjMatrix

Vec3f axObjectToWorldPos(Vec4f inPos) { return mul(AX_MATRIX_M,  inPos).xyz; }
Vec4f axWorldToClipPos  (Vec4f inPos) { return mul(AX_MATRIX_VP, inPos); }
Vec4f axObjectToClipPos (Vec4f inPos) { return mul(AX_MATRIX_VP, mul(AX_MATRIX_M, inPos)); }

Vec3f axObjectToWorldPos(Vec3f inPos) { return axObjectToWorldPos(Vec4f(inPos, 1)); }
Vec4f axWorldToClipPos  (Vec3f inPos) { return axWorldToClipPos  (Vec4f(inPos, 1)); }
Vec4f axObjectToClipPos (Vec3f inPos) { return axObjectToClipPos (Vec4f(inPos, 1)); }

Vec3f axObjectToWorldNormal(Vec3f inNormal) { return mul((Mat3f)AX_MATRIX_M , inNormal); }
Vec3f axWorldToClipNormal  (Vec3f inNormal) { return mul((Mat3f)AX_MATRIX_VP, inNormal); }
Vec3f axObjectToClipNormal (Vec3f inNormal) { return axWorldToClipNormal(axObjectToWorldNormal(inNormal)); }

#define AS_GROUP_SIZE AX_HLSL_THREADS_PER_WAVE
#define MS_GROUP_SIZE ROUNDUP(MAX(AX_HLSL_MESH_SHADER_MAX_VERT_COUNT, AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT), AX_HLSL_THREADS_PER_WAVE)

struct Payload {
	u32 meshObjectId;
	u32 clusterCount;
	u32 meshletIds[AS_GROUP_SIZE];
};

groupshared Payload s_payload;

#define MAX(x, y) (x > y ? x : y)
#define ROUNDUP(x, y) ((x + y - 1) & ~(y - 1))

struct MeshletVS_Input {
	SEM_pos(float3);
	SEM_color0(float4);
	SEM_normal(float3);
	SEM_uv0(float2);
	SEM_uv1(float2);
};

struct MeshletVS_Output {
	SEM_sv_pos(float4);
	SEM_color0(float4);
	SEM_normal(float3);
	SEM_worldPos(float3);
	SEM_uv0(float2);
	SEM_uv1(float2);
};

struct MeshletVS_PrimId_Out {
	uint primitiveId : SV_PrimitiveID;
};

static Vec3f axMeshlet_clusterDebug(AxGpuData_MeshletCluster cluster) {
	AxGpuData_MeshObject meshObject = axGpuData_MeshObject[cluster.meshObjectId];
	Vec3f meshCenter = (meshObject.boundsMin + meshObject.boundsMax) / 2;
	Vec3f clusterVec = cluster.center - meshCenter;
	float dis = length(clusterVec);
	Vec3f clusterDir = abs(dis) > 0.001f ? clusterVec / dis : Vec3f(0,0,0);
	return clusterDir * axDebug.drawClusterOffset;
}

static float axMeshlet_boundsError(AxGpuData_MeshletGroup group) {
	float camProj = 1.f / tan( radians(axCamera.fieldOfView * 0.5));
	float camDistance = distance(axObjectToWorldPos(group.center), axCamera.worldPos);
	float d = camDistance - group.radius;
	float viewportH = axCamera.viewportMax.y - axCamera.viewportMin.y;
	return viewportH * group.error / (d > axCamera.nearClip ? d : axCamera.nearClip) * (camProj * 0.5f);
}

static bool axMeshlet_ClusterCulling(AxGpuData_MeshletCluster cluster) {
	Vec3f worldCenter = axObjectToWorldPos(cluster.center);

	if (!ax_bit_has(axDebug.flags, AxGpuData_Debug_FLAG_DisableFrustumCulling)) {
		for (int i = 0; i < 6; i++) {
			Vec4f plane = axCamera.cullingPlanes[i];
			if (dot(worldCenter, plane.xyz) - plane.w > cluster.radius)
				return false;
		}
	}

	if (!ax_bit_has(axDebug.flags, AxGpuData_Debug_FLAG_DisableBackConeCulling)) {
		Vec3f viewDir  = normalize(worldCenter - axCamera.cullingCameraPos);

		// coneAxis can be (0,0,0), but will equal cutoff(0)
//		Vec4f cone_axis_cutoff = cluster.cone_axis_cutoff;
		Vec4f cone_axis_cutoff = ax_unpack_normal4_u32(cluster.cone_axis_cutoff_packed);
		Vec3f coneAxis   = cone_axis_cutoff.xyz;
		float coneCutoff = cone_axis_cutoff.w;

		if (dot(viewDir, coneAxis) >= coneCutoff + 0.02f)
			return false;
	}

	return true;
}

[numthreads(AS_GROUP_SIZE, 1, 1)]
void axMeshlet_AmplificationMain(
	SEM_gid(u32),
	SEM_gtid(u32),
	SEM_dtid(u32)
) {
	AxGpuData_MeshObject meshObject = axGpuData_MeshObject[AX_MESH_OBJECT_ID];

	if (gtid == 0) {
		s_payload.meshObjectId = AX_MESH_OBJECT_ID;
	}

	u32 clusterOffset = meshObject.meshletClusterOffset;
	u32 clusterCount  = meshObject.meshletClusterCount;
	u32 groupOffset   = meshObject.meshletGroupOffset;

	bool visibleResult = false;

	if (dtid < clusterCount) {
		u32 clusterId = clusterOffset + dtid;
		AxGpuData_MeshletCluster cluster = axGpuData_MeshletCluster[clusterId];

		Vec3f camPos           = axCamera.worldPos;
		float camTanFov        = tan(radians(axCamera.fieldOfView * 0.5)) * 2.0;
		float maxErrorInPixels = axCamera.maxMeshletErrorInPixels;

		AxGpuData_MeshletGroup group       = axGpuData_MeshletGroup[groupOffset + cluster.groupId];
		AxGpuData_MeshletGroup refineGroup = axGpuData_MeshletGroup[groupOffset + cluster.refinedGroupId];

		// when requesting DAG cut from a viewpoint, we need to check if each cluster is the least detailed cluster that passes the error threshold
		bool lodMatch 
		=  (cluster.refinedGroupId < 0 || axMeshlet_boundsError(refineGroup) <= maxErrorInPixels) 
		&& (                              axMeshlet_boundsError(group      ) >  maxErrorInPixels);

		bool culling = axMeshlet_ClusterCulling(cluster);

		visibleResult = lodMatch && culling;

		if (axDebug.showAllLodDistance > 0) {
			visibleResult = true;
		}

		if (visibleResult) {
			uint outIndex = WavePrefixCountBits(visibleResult);
			s_payload.meshletIds[outIndex] = clusterId;
		}
	}

	uint visibleResultCount = WaveActiveCountBits(visibleResult);
	s_payload.clusterCount = clusterCount;
	DispatchMesh(visibleResultCount, 1, 1, s_payload);
}


[numthreads(MS_GROUP_SIZE, 1, 1)]
[outputtopology("triangle")]
void axMeshlet_MeshMain(
	SEM_gid(u32),
	SEM_gtid(u32),
	SEM_dtid(u32),
	in payload Payload payload,
	out vertices   MeshletVS_Output     outVert[AX_HLSL_MESH_SHADER_MAX_VERT_COUNT],
	out primitives MeshletVS_PrimId_Out outPrimId[AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT],
	out indices    u32x3                outPrim[AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT]
) {
	u32 clusterId = payload.meshletIds[gid];
	AxGpuData_MeshletCluster cluster = axGpuData_MeshletCluster[clusterId];

	uint4 vert_prim_count = ax_unpack_uint4_u32(cluster.vert_prim_count);
	uint  vertCount = vert_prim_count.x;
	uint  primCount = vert_prim_count.y;

	SetMeshOutputCounts(vertCount, primCount);

	if (gtid < primCount) {
		outPrim[gtid] = ax_unpack_tri_indices(axGpuData_MeshletPrim[cluster.primOffset + gtid].packedTriIndices);
		outPrimId[gtid].primitiveId = gtid;
	}

	if (gtid < vertCount) {
		MeshletVS_Input i;

		AxGpuData_MeshletVert mv = axGpuData_MeshletVert[cluster.vertOffset + gtid];
		i.pos = mv.pos;

		if (axDebug.showAllLodDistance > 0) { i.pos += ax_debug_lod_offset(cluster.lod); }
		if (axDebug.drawClusterOffset  > 0) { i.pos += axMeshlet_clusterDebug(cluster); }

		i.color0 = float4(1,1,1,1);
		i.normal = ax_unpack_normal_octahedral(mv.normal_octahedral);
		i.uv0    = ax_unpack_uv_u32(mv.uv0_packed);
		i.uv1    = ax_unpack_uv_u32(mv.uv1_packed);

		switch (axDebug.debugColorCode) {
			case AxGpuDebugColorCode_MeshletCluster:      i.color0 = ax_debug_color(clusterId             ); break;
			case AxGpuDebugColorCode_MeshletGroup:        i.color0 = ax_debug_color(cluster.groupId       ); break;
			case AxGpuDebugColorCode_MeshletRefinedGroup: i.color0 = ax_debug_color(cluster.refinedGroupId); break;
			case AxGpuDebugColorCode_MeshletLod:          i.color0 = ax_debug_color(cluster.lod           ); break;
		}

		MeshletVS_Output o;
		o.worldPos = axObjectToWorldPos(i.pos);
		o.sv_pos   = axObjectToClipPos(i.pos);
		o.normal   = axObjectToClipNormal(i.normal);
		o.color0   = i.color0;
		o.uv0      = i.uv0;
		o.uv1      = i.uv1;
		outVert[gtid] = o;
	}
}



#endif // __AxMeshShaderDraw_hlsl__