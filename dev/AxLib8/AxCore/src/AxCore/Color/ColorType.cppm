module;

export module AxCore.Color:ColorType;
export import AxCore.Mat;

export namespace ax {

#if 0
#pragma mark ------------------ ColorElem ------------------
#endif

AX_ENUM_CLASS(AX_ColorElem_ENUM_LIST, ColorElem, u8)

template<ColorElem E> struct ColorElem_EnumAsType_ { static constexpr ColorElem kColorElem = E; };
struct ColorElem_R5G6B5       	: public ColorElem_EnumAsType_<ColorElem::R5G6B5     > {};
struct ColorElem_R5G5B5A1     	: public ColorElem_EnumAsType_<ColorElem::R5G5B5A1   > {};
struct ColorElem_R11G11B10    	: public ColorElem_EnumAsType_<ColorElem::R11G11B10  > {};
struct ColorElem_R10G10B10A2  	: public ColorElem_EnumAsType_<ColorElem::R10G10B10A2> {};
struct ColorElem_DXT_BC1  		: public ColorElem_EnumAsType_<ColorElem::DXT_BC1    > {};
struct ColorElem_DXT_BC2  		: public ColorElem_EnumAsType_<ColorElem::DXT_BC2    > {};
struct ColorElem_DXT_BC3  		: public ColorElem_EnumAsType_<ColorElem::DXT_BC3    > {};
struct ColorElem_DXT_BC4  		: public ColorElem_EnumAsType_<ColorElem::DXT_BC4    > {};
struct ColorElem_DXT_BC5  		: public ColorElem_EnumAsType_<ColorElem::DXT_BC5    > {};
struct ColorElem_DXT_BC6h 		: public ColorElem_EnumAsType_<ColorElem::DXT_BC6h   > {};
struct ColorElem_DXT_BC7  		: public ColorElem_EnumAsType_<ColorElem::DXT_BC7    > {};

template<class T>   struct ColorElemLimit;

template<>
struct ColorElemLimit<UNorm8> {
	using Type = UNorm8;
	AX_INLINE static constexpr Type kZero() { return Type::kMin (); }
	AX_INLINE static constexpr Type kOne () { return Type::kMax (); }
	AX_INLINE static constexpr Type kHalf() { return Type::kHalf(); }
};

template<>
struct ColorElemLimit<UNorm16> {
	using Type = UNorm16;
	AX_INLINE static constexpr Type kZero() { return Type::kMin (); }
	AX_INLINE static constexpr Type kOne () { return Type::kMax (); }
	AX_INLINE static constexpr Type kHalf() { return Type::kHalf(); }
};

template<>
struct ColorElemLimit<f16> {
	using Type = f16;
	AX_INLINE static constexpr Type kZero() { return Type::s_zero(); }
	AX_INLINE static constexpr Type kOne () { return Type::s_one (); }
	AX_INLINE static constexpr Type kHalf() { return Type::s_half(); }
};

template<>
struct ColorElemLimit<f32> {
	using Type = f32;
	AX_INLINE static constexpr Type kZero() { return 0;    }
	AX_INLINE static constexpr Type kOne () { return 1.0f; }
	AX_INLINE static constexpr Type kHalf() { return 0.5f; }
};

template<>
struct ColorElemLimit<f64> {
	using Type = f64;
	AX_INLINE static constexpr Type kZero() { return 0;   }
	AX_INLINE static constexpr Type kOne () { return 1.0; }
	AX_INLINE static constexpr Type kHalf() { return 0.5; }
};

// C++ Type -> ColorElem
template<class T> struct ColorElem_get_;
template<class T> constexpr ColorElem	ColorElem_get = ColorElem_get_<T>::value;

// ColorElem -> C++ Type
template<ColorElem E>	struct	ColorElem_Type_;
template<ColorElem E>	using	ColorElem_Type = typename ColorElem_Type_<E>::Type;

#define E(T, ...) \
	template<> struct ColorElem_get_<T> { static constexpr ColorElem value = ColorElem::T; }; \
	template<> struct ColorElem_Type_<ColorElem::T>	{ using Type = T; }; \
//----
	AX_ColorElem_Number_ENUM_LIST(E)
#undef E


struct ColorElemUtil {
	template<class DST, class SRC> AX_INLINE static constexpr DST s_cast(const SRC& src);

