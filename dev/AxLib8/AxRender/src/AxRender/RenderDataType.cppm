module;

export module AxRender:RenderDataType;
export import :Common;

export namespace ax /*::AxRender*/ {

using RenderSeqId = i64;

class RenderObject : public RttiObject {
	AX_RTTI_INFO(RenderObject, RttiObject)
public:

#if AX_RENDER_DEBUG_NAME
	void setDebugName(InNameId name) { _debugName = name; onSetDebugName(_debugName); }
#endif

	NameId  name() const		{ return _name; }
	NameId	debugName() const	{ return _debugName; }

protected:
	NameId	_name;
	NameId	_debugName;
#if AX_RENDER_DEBUG_NAME
	virtual void onSetDebugName(NameId name) {}
#endif
};

class RenderRequestBase : public RenderObject {
	AX_RTTI_INFO(RenderRequestBase, RenderObject)
public:
	AX_INLINE 	RenderSeqId	renderSeqId() const { return _renderSeqId; }
protected:
	RenderSeqId	_renderSeqId = 0;
};

struct RenderSeqIdGraud {
	RenderSeqId last = 0;
	AX_NODISCARD AX_INLINE bool update(RenderRequestBase* req) {
		auto cur = req->renderSeqId();
		if (last == cur) return false;
		last = cur;
		return true;
	}
};


struct RenderMemoryInfo {
	Int used = 0;
	Int budget = 0;
};

// HLSL: "register space", DX Reflection: Space, Vulkan: "set"
#define AX_ShaderParamBindSpace_ENUM_LIST(E) \
	E(Default   , = 0) \
	E(World     , = 1) \
	E(Object    , = 2) \
	E(Bindless  , = 3) \
	E(_COUNT    ,    ) \
	E(Invalid   ,= u16_max) \
//-----
AX_ENUM_CLASS(AX_ShaderParamBindSpace_ENUM_LIST, ShaderParamBindSpace, u16);

constexpr bool ShaderParamBindSpace_isCommon(ShaderParamBindSpace t) {
	using BS = ShaderParamBindSpace;
	return t == BS::World || t == BS::Bindless;
}

// HLSL: "shader register", DX Reflection: BindPoint, Vulkan: "binding"
enum class ShaderParamBindPoint : u32 {
	GlobalConstBuffer = 0,
	VertexBuffer	  = 30, // the max limit is 32 (0~31) on RTX 3080
//-----
	Invalid			  = u32_max,
};
AX_ENUM_STR_FROM_INT(ShaderParamBindPoint)


using ShaderParamBindCount = u32;

#define AX_ShaderStageFlags_ENUM_LIST(E) \
	E(None,) \
	E(Vertex,	= 1 << 0) \
	E(Pixel,	= 1 << 1) \
	E(Geometry, = 1 << 2) \
	E(Compute,	= 1 << 15) \
	E(_END,) \
//----
AX_ENUM_FLAGS_CLASS(AX_ShaderStageFlags_ENUM_LIST, ShaderStageFlags, u32)

#define AX_RENDER_AutoFrameSize_ENUM_LIST(E) \
	E(None,) \
	E(Full,) \
	E(Half,) \
	E(Quarter,) \
	E(Div8,) \
	E(Div16,) \
	E(Div32,) \
	E(Div64,) \
	E(Plus4,) \
//----
AX_ENUM_FLAGS_CLASS(AX_RENDER_AutoFrameSize_ENUM_LIST, AutoFrameSize, u8)

inline
Vec2i AutoFrameSize_Compute(AutoFrameSize mode, Vec2i v) {
	using E = AutoFrameSize;
	switch (mode) {
		case E::None:		return v;
		case E::Full:		return v;
		case E::Half:		return v / 2;
		case E::Quarter:	return v / 4;
		case E::Div8:		return v / 8;
		case E::Div16:		return v / 16;
		case E::Div32:		return v / 32;
		case E::Div64:		return v / 64;
		case E::Plus4:		return v + Margin2i(4).total();
		default: AX_ASSERT(false); return v;
	}
}

#define AX_RenderGeoComponentFlags_ENUM_LIST(E) \
	E(None,) \
	E(Point,	 = 1 << 0) \
	E(Vertex,	 = 1 << 1) \
	E(Edge,		 = 1 << 2) \
	E(Primitive, = 1 << 3) \
//----
AX_ENUM_FLAGS_CLASS(AX_RenderGeoComponentFlags_ENUM_LIST, RenderGeoComponentFlags, u8)

#define AX_RenderPrimitiveType_ENUM_LIST(E) \
	E(None,) \
	E(Points,) \
	E(Lines,) \
	E(Triangles,) \
//----
AX_ENUM_CLASS(AX_RenderPrimitiveType_ENUM_LIST, RenderPrimitiveType, u8)

#define AX_RenderDepthType_ENUM_LIST(E) \
	E(None,) \
	E(Depth_UNorm16,) \
	E(Depth_Float32,) \
	/* E(Depth_UNorm16_Stencil_UInt8,) - no in DX12 */  \
	E(Depth_UNorm24_Stencil_UInt8,) \
	E(Depth_Float32_Stencil_UInt8,) \
	/* E(Stencil_UInt8,) - no in DX12 */ \
//----
AX_ENUM_CLASS(AX_RenderDepthType_ENUM_LIST, RenderDepthType, u8)

#define AX_RenderBufferLoadOp_ENUM_LIST(E) \
	E(None		,) \
	E(DontCare	,) \
	E(Load		,) \
	E(Clear		,) \
//--
AX_ENUM_CLASS(AX_RenderBufferLoadOp_ENUM_LIST, RenderBufferLoadOp, u8)

#define AX_RenderDataType_Int_ENUM_LIST(E) \
	E(i8,) E(i16,) E(i32,) E(i64,) \
	E(u8,) E(u16,) E(u32,) E(u64,) \
//---

#define AX_RenderDataType_Float_ENUM_LIST(E) \
	E(f16,) E(f32,) E(f64,) \
//---

#define AX_RenderDataType_Vec_ENUM_LIST(E) \
	E(Vec1i8_Basic,) E(Vec2i8_Basic,) E(Vec3i8_Basic,) E(Vec4i8_Basic,) \
	E(Vec1u8_Basic,) E(Vec2u8_Basic,) E(Vec3u8_Basic,) E(Vec4u8_Basic,) \
	\
	E(Vec1i16_Basic,) E(Vec2i16_Basic,) E(Vec3i16_Basic,) E(Vec4i16_Basic,) \
	E(Vec1i32_Basic,) E(Vec2i32_Basic,) E(Vec3i32_Basic,) E(Vec4i32_Basic,) \
	E(Vec1i64_Basic,) E(Vec2i64_Basic,) E(Vec3i64_Basic,) E(Vec4i64_Basic,) \
	\
	E(Vec1u16_Basic,) E(Vec2u16_Basic,) E(Vec3u16_Basic,) E(Vec4u16_Basic,) \
	E(Vec1u32_Basic,) E(Vec2u32_Basic,) E(Vec3u32_Basic,) E(Vec4u32_Basic,) \
	E(Vec1u64_Basic,) E(Vec2u64_Basic,) E(Vec3u64_Basic,) E(Vec4u64_Basic,) \
	\
	E(Vec1h_Basic,) E(Vec2h_Basic,) E(Vec3h_Basic,) E(Vec4h_Basic,) \
	E(Vec1f_Basic,) E(Vec2f_Basic,) E(Vec3f_Basic,) E(Vec4f_Basic,) \
	E(Vec1d_Basic,) E(Vec2d_Basic,) E(Vec3d_Basic,) E(Vec4d_Basic,) \
	\
	E(Vec1snorm8_Basic ,) E(Vec2snorm8_Basic ,) E(Vec3snorm8_Basic ,) E(Vec4snorm8_Basic ,) \
	E(Vec1snorm16_Basic,) E(Vec2snorm16_Basic,) E(Vec3snorm16_Basic,) E(Vec4snorm16_Basic,) \
	E(Vec1snorm32_Basic,) E(Vec2snorm32_Basic,) E(Vec3snorm32_Basic,) E(Vec4snorm32_Basic,) \
	\
	E(Vec1unorm8_Basic ,) E(Vec2unorm8_Basic ,) E(Vec3unorm8_Basic ,) E(Vec4unorm8_Basic ,) \
	E(Vec1unorm16_Basic,) E(Vec2unorm16_Basic,) E(Vec3unorm16_Basic,) E(Vec4unorm16_Basic,) \
	E(Vec1unorm32_Basic,) E(Vec2unorm32_Basic,) E(Vec3unorm32_Basic,) E(Vec4unorm32_Basic,) \
//---

#define AX_RenderDataType_Mat_ENUM_LIST(E) \
	E(Mat4f_Basic,) E(Mat4d_Basic,) \
//---

#define AX_RenderDataType_Norm_ENUM_LIST(E) \
	E(SNorm8,) E(SNorm16,) E(SNorm32,) \
	E(UNorm8,) E(UNorm16,) E(UNorm32,) \
//---

#define AX_RenderDataType_Color_ENUM_LIST(E) \
	E(Color3b_Basic,) E(Color4b_Basic,) \
	E(Color3s_Basic,) E(Color4s_Basic,) \
	E(Color3h_Basic,) E(Color4h_Basic,) \
	E(Color3f_Basic,) E(Color4f_Basic,) \
	E(Color3d_Basic,) E(Color4d_Basic,) \
//---

#define AX_RenderDataType_Char_ENUM_LIST(E) \
	E(CharA,) E(Char16,) E(Char32,) E(CharW,) \
//---

#define AX_RenderDataType_String_ENUM_LIST(E) \
	E(StringA,) E(String16,) E(String32,) E(StringW,) \
//---


#define AX_RenderDataType_Basic_ENUM_LIST(E) \
	/* E(bool,) */ \
	AX_RenderDataType_Int_ENUM_LIST(E) \
	AX_RenderDataType_Float_ENUM_LIST(E) \
	AX_RenderDataType_Vec_ENUM_LIST(E) \
	AX_RenderDataType_Mat_ENUM_LIST(E) \
	AX_RenderDataType_Norm_ENUM_LIST(E) \
	AX_RenderDataType_Char_ENUM_LIST(E) \
	AX_RenderDataType_String_ENUM_LIST(E) \
	AX_RenderDataType_Color_ENUM_LIST(E) \
//---

#define AX_RenderDataType_ENUM_LIST_WITHOUT_NONE(E) \
	AX_RenderDataType_Basic_ENUM_LIST(E) \
	\
	E(SamplerState,) \
	E(DepthTexture,) \
	E(Texture1D,)   \
	E(Texture2D,)   \
	E(Texture3D,)   \
	E(TextureCube,) \
	\
	E(Texture1DArray,)   \
	E(Texture2DArray,)   \
	E(Texture3DArray,)   \
	E(TextureCubeArray,) \
	\
	E(ConstBuffer,) \
	E(StorageBuffer,) \
//----

#define AX_RenderDataType_ENUM_LIST(E) \
	E(None,) \
	AX_RenderDataType_ENUM_LIST_WITHOUT_NONE(E) \
//-------
AX_ENUM_CLASS(AX_RenderDataType_ENUM_LIST, RenderDataType, u16);

struct RenderDataTypeInfo {
	RenderDataType	dataType		= RenderDataType::None;
	i16				elementCount	= 0;
	Int				sizeInBytes		= 0;

