#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

#include "AxBasicType.hlsl"


StructuredBuffer<AxGpuData_MeshletVert       > axGpuData_MeshletVert        : register(t201, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletPrim       > axGpuData_MeshletPrim        : register(t202, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletCluster    > axGpuData_MeshletCluster     : register(t203, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletGroup      > axGpuData_MeshletGroup       : register(t204, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshObject        > axGpuData_MeshObject         : register(t205, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshObjectRenderer> axGpuData_MeshObjectRenderer : register(t206, AX_BindSpace_World);

#endif //  __AxMeshlet_hlsl__
