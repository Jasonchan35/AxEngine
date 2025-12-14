#pragma once


#define AX_RENDER_StockTextureId_SolidColor_ENUM_LIST(E) \
	E(Zero			,) \
	E(Black			,) \
	E(White			,) \
	E(Red			,) \
	E(Green			,) \
	E(Blue			,) \
	E(Yellow		,) \
	E(Cyan			,) \
	E(Magenta		,) \
	E(Gray			,) \
	E(DarkRed		,) \
	E(DarkGreen		,) \
	E(DarkBlue		,) \
	E(DarkYellow	,) \
	E(DarkCyan		,) \
	E(DarkMagenta	,) \
//-----

#define AX_ShaderPropType_Numbers_EnumList(E) \
	E(i32,	)	E(i32x2,	)	E(i32x3,	)	E(i32x4,	)	\
	E(u32,	)	E(u32x2,	)	E(u32x3,	)	E(u32x4,	)	\
	E(f16,	)	E(f16x2,	)	E(f16x3,	)	E(f16x4,	)	\
	E(f32,	)	E(f32x2,	)	E(f32x3,	)	E(f32x4,	)	\
	E(f64,	)	E(f64x2,	)	E(f64x3,	)	E(f64x4,	)	\
//---

#define AX_ShaderPropType_EnumList_WITHOUT_NONE(E) \
	AX_ShaderPropType_Numbers_EnumList(E) \
	E(Mat4f,    )\
	E(Color3f,  ) E(Color4f,  ) \
	E(Sampler,  ) \
	E(Texture2D,) E(Texture3D,) E(TextureCube,)\
//----

#define AX_ShaderPropType_EnumList(E) \
	E(None,     ) \
	AX_ShaderPropType_EnumList_WITHOUT_NONE(E) \
//----

#define AX_RenderObject_LIST(E, API, SUFFIX) \
	E(RenderContext				, API, SUFFIX) \
	E(RenderPass				, API, SUFFIX) \
	E(RenderTargetColorBuffer	, API, SUFFIX) \
	E(RenderTargetDepthBuffer	, API, SUFFIX) \
	E(RenderRequest				, API, SUFFIX) \
	E(GpuBuffer					, API, SUFFIX) \
	E(Shader					, API, SUFFIX) \
	E(ShaderParamSpace			, API, SUFFIX) \
	E(Material					, API, SUFFIX) \
	E(MaterialParamSpace		, API, SUFFIX) \
	E(Sampler					, API, SUFFIX) \
	E(Texture2D					, API, SUFFIX) \
	/* E(Texture3D					, API, SUFFIX) */ \
	/* E(TextureCube				, API, SUFFIX) */ \
//----

#define AX_RenderObject_ForwardDeclare(OBJ, ...)	\
	class OBJ; \
	class OBJ ## _Backend; \
	class OBJ ## _CreateDesc; \
//---

#define AX_Renderer_NewObject(OBJ, API, SUFFIX) \
	virtual UPtr<OBJ##_Backend> new##OBJ(const MemAllocRequest& req, const OBJ##_CreateDesc& desc) SUFFIX; \
	virtual void _newObject(UPtr<OBJ##_Backend> & outObj, const MemAllocRequest& req, const OBJ##_CreateDesc& desc) SUFFIX; \
//----

#define AX_Renderer_NewObjectImp(OBJ, API, SUFFIX) \
	UPtr<OBJ##_Backend> Renderer_##API::new##OBJ(const MemAllocRequest& req, const OBJ##_CreateDesc& desc ) { \
		return UPtr_new<OBJ##_##API>(req, desc); \
	} \
	void Renderer_##API::_newObject(UPtr<OBJ##_Backend> & outObj, const MemAllocRequest& req, const OBJ##_CreateDesc& desc) { \
		outObj = UPtr_new<OBJ##_##API>(req, desc); \
	} \
//----

#define AX_Renderer_FunctionInterfaces_pure()			AX_RenderObject_LIST(AX_Renderer_NewObject,    AX_EMPTY,  =0)
#define AX_Renderer_FunctionInterfaces_override(API)	AX_RenderObject_LIST(AX_Renderer_NewObject,    API, override)
#define AX_Renderer_FunctionBodies(             API)	AX_RenderObject_LIST(AX_Renderer_NewObjectImp, API, override)

#define AX_RENDER_CommandBuffer_FunctionInterfaces(IMP) \
	virtual void onCommandBegin() IMP; \
	virtual void onCommandEnd() IMP; \
	virtual void onSetViewport(const Rect2f& rect, float minDepth, float maxDepth) IMP; \
	virtual void onSetScissorRect(const Rect2f& rect) IMP; \
	virtual void onRenderPassBegin(RenderPass* pass) IMP; \
	virtual void onRenderPassEnd() IMP; \
	virtual void onDrawCall(Cmd_DrawCall& cmd) IMP; \
//-----