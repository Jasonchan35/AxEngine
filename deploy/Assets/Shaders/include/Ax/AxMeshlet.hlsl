#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

#include "AxBasicType.hlsl"


StructuredBuffer<AxGpuData_MeshletVert       > axGpuData_MeshletVert     : register(t801, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletPrim       > axGpuData_MeshletPrim     : register(t802, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletCluster    > axGpuData_MeshletCluster  : register(t803, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletGroup      > axGpuData_MeshletGroup    : register(t804, AX_BindSpace_World);

#endif //  __AxMeshlet_hlsl__