	static AX_INLINE f32	to_f32(UNorm8  v) { return v.to_f32(); }
	static AX_INLINE f32	to_f32(UNorm16 v) { return v.to_f32(); }
	static AX_INLINE f32	to_f32(UNorm32 v) { return v.to_f32(); }
	static AX_INLINE f32	to_f32(f16     v) { return v.to_f32(); }
	static AX_INLINE f32	to_f32(f32     v) { return v; }
	static AX_INLINE f32	to_f32(f64     v) { return static_cast<f32>(v); }

	static AX_INLINE f64	to_f64(UNorm8  v) { return v.to_f64(); }
	static AX_INLINE f64	to_f64(UNorm16 v) { return v.to_f64(); }
	static AX_INLINE f64	to_f64(UNorm32 v) { return v.to_f64(); }
	static AX_INLINE f64	to_f64(f16     v) { return v.to_f64(); }
	static AX_INLINE f64	to_f64(f32     v) { return static_cast<f64>(v); }
	static AX_INLINE f64	to_f64(f64     v) { return v; }
		
	template<class T> static T from_f32(f32 v);
	template<class T> static T from_f64(f64 v);
		
	template<class T> static T mul_f32(const T& v, f32 r) { return from_f32<T>(to_f32(v) * r); }
	template<class T> static T mul_f64(const T& v, f64 r) { return from_f64<T>(to_f64(v) * r); }
		
	template<class T> AX_INLINE static T grayscale(T r, T g, T b) {
		return from_f32(to_f32(r) * 0.299f + to_f32(g) * 0.587f + to_f32(b) * 0.114f);
	}
		
};

template<> AX_INLINE UNorm8  ColorElemUtil::from_f32<UNorm8 >(f32 v) { return  UNorm8::s_fromFloat(v); }
template<> AX_INLINE UNorm16 ColorElemUtil::from_f32<UNorm16>(f32 v) { return UNorm16::s_fromFloat(v); }
template<> AX_INLINE UNorm32 ColorElemUtil::from_f32<UNorm32>(f32 v) { return UNorm32::s_fromFloat(v); }
template<> AX_INLINE f16     ColorElemUtil::from_f32<f16    >(f32 v) { return     f16::s_fromFloat(v); }
template<> AX_INLINE f32     ColorElemUtil::from_f32<f32    >(f32 v) { return v; }
template<> AX_INLINE f64     ColorElemUtil::from_f32<f64    >(f32 v) { return static_cast<f64>(v); }

template<> AX_INLINE UNorm8  ColorElemUtil::from_f64<UNorm8 >(f64 v) { return  UNorm8::s_fromFloat(v); }
template<> AX_INLINE UNorm16 ColorElemUtil::from_f64<UNorm16>(f64 v) { return UNorm16::s_fromFloat(v); }
template<> AX_INLINE UNorm32 ColorElemUtil::from_f64<UNorm32>(f64 v) { return UNorm32::s_fromFloat(v); }
template<> AX_INLINE f16     ColorElemUtil::from_f64<f16    >(f64 v) { return     f16::s_fromFloat(v); }
template<> AX_INLINE f32     ColorElemUtil::from_f64<f32    >(f64 v) { return static_cast<f32>(v); }
template<> AX_INLINE f64     ColorElemUtil::from_f64<f64    >(f64 v) { return v; }

template<class DST, class SRC> AX_INLINE constexpr
DST ColorElemUtil::s_cast(const SRC& src) {
	if constexpr (std::is_same_v<DST, SRC>) {
		return src; // same type

	} else if constexpr (std::is_floating_point_v<DST>) { // to float
		if constexpr (std::is_floating_point_v<SRC>) {
			return static_cast<DST>(src);
		} else {
			return static_cast<DST>(src.toFloat());
		}

	} else { // to f16 or UNorm<T>
		if constexpr (std::is_floating_point_v<SRC>) {
			return DST::s_fromFloat(src);
		} else {
			return DST::s_fromFloat(src.toFloat());
		}
	}
}

#if 0
#pragma mark ------------------ ColorModel ------------------
#endif

AX_ENUM_CLASS(AX_ColorModel_ENUM_LIST, ColorModel, u8);


#if 0
#pragma mark ------------------ ColorType ------------------
#endif


enum class ColorType : u32;
struct ColorTypeInfo;

using ColorType_IntType = Type_EnumInt<ColorType>;

AX_INLINE constexpr ColorType ColorType_make(ColorModel model, ColorElem elem) { 
	return static_cast<ColorType>( ax_enum_int(elem) << 8 | ax_enum_int(model) );
}

template<class E>
AX_INLINE constexpr ColorType ColorType_make(ColorModel model) {
	return ColorType_make(model, ColorElem_get<E>());
}

AX_INLINE constexpr ColorType_IntType ColorType_int(ColorModel model, ColorElem elem) {
	return static_cast<ColorType_IntType>(ColorType_make(model, elem));
}


AX_ENUM_CLASS(AX_ColorType_ENUM_LIST, ColorType, ColorType_IntType);

template<ColorModel MODEL, class ELEM, VecSimd SIMD = VecSimd_Default> class Color_;

//e.g. ColorRGBAf = Color_<ColorModel::RGBA, f32>;
#define E(MODEL,...) \
	template<class T, VecSimd SIMD = VecSimd_Default> using Color## MODEL ##_ = Color_<ColorModel::MODEL, T, SIMD>; \
	using Color## MODEL ##b = Color_<ColorModel::MODEL, UNorm8 >; \
	using Color## MODEL ##s = Color_<ColorModel::MODEL, UNorm16>; \
	using Color## MODEL ##h = Color_<ColorModel::MODEL, f16    >; \
	using Color## MODEL ##f = Color_<ColorModel::MODEL, f32    >; \
	using Color## MODEL ##d = Color_<ColorModel::MODEL, f64    >; \
//---
	AX_ColorModel_Basic_ENUM_LIST(E)
#undef E

template<class ELEM, VecSimd SIMD = VecSimd_Default>
using ColorPacked_  = Color_<ColorModel::Packed, ELEM, SIMD>;

using ColorR5G6B5      = ColorPacked_<ColorElem_R5G6B5     >;
using ColorR5G5B5A1    = ColorPacked_<ColorElem_R5G5B5A1   >;
using ColorR11G11B10   = ColorPacked_<ColorElem_R11G11B10  >;
using ColorR10G10B10A2 = ColorPacked_<ColorElem_R10G10B10A2>;

template<class ELEM, VecSimd SIMD = VecSimd_Default>
using ColorDXT_  = Color_<ColorModel::DXT, ELEM, SIMD>;
using ColorDXT_BC1  = ColorDXT_<ColorElem_DXT_BC1 >;
using ColorDXT_BC2  = ColorDXT_<ColorElem_DXT_BC2 >;
using ColorDXT_BC3  = ColorDXT_<ColorElem_DXT_BC3 >;
using ColorDXT_BC4  = ColorDXT_<ColorElem_DXT_BC4 >;
using ColorDXT_BC5  = ColorDXT_<ColorElem_DXT_BC5 >;
using ColorDXT_BC6h = ColorDXT_<ColorElem_DXT_BC6h>;
using ColorDXT_BC7  = ColorDXT_<ColorElem_DXT_BC7 >;

// Info for run-time
struct ColorTypeInfo {
	ColorModel colorModel			= ColorModel::None;
	ColorElem  colorElem			= ColorElem::None;
	ColorType  colorType			= ColorType::None;
	Int		   elementCount			= 0;
	Int		   alphaBits			= 0;
	Int		   sizeInBytes			= 0;
	Int		   compressedBlockSize	= 0;
	ColorType  uncompressedType	= ColorType::None;

