#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

#include "AxBasicType.hlsl"


StructuredBuffer<AxGpuMeshletVert       > axGpuMeshletVert        : register(t201, AX_BindSpace_World);
StructuredBuffer<AxGpuMeshletPrim       > axGpuMeshletPrim        : register(t202, AX_BindSpace_World);
StructuredBuffer<AxGpuMeshlet           > axGpuMeshlet            : register(t203, AX_BindSpace_World);
StructuredBuffer<AxGpuMeshObject        > axGpuMeshObject         : register(t204, AX_BindSpace_World);
StructuredBuffer<AxGpuMeshObjectRenderer> axGpuMeshObjectRenderer : register(t205, AX_BindSpace_World);

#endif //  __AxMeshlet_hlsl__
