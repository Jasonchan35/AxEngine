#ifndef __AxMeshlet_hlsl__
#define __AxMeshlet_hlsl__

#include "AxBasicType.hlsl"


StructuredBuffer<AxMeshletVert> axMeshletVert : register(t201, AX_BindSpace_World);


StructuredBuffer<AxMeshletPrim> axMeshletPrim : register(t202, AX_BindSpace_World);


StructuredBuffer<AxMeshlet> axMeshlet : register(t203, AX_BindSpace_World);

StructuredBuffer<AxMeshInfo> axMeshInfo : register(t204, AX_BindSpace_World);

#endif //  __AxMeshlet_hlsl__
