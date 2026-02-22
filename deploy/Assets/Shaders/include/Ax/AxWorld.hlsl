#ifndef __AxWorld_HLS__
#define __AxWorld_HLS__

#include "AxBasicType.hlsl"

ConstantBuffer<AxGpuData_World > axWorld  : register(b100, AX_BindSpace_World);
ConstantBuffer<AxGpuData_Camera> axCamera : register(b101, AX_BindSpace_World);
ConstantBuffer<AxGpuData_Debug > axDebug  : register(b102, AX_BindSpace_World);

StructuredBuffer<AxGpuData_MeshObject    > axGpuData_MeshObject     : register(t200, AX_BindSpace_World);
StructuredBuffer<AxGpuData_TileLighting  > axGpuData_TileLighting   : register(t201, AX_BindSpace_World);
StructuredBuffer<AxGpuData_LightObject   > axGpuData_LightObject    : register(t202, AX_BindSpace_World);
StructuredBuffer<AxGpuData_CameraObject  > axGpuData_CameraObject   : register(t203, AX_BindSpace_World);

StructuredBuffer<AxGpuData_MeshletVert   > axGpuData_MeshletVert    : register(t301, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletPrim   > axGpuData_MeshletPrim    : register(t302, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletCluster> axGpuData_MeshletCluster : register(t303, AX_BindSpace_World);
StructuredBuffer<AxGpuData_MeshletGroup  > axGpuData_MeshletGroup   : register(t304, AX_BindSpace_World);


Vec3f ax_debug_lod_offset(uint lod) { return Vec3f(lod * axDebug.showAllLodDistance, 0, 0); }

Color3f axLight_Blinn(
	AxGpuData_LightObject light,
	Vec3f worldPos, Vec3f worldNormal
) {
	Color4f color     = Color4f(1, 1, 1, 1);
    Color3f ambient   = Color3f(0.2, 0.2, 0.2);
    Color3f diffuse   = Color3f(0.8, 0.8, 0.8);
    Color3f specular  = Color3f(1, 1, 1);
    float   shininess = 32;

	Vec3f lightVec = light.worldPos - worldPos;
	float lightDis = length(lightVec);
	Vec3f lightDir = lightVec / lightDis;

	Vec3f viewDir  = normalize(axCamera.worldPos - worldPos);

	Color3f lightColor = light.color;

	float range       = max(0, lightDis / light.radius);
	float attenuation = max(0, 1 - range * range);
	float intensity   = light.intensity * attenuation;

	// ambient
	Color3f outAmbient = lightColor * ambient;

	// diffuse 
	float diff       = max(dot(worldNormal, lightDir), 0.0);
	Color3f outDiffuse = lightColor * (diff * diffuse);

	// specular
	Vec3f reflectDir  = reflect(-lightDir, worldNormal);  
	float spec        = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	Color3f outSpecular = lightColor * (specular * spec);
		
	Color3f lightResult = outAmbient + outDiffuse + outSpecular;
	return lightResult * intensity;
}

Color3f axTileLight_Blinn(Vec3f worldPos, Vec3f worldNormal) {
	Color3f lightResult = Color3f(0,0,0);

	AxGpuData_TileLighting tile = axGpuData_TileLighting[0];
	for (int j = 0; j < tile.lightCount; ++j) {
		AxGpuData_LightObject light = axGpuData_LightObject[tile.lightIds[j]];
		lightResult += axLight_Blinn(light, worldPos, worldNormal);
	}

	return lightResult;
}

#endif // __AxWorld_HLS__