	explicit operator bool() const { return colorType != ColorType::None; }

	static const ColorTypeInfo&	s_get(ColorType t);
};

// C++ Type -> ColorType
template<class T> struct ColorType_get_;

template<class T> constexpr ColorType ColorType_get = ColorType_get_<T>::value;

// ColorType -> C++ Type
template<ColorType t> struct ColorType_Type_;
template<ColorType t> using  ColorType_Type = typename ColorType_Type_<t>::Type;

// #define E(T, ...) \
// 	template<> struct ColorType_Type_<ColorType::T>	{ using Type = Color##T; }; \
// 	template<> struct ColorType_get_<Color##T> { static constexpr ColorType value = ColorType::T; }; \
// //----
// 	AX_ColorType_Basic_ENUM_LIST(E)
// #undef E
//
// #define E(T, ...) \
// 	template<> struct ColorType_Type_<ColorType::T>	{ using Type = Color##T; }; \
// 	template<> struct ColorType_get_<Color##T> { static constexpr ColorType value = ColorType::T; }; \
// //----
// 	AX_ColorType_NonBasic_ENUM_LIST(E)
// #undef E


template<class T> constexpr bool IsColor_ = false;

template<ColorModel MODEL, class ELEM>
constexpr bool IsColor_< Color_<MODEL, ELEM> > = true;



} // namespace ax
