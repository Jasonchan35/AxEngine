#ifndef __AX_HLSL_CPU_COMMON_h__
#define __AX_HLSL_CPU_COMMON_h__

// https://developer.nvidia.com/blog/introduction-turing-mesh-shaders/
// recommend using up to 64 vertices and 126 primitives
// The first generation hardware allocates primitive indices in 128 byte granularity 
// and needs to reserve 4 bytes for the primitive count. 
// Therefore 3 * 126 + 4 maximizes the fit into a 3 * 128 = 384 bytes block. 
// Going beyond 126 triangles would allocate the next 128 bytes. 
// 84 and 40 are other maxima that work well for triangles.
#define AX_HLSL_MESH_SHADER_MAX_VERT_COUNT	126U 
#define AX_HLSL_MESH_SHADER_MAX_PRIM_COUNT	126U
#define AX_HLSL_THREADS_PER_WAVE			32U

#define AxGpuDebugColorCode_None                0
#define AxGpuDebugColorCode_Tri                 1
#define AxGpuDebugColorCode_MeshletCluster      2
#define AxGpuDebugColorCode_MeshletGroup        3
#define AxGpuDebugColorCode_MeshletRefinedGroup 4
#define AxGpuDebugColorCode_MeshletLod          5

#define AX_HLSL_MAX_ACTIVE_LIGHTS 32

#define AxGpuData_Debug_FLAG_DisableFrustumCulling	u32(1 << 0)

#endif // __AX_HLSL_CPU_COMMON_h__