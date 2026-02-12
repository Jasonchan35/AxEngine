#ifndef __AxWorld_HLS__
#define __AxWorld_HLS__

#include "AxMeshlet.hlsl"

ConstantBuffer<AxRenderGpuData_World> axWorld : register(b100, AX_BindSpace_World);
ConstantBuffer<AxRenderGpuData_Camera> axCamera : register(b101, AX_BindSpace_World);

StructuredBuffer<AxRenderGpuData_Light> axLights : register(t100, AX_BindSpace_World);
StructuredBuffer<AxRenderGpuData_Object> axObjects : register(t101, AX_BindSpace_World);


#endif // __AxWorld_HLS__