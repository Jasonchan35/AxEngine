#ifndef __AxWorld_HLS__
#define __AxWorld_HLS__

#include "AxMeshlet.hlsl"

ConstantBuffer<AxGpuData_World > axWorld  : register(b100, AX_BindSpace_World);
ConstantBuffer<AxGpuData_Camera> axCamera : register(b101, AX_BindSpace_World);
ConstantBuffer<AxGpuData_Debug > axDebug  : register(b102, AX_BindSpace_World);

StructuredBuffer<AxGpuData_LightObject >	axGpuData_LightObject  : register(t100, AX_BindSpace_World);
StructuredBuffer<AxGpuData_TileLighting>	axGpuData_TileLighting : register(t101, AX_BindSpace_World);

Vec3f ax_debug_lod_offset(uint lod) { return Vec3f(lod * axDebug.showAllLodDistance, 0, 0); }

Color3f axLight_Blinn(
	AxGpuData_LightObject light,
	Vec3f worldPos, Vec3f worldNormal,
	Color3f ambient,
	Color3f diffuse,
	Color3f specular,
	float   shininess
) {
	Vec3f lightDir = normalize(light.worldPos    - worldPos);
	Vec3f viewDir  = normalize(axCamera.worldPos - worldPos);

	// ambient
	Color3f outAmbient = light.color * ambient;

	// diffuse 
	Vec3f norm       = worldNormal;
	float diff       = max(dot(norm, lightDir), 0.0);
	Color3f outDiffuse = light.color * (diff * diffuse);

	// specular
	Vec3f reflectDir  = reflect(-lightDir, norm);  
	float spec        = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	Color3f outSpecular = light.color * (specular * spec);
		
	Color3f lightResult = outAmbient + outDiffuse + outSpecular;
	return lightResult;
}

#endif // __AxWorld_HLS__