module;

export module AxRender:RenderDataType;
export import :Common;

export namespace ax /*::AxRender*/ {

using RenderSeqId = i64;

class RenderObject : public RttiObject {
	AX_RTTI_INFO(RenderObject, RttiObject)
public:
	NameId  name() const		{ return _name; }

	void setName(InNameId name) { _name = name; onSetName(name); }

	using GpuData = nullptr_t;
	static constexpr Int s_gpuBufferMaxSize () { return 1 * Math::GigaBytes; }
	static constexpr Int s_gpuBufferPageSize() { return 4 * Math::MegaBytes; }
protected:
	virtual void onSetName(NameId name) {};
	
	NameId	_name;
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
	E(Bindless  , = 2) \
	E(RootConst , = 3) \
	E(_COUNT    ,    ) \
	E(Invalid   , = u16_max) \
//-----
AX_ENUM_CLASS(AX_ShaderParamBindSpace_ENUM_LIST, ShaderParamBindSpace, u16);

constexpr bool ShaderParamBindSpace_isCommon(ShaderParamBindSpace t) {
	using BS = ShaderParamBindSpace;
	return t == BS::World || t == BS::Bindless;
}

// HLSL: "shader register", DX Reflection: BindPoint, Vulkan: "binding"
enum class ShaderParamBindPoint : u32 {
	Zero = 0,
//	GlobalConstBuffer = 0,
	BindVertexBuffer	  = 30, // the max limit is 32 (0~31) on RTX 3080
//-----
	Invalid			  = u32_max,
};
AX_ENUM_STR_FROM_INT(ShaderParamBindPoint)

using ShaderParamBindCount = u32;

#define AX_ShaderStageFlags_ENUM_LIST(E) \
	E(None,) \
	E(Vertex       , = 1 << 0) \
	E(Pixel        , = 1 << 1) \
	E(Geometry     , = 1 << 2) \
	E(Compute      , = 1 << 3) \
	E(Mesh         , = 1 << 4) \
	E(Amplification, = 1 << 5) \
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
	E(i8x1,) E(i8x2,) E(i8x3,) E(i8x4,) \
	E(u8x1,) E(u8x2,) E(u8x3,) E(u8x4,) \
	\
	E(i16x1,) E(i16x2,) E(i16x3,) E(i16x4,) \
	E(i32x1,) E(i32x2,) E(i32x3,) E(i32x4,) \
	E(i64x1,) E(i64x2,) E(i64x3,) E(i64x4,) \
	\
	E(u16x1,) E(u16x2,) E(u16x3,) E(u16x4,) \
	E(u32x1,) E(u32x2,) E(u32x3,) E(u32x4,) \
	E(u64x1,) E(u64x2,) E(u64x3,) E(u64x4,) \
	\
	E(f16x1,) E(f16x2,) E(f16x3,) E(f16x4,) \
	E(f32x1,) E(f32x2,) E(f32x3,) E(f32x4,) \
	E(f64x1,) E(f64x2,) E(f64x3,) E(f64x4,) \
	\
	E(SNorm8x1 ,) E(SNorm8x2 ,) E(SNorm8x3 ,) E(SNorm8x4 ,) \
	E(SNorm16x1,) E(SNorm16x2,) E(SNorm16x3,) E(SNorm16x4,) \
	E(SNorm32x1,) E(SNorm32x2,) E(SNorm32x3,) E(SNorm32x4,) \
	\
	E(UNorm8x1 ,) E(UNorm8x2 ,) E(UNorm8x3 ,) E(UNorm8x4 ,) \
	E(UNorm16x1,) E(UNorm16x2,) E(UNorm16x3,) E(UNorm16x4,) \
	E(UNorm32x1,) E(UNorm32x2,) E(UNorm32x3,) E(UNorm32x4,) \
//---

#define AX_RenderDataType_Mat_ENUM_LIST(E) \
	E(Mat3f,) E(Mat3d,) \
	E(Mat4f,) E(Mat4d,) \
//---

#define AX_RenderDataType_Norm_ENUM_LIST(E) \
	E(SNorm8,) E(SNorm16,) E(SNorm32,) \
	E(UNorm8,) E(UNorm16,) E(UNorm32,) \
//---

#define AX_RenderDataType_Color_ENUM_LIST(E) \
	E(Color3b,) E(Color4b,) \
	E(Color3s,) E(Color4s,) \
	E(Color3h,) E(Color4h,) \
	E(Color3f,) E(Color4f,) \
	E(Color3d,) E(Color4d,) \
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
	E(Bool,) \
	\
	E(SamplerState,) \
	E(DepthTexture,) \
	E(Texture1D,)   \
	E(Texture2D,)   \
	E(Texture3D,)   \
	E(TextureCube,) \
	\
	E(RWTexture1D,)   \
	E(RWTexture2D,)   \
	E(RWTexture3D,)   \
	E(RWTextureCube,) \
	\
	E(Texture1DArray,)   \
	E(Texture2DArray,)   \
	E(Texture3DArray,)   \
	E(TextureCubeArray,) \
	\
	E(Struct,) \
	E(ConstBuffer,) \
	E(StructuredBuffer,) \
	E(RWStructuredBuffer,) \
	E(ByteAddressBuffer,) \
	E(RWByteAddressBuffer,) \
//----

#define AX_RenderDataType_ENUM_LIST(E) \
	E(None,) \
	AX_RenderDataType_ENUM_LIST_WITHOUT_NONE(E) \
//-------
AX_ENUM_CLASS(AX_RenderDataType_ENUM_LIST, RenderDataType, u16);

struct ShaderVariableType {
	RenderDataType	dataType = RenderDataType::None;
	i16 elementCount = 0;
		
	bool isArray() const { return elementCount > 0; }
	bool operator==(const ShaderVariableType&) const = default;
		
	template<class CH>
	void onFormat(Format_<CH>& fmt) const {
		if (elementCount > 1) {
			fmt << Fmt("{}[{}]", dataType, elementCount);
		} else {
			fmt << dataType;
		}
	}
		
	template<class SE>
	void onJsonIO(SE & se) {
		AX_JSON_IO(se, dataType);
		AX_JSON_IO(se, elementCount);
	}		
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
	f32                clearDepth   = ProjectionDesc().isReverseZ ? 0.0f : 1.0f;
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