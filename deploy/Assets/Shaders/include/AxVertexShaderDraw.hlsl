#ifndef __AxVertexShaderDraw_hlsl__
#define __AxVertexShaderDraw_hlsl__

#include "Ax/AxWorld.hlsl"

AX_ROOT_CONST_STRUCT(AxVertexShaderDraw_RootConst, axVertexShaderDraw_RootConst) 
#define AX_MATRIX_M			axVertexShaderDraw_RootConst.worldMatrix
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


#endif // __AxVertexShaderDraw_hlsl__