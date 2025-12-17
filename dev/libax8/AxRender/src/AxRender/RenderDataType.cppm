module;

export module AxRender:RenderDataType;
export import :Common;

export namespace ax /*::AxRender*/ {

using RenderSeqId = i64;

struct RenderMemoryInfo {
	Int used = 0;
	Int budget = 0;
};

// HLSL: "register space", DX: BindPoint, Vulkan: "set"
#define AX_RenderParamSpace_ENUM_LIST(E) \
	E(Default       , = 0) \
	E(Global        , = 1) \
	E(PerFrame      , = 2) \
	E(PerObject     , = 3) \
	E(_COUNT        ,    ) \
	E(Invalid       ,= u16_max) \
//-----
AX_ENUM_CLASS(AX_RenderParamSpace_ENUM_LIST, ShaderParamSpaceType, u16);

// HLSL: "shader register", DX: Space, Vulkan: "binding"
enum class ShaderResourceBindPoint : u16 {
	GlobalConstBuffer = 0,
	VertexBuffer	   = 30, // the max limit is 32 (0~31) on RTX 3080
//-----
	Invalid			   = u16_max,
};

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

#define AX_RENDER_GeoComponentFlags_ENUM_LIST(E) \
	E(None,) \
	E(Point,	 = 1 << 0) \
	E(Vertex,	 = 1 << 1) \
	E(Edge,		 = 1 << 2) \
	E(Primitive, = 1 << 3) \
//----
AX_ENUM_FLAGS_CLASS(AX_RENDER_GeoComponentFlags_ENUM_LIST, RenderGeoComponentFlags, u8)

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

#define AX_RenderDataType_Num_ENUM_LIST(E) \
	E(i8x1		,) E(i8x2		,) E(i8x3		,) E(i8x4		,) \
	E(i16x1		,) E(i16x2		,) E(i16x3		,) E(i16x4		,) \
	E(i32x1		,) E(i32x2		,) E(i32x3		,) E(i32x4		,) \
	E(i64x1		,) E(i64x2		,) E(i64x3		,) E(i64x4		,) \
	\
	E(u8x1		,) E(u8x2		,) E(u8x3		,) E(u8x4		,) \
	E(u16x1		,) E(u16x2		,) E(u16x3		,) E(u16x4		,) \
	E(u32x1		,) E(u32x2		,) E(u32x3		,) E(u32x4		,) \
	E(u64x1		,) E(u64x2		,) E(u64x3		,) E(u64x4		,) \
	\
	E(f16x1		,) E(f16x2		,) E(f16x3		,) E(f16x4		,) \
	E(f32x1		,) E(f32x2		,) E(f32x3		,) E(f32x4		,) \
	E(f64x1		,) E(f64x2		,) E(f64x3		,) E(f64x4		,) \
	\
//---

#define AX_RenderDataType_Vec_ENUM_LIST(E) \
	E(Vec1i		,) E(Vec2i		,) E(Vec3i		,) E(Vec4i		,) \
	E(Vec1h		,) E(Vec2h		,) E(Vec3h		,) E(Vec4h		,) \
	E(Vec1f		,) E(Vec2f		,) E(Vec3f		,) E(Vec4f		,) \
	E(Vec1d		,) E(Vec2d		,) E(Vec3d		,) E(Vec4d		,) \
//---
#define AX_RenderDataType_Mat_ENUM_LIST(E) \
	E(Mat4f		,) E(Mat4d		,) \
//---

#define AX_RenderDataType_Norm_ENUM_LIST(E) \
	E(SNorm8	,) E(SNorm8x2	,) E(SNorm8x3	,) E(SNorm8x4	,) \
	E(SNorm16	,) E(SNorm16x2	,) E(SNorm16x3	,) E(SNorm16x4	,) \
	E(SNorm32	,) E(SNorm32x2	,) E(SNorm32x3	,) E(SNorm32x4	,) \
	\
	E(UNorm8	,) E(UNorm8x2	,) E(UNorm8x3	,) E(UNorm8x4	,) \
	E(UNorm16	,) E(UNorm16x2	,) E(UNorm16x3	,) E(UNorm16x4	,) \
	E(UNorm32	,) E(UNorm32x2	,) E(UNorm32x3	,) E(UNorm32x4	,) \
//---

#define AX_RenderDataType_Color_ENUM_LIST(E) \
	E(Color3b	,) E(Color3s	,) E(Color3h	,) E(Color3f	,)	E(Color3d	,)\
	E(Color4b	,) E(Color4s	,) E(Color4h	,) E(Color4f	,)	E(Color4d	,)\
//---

#define AX_RenderDataType_Char_ENUM_LIST(E) \
	E(CharA		,) E(Char16		,) E(Char32		,) E(CharW		,) \
//---

#define AX_RenderDataType_String_ENUM_LIST(E) \
	E(StringA	,) E(String16	,) E(String32	,) E(StringW	,) \
//---


#define AX_RenderDataType_Basic_ENUM_LIST(E) \
	/* E(bool,) */ \
	AX_RenderDataType_Int_ENUM_LIST(E) \
	AX_RenderDataType_Float_ENUM_LIST(E) \
	AX_RenderDataType_Num_ENUM_LIST(E) \
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
	ColorType          colorType       = ColorType::RGBAb;
	RenderBufferLoadOp loadOp          = RenderBufferLoadOp::Clear;
	Color4f            clearColorValue = Color4f::kBlack();

	bool operator==(const RenderPassColorAttachmentDesc& r) const {
		return colorType  == r.colorType
			&& loadOp     == r.loadOp
			&& clearColorValue.exactlyEqual(r.clearColorValue);
	}
};

class RenderPassDepthAttachmentDesc {
public:
	RenderDepthType    depthType         = RenderDepthType::Depth_UNorm24_Stencil_UInt8;
	RenderBufferLoadOp loadOp            = RenderBufferLoadOp::Clear;
	f32                clearDepthValue   = 1;
	u32                clearStencilValue = 0;
	
	AX_INLINE bool isEnabled() const { return depthType != RenderDepthType::None; }
	AX_INLINE explicit operator bool() const { return isEnabled(); }
	
	bool operator==(const RenderPassDepthAttachmentDesc& r) const {
		return depthType	== r.depthType
			&& loadOp		== r.loadOp
			&& Math::exactlyEqual(clearDepthValue, r.clearDepthValue)
			&& clearStencilValue	== r.clearStencilValue;
	}
};

template<class T> struct RenderDataType_get_;

// C++ Type -> RenderDataType
template<class T> constexpr RenderDataType DataType_get = RenderDataType_get_<T>::value;

// RenderDataType -> C++ Type
template<RenderDataType dt> struct RenderDataType_Type_;
template<RenderDataType dt> using  RenderDataType_Type = typename RenderDataType_Type_<dt>::Type;

#define E(T, ...)\
	template<> struct RenderDataType_Type_<RenderDataType::T> { using Type = T; }; \
	template<> struct RenderDataType_get_<T> { static constexpr RenderDataType value = RenderDataType::T; }; \
//----
	AX_RenderDataType_Basic_ENUM_LIST(E)
#undef E

} // namespace ax /*::AxRender*/