	explicit operator bool() const { return dataType != RenderDataType::None; }
	const RenderDataTypeInfo& s_get(RenderDataType t);
};

class RenderPassColorAttachmentDesc {
public:
	ColorType          colorType  = ColorType::RGBAb;
	RenderBufferLoadOp loadOp     = RenderBufferLoadOp::Clear;
	Color4f            clearColor = Color4f::kBlack();

	bool operator==(const RenderPassColorAttachmentDesc& r) const {
		return colorType  == r.colorType
			&& loadOp     == r.loadOp
			&& clearColor.exactlyEqual(r.clearColor);
	}
};

class RenderPassDepthAttachmentDesc {
public:
	RenderDepthType    depthType    = RenderDepthType::Depth_UNorm24_Stencil_UInt8;
	RenderBufferLoadOp loadOp       = RenderBufferLoadOp::Clear;
	f32                clearDepth   = 1;
	u32                clearStencil = 0;
	
	AX_INLINE bool isEnabled() const { return depthType != RenderDepthType::None; }
	AX_INLINE explicit operator bool() const { return isEnabled(); }
	
	bool operator==(const RenderPassDepthAttachmentDesc& r) const {
		return depthType	== r.depthType
			&& loadOp		== r.loadOp
			&& Math::exactlyEqual(clearDepth, r.clearDepth)
			&& clearStencil	== r.clearStencil;
	}
};

template<class T> struct RenderDataType_get_Struct;

// C++ Type -> RenderDataType
template<class T> constexpr RenderDataType RenderDataType_get = RenderDataType_get_Struct<T>::value;

// RenderDataType -> C++ Type
template<RenderDataType T> struct RenderDataType_Type_;
template<RenderDataType T> using  RenderDataType_Type = typename RenderDataType_Type_<T>::Type;

#define E(T, ...)\
	template<> struct RenderDataType_Type_<RenderDataType::T> { using Type = T; }; \
	template<> struct RenderDataType_get_Struct<T> { static constexpr RenderDataType value = RenderDataType::T; }; \
//----
	AX_RenderDataType_Basic_ENUM_LIST(E)
#undef E

} // namespace ax /*::AxRender*/