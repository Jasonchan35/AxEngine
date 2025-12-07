module;

#include "ColorType.h"

export module AxRender.ColorType;
export import AxRender.PCH;

export namespace ax {

#if 0
#pragma mark ------------------ ColorElem ------------------
#endif

AX_ENUM_CLASS(AX_ColorElem_ENUM_LIST, ColorElem, u8)

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

	template<class T> static T from_f32(f32 v);
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


#define AX_ColorModel_Basic_ENUM_LIST(E) \
E(RGBA, )                                   \
E(RGB, )                                    \
E(RG, )                                     \
E(R, )                                      \
E(LA, )                                     \
E(L, )                                      \
E(A, )                                      \
E(HSBA, )                                   \
E(BGR, )                                    \
E(BGRA, )                                   \
//-------

#define AX_ColorModel_ENUM_LIST_WITHOUT_NONE(E) \
AX_ColorModel_Basic_ENUM_LIST(E) \
E(Packed,) \
E(DXT,) \
AX_ColorElem_Combined_ENUM_LIST(E) \
//----

#define AX_ColorModel_ENUM_LIST(E) \
E(None,) \
AX_ColorModel_ENUM_LIST_WITHOUT_NONE(E) \
//---
AX_ENUM_CLASS(AX_ColorModel_ENUM_LIST, ColorModel, u8);


#if 0
#pragma mark ------------------ ColorType ------------------
#endif


enum class ColorType : u32;
struct ColorTypeInfo;

using ColorType_IntType = Enum<ColorType>::IntType;

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

#define AX_ColorType_Basic_ENUM_LIST(E) \
	E(RGBAb, = ColorType_int(ColorModel::RGBA, ColorElem::UNorm8 )) \
	E(RGBAs, = ColorType_int(ColorModel::RGBA, ColorElem::UNorm16)) \
	E(RGBAh, = ColorType_int(ColorModel::RGBA, ColorElem::f16    )) \
	E(RGBAf, = ColorType_int(ColorModel::RGBA, ColorElem::f32    )) \
	E(RGBAd, = ColorType_int(ColorModel::RGBA, ColorElem::f64    )) \
	\
	E( RGBb, = ColorType_int(ColorModel::RGB , ColorElem::UNorm8 )) \
	E( RGBs, = ColorType_int(ColorModel::RGB , ColorElem::UNorm16)) \
	E( RGBh, = ColorType_int(ColorModel::RGB , ColorElem::f16    )) \
	E( RGBf, = ColorType_int(ColorModel::RGB , ColorElem::f32    )) \
	E( RGBd, = ColorType_int(ColorModel::RGB , ColorElem::f64    )) \
	\
	E(  RGb, = ColorType_int(ColorModel::RG  , ColorElem::UNorm8 )) \
	E(  RGs, = ColorType_int(ColorModel::RG  , ColorElem::UNorm16)) \
	E(  RGh, = ColorType_int(ColorModel::RG  , ColorElem::f16    )) \
	E(  RGf, = ColorType_int(ColorModel::RG  , ColorElem::f32    )) \
	E(  RGd, = ColorType_int(ColorModel::RG  , ColorElem::f64    )) \
	\
	E(   Rb, = ColorType_int(ColorModel::R   , ColorElem::UNorm8 )) \
	E(   Rs, = ColorType_int(ColorModel::R   , ColorElem::UNorm16)) \
	E(   Rh, = ColorType_int(ColorModel::R   , ColorElem::f16    )) \
	E(   Rf, = ColorType_int(ColorModel::R   , ColorElem::f32    )) \
	E(   Rd, = ColorType_int(ColorModel::R   , ColorElem::f64    )) \
	\
	E(   Ab, = ColorType_int(ColorModel::A   , ColorElem::UNorm8 )) \
	E(   As, = ColorType_int(ColorModel::A   , ColorElem::UNorm16)) \
	E(   Ah, = ColorType_int(ColorModel::A   , ColorElem::f16    )) \
	E(   Af, = ColorType_int(ColorModel::A   , ColorElem::f32    )) \
	E(   Ad, = ColorType_int(ColorModel::A   , ColorElem::f64    )) \
	\
	E(   Lb, = ColorType_int(ColorModel::L   , ColorElem::UNorm8 )) \
	E(   Ls, = ColorType_int(ColorModel::L   , ColorElem::UNorm16)) \
	E(   Lh, = ColorType_int(ColorModel::L   , ColorElem::f16    )) \
	E(   Lf, = ColorType_int(ColorModel::L   , ColorElem::f32    )) \
	E(   Ld, = ColorType_int(ColorModel::L   , ColorElem::f64    )) \
	\
	E(  LAb, = ColorType_int(ColorModel::LA  , ColorElem::UNorm8 )) \
	E(  LAs, = ColorType_int(ColorModel::LA  , ColorElem::UNorm16)) \
	E(  LAh, = ColorType_int(ColorModel::LA  , ColorElem::f16    )) \
	E(  LAf, = ColorType_int(ColorModel::LA  , ColorElem::f32    )) \
	E(  LAd, = ColorType_int(ColorModel::LA  , ColorElem::f64    )) \
	\
	E(HSBAb, = ColorType_int(ColorModel::HSBA, ColorElem::UNorm8 )) \
	E(HSBAs, = ColorType_int(ColorModel::HSBA, ColorElem::UNorm16)) \
	E(HSBAh, = ColorType_int(ColorModel::HSBA, ColorElem::f16    )) \
	E(HSBAf, = ColorType_int(ColorModel::HSBA, ColorElem::f32    )) \
	E(HSBAd, = ColorType_int(ColorModel::HSBA, ColorElem::f64    )) \
//----

#define AX_ColorType_Packed_ENUM_LIST(E) \
	E(R5G6B5,      = ColorType_int(ColorModel::Packed, ColorElem::R5G6B5     )) \
	E(R5G5B5A1,    = ColorType_int(ColorModel::Packed, ColorElem::R5G5B5A1   )) \
	E(R11G11B10,   = ColorType_int(ColorModel::Packed, ColorElem::R11G11B10  )) \
	E(R10G10B10A2, = ColorType_int(ColorModel::Packed, ColorElem::R10G10B10A2)) \
//-----

#define AX_ColorType_Compressed_ENUM_LIST(E) \
	E(DXT_BC1  , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC1 )) \
	E(DXT_BC2  , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC2 )) \
	E(DXT_BC3  , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC3 )) \
	E(DXT_BC4  , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC4 )) \
	E(DXT_BC5  , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC5 )) \
	E(DXT_BC6h , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC6h)) \
	E(DXT_BC7  , = ColorType_int(ColorModel::DXT, ColorElem::DXT_BC7 )) \
	//\
	//E(PVRTC,) \
	//E(ASTC,) \
//-----

#define AX_ColorType_NonBasic_ENUM_LIST(E) \
	AX_ColorType_Packed_ENUM_LIST(E) \
	AX_ColorType_Compressed_ENUM_LIST(E) \
//----	

#define AX_ColorType_ENUM_LIST_WITHOUT_NONE(E) \
	AX_ColorType_Basic_ENUM_LIST(E) \
	AX_ColorType_NonBasic_ENUM_LIST(E) \
//-------
#define AX_ColorType_ENUM_LIST(E) \
	E(None,)	\
	AX_ColorType_ENUM_LIST_WITHOUT_NONE(E) \
//-------
AX_ENUM_CLASS(AX_ColorType_ENUM_LIST, ColorType, ColorType_IntType);

template<ColorModel MODEL, class ELEM, VecSIMD SIMD = VecSIMD_Default> class Color_;

//e.g. ColorRGBAf = Color_<ColorModel::RGBA, f32>;
#define E(MODEL,...) \
	template<class T> using Color## MODEL ##_ = Color_<ColorModel::MODEL, T>; \
	using Color## MODEL ##b = Color_<ColorModel::MODEL, UNorm8 >; \
	using Color## MODEL ##s = Color_<ColorModel::MODEL, UNorm16>; \
	using Color## MODEL ##h = Color_<ColorModel::MODEL, f16    >; \
	using Color## MODEL ##f = Color_<ColorModel::MODEL, f32    >; \
	using Color## MODEL ##d = Color_<ColorModel::MODEL, f64    >; \
//---
	AX_ColorModel_Basic_ENUM_LIST(E)
#undef E

template<ColorElem E>
struct ColorElem_EnumAsType {
	static constexpr ColorElem kColorElem = E;
};

template<ColorElem E>
using ColorPacked_  = Color_<ColorModel::Packed, ColorElem_EnumAsType<E>>;

using ColorR5G6B5      = ColorPacked_<ColorElem::R5G6B5     >;
using ColorR5G5B5A1    = ColorPacked_<ColorElem::R5G5B5A1   >;
using ColorR11G11B10   = ColorPacked_<ColorElem::R11G11B10  >;
using ColorR10G10B10A2 = ColorPacked_<ColorElem::R10G10B10A2>;

template<ColorElem E>
using ColorDXT_  = Color_<ColorModel::DXT, ColorElem_EnumAsType<E>>;

using ColorDXT_BC1  = ColorDXT_<ColorElem::DXT_BC1 >;
using ColorDXT_BC2  = ColorDXT_<ColorElem::DXT_BC2 >;
using ColorDXT_BC3  = ColorDXT_<ColorElem::DXT_BC3 >;
using ColorDXT_BC4  = ColorDXT_<ColorElem::DXT_BC4 >;
using ColorDXT_BC5  = ColorDXT_<ColorElem::DXT_BC5 >;
using ColorDXT_BC6h = ColorDXT_<ColorElem::DXT_BC6h>;
using ColorDXT_BC7  = ColorDXT_<ColorElem::DXT_BC7 >;

// Info for run-time
struct ColorTypeInfo {
	ColorModel colorModel		   = ColorModel::None;
	ColorElem  colorElem		   = ColorElem::None;
	ColorType  colorType		   = ColorType::None;
	Int		   elementCount		   = 0;
	Int		   alphaBits		   = 0;
	Int		   sizeInBytes		   = 0;
	Int		   compressedBlockSize = 0;
	ColorType  uncompressedType	   = ColorType::None;

	explicit operator bool() const { return colorType != ColorType::None; }

	static const ColorTypeInfo&	s_get(ColorType t);
};

// C++ Type -> ColorType
template<class T> struct ColorType_get_;

template<class T> constexpr ColorType ColorType_get = ColorType_get_<T>::value;

// ColorType -> C++ Type
template<ColorType t> struct ColorType_Type_;
template<ColorType t> using  ColorType_Type = typename ColorType_Type_<t>::Type;

#define E(T, ...) \
	template<> struct ColorType_Type_<ColorType::T>	{ using Type = Color##T; }; \
	template<> struct ColorType_get_<Color##T> { static constexpr ColorType value = ColorType::T; }; \
//----
	AX_ColorType_Basic_ENUM_LIST(E)
#undef E

#define E(T, ...) \
	template<> struct ColorType_Type_<ColorType::T>	{ using Type = Color##T; }; \
	template<> struct ColorType_get_<Color##T> { static constexpr ColorType value = ColorType::T; }; \
//----
	AX_ColorType_NonBasic_ENUM_LIST(E)
#undef E

template<class> struct _IsColor_ : std::false_type {};
template<ColorModel MODEL, class ELEM>
struct _IsColor_< Color_<MODEL, ELEM> > : std::true_type {};

template<class T> constexpr bool IsColor_ = _IsColor_<T>::value;



template<class T>
struct ColorTypeUtil {
	using ColorR    = ColorR_<T>;
	using ColorRG   = ColorRG_<T>;
	using ColorRGB  = ColorRGB_<T>;
	using ColorRGBA = ColorRGBA_<T>;
	
	template<class FUNC> AX_INLINE constexpr static ColorR    op1(const ColorR   & v0, FUNC func) { return ColorR   (func(v0.r)); }
	template<class FUNC> AX_INLINE constexpr static ColorRG   op1(const ColorRG  & v0, FUNC func) { return ColorRG  (func(v0.r), func(v0.g)); }
	template<class FUNC> AX_INLINE constexpr static ColorRGB  op1(const ColorRGB & v0, FUNC func) { return ColorRGB (func(v0.r), func(v0.g), func(v0.b)); }
	template<class FUNC> AX_INLINE constexpr static ColorRGBA op1(const ColorRGBA& v0, FUNC func) { return ColorRGBA(func(v0.r), func(v0.g), func(v0.b), func(v0.a)); }

	template<class FUNC> AX_INLINE constexpr static ColorR    op2(const ColorR   & v0, const ColorR   & v1, FUNC func) { return ColorR   (func(v0.r, v1.r)); }
	template<class FUNC> AX_INLINE constexpr static ColorRG   op2(const ColorRG  & v0, const ColorRG  & v1, FUNC func) { return ColorRG  (func(v0.r, v1.r), func(v0.g, v1.g)); }
	template<class FUNC> AX_INLINE constexpr static ColorRGB  op2(const ColorRGB & v0, const ColorRGB & v1, FUNC func) { return ColorRGB (func(v0.r, v1.r), func(v0.g, v1.g), func(v0.b, v1.b)); }
	template<class FUNC> AX_INLINE constexpr static ColorRGBA op2(const ColorRGBA& v0, const ColorRGBA& v1, FUNC func) { return ColorRGBA(func(v0.r, v1.r), func(v0.g, v1.g), func(v0.b, v1.b), func(v0.a, v1.a)); }

	template<class FUNC> AX_INLINE constexpr static ColorR    op3(const ColorR   & v0, const ColorR   & v1, const ColorR   & v2, FUNC func) { return ColorR   (func(v0.r, v1.r, v2.r)); }
	template<class FUNC> AX_INLINE constexpr static ColorRG   op3(const ColorRG  & v0, const ColorRG  & v1, const ColorRG  & v2, FUNC func) { return ColorRG  (func(v0.r, v1.r, v2.r), func(v0.g, v1.g, v2.g)); }
	template<class FUNC> AX_INLINE constexpr static ColorRGB  op3(const ColorRGB & v0, const ColorRGB & v1, const ColorRGB & v2, FUNC func) { return ColorRGB (func(v0.r, v1.r, v2.r), func(v0.g, v1.g, v2.g), func(v0.b, v1.b, v2.b)); }
	template<class FUNC> AX_INLINE constexpr static ColorRGBA op3(const ColorRGBA& v0, const ColorRGBA& v1, const ColorRGBA& v2, FUNC func) { return ColorRGBA(func(v0.r, v1.r, v2.r), func(v0.g, v1.g, v2.g), func(v0.b, v1.b, v2.b), func(v0.a, v1.a, v2.a)); }

	template<class FUNC> AX_INLINE constexpr static bool bool_and1(const ColorR   & v0, FUNC func) { return func(v0.r); }
	template<class FUNC> AX_INLINE constexpr static bool bool_and1(const ColorRG  & v0, FUNC func) { return func(v0.r) && func(v0.g); }
	template<class FUNC> AX_INLINE constexpr static bool bool_and1(const ColorRGB & v0, FUNC func) { return func(v0.r) && func(v0.g) && func(v0.b); }
	template<class FUNC> AX_INLINE constexpr static bool bool_and1(const ColorRGBA& v0, FUNC func) { return func(v0.r) && func(v0.g) && func(v0.b) && func(v0.a); }

	template<class FUNC> AX_INLINE constexpr static bool bool_and2(const ColorR   & v0, const ColorR   & v1, FUNC func) { return func(v0.r, v1.r); }
	template<class FUNC> AX_INLINE constexpr static bool bool_and2(const ColorRG  & v0, const ColorRG  & v1, FUNC func) { return func(v0.r, v1.r) && func(v0.g, v1.g); }
	template<class FUNC> AX_INLINE constexpr static bool bool_and2(const ColorRGB & v0, const ColorRGB & v1, FUNC func) { return func(v0.r, v1.r) && func(v0.g, v1.g) && func(v0.b, v1.b); }
	template<class FUNC> AX_INLINE constexpr static bool bool_and2(const ColorRGBA& v0, const ColorRGBA& v1, FUNC func) { return func(v0.r, v1.r) && func(v0.g, v1.g) && func(v0.b, v1.b) && func(v0.a, v1.a); }
};

} // namespace ax

namespace ax::Math {

template<ColorModel M, class T> AX_INLINE constexpr
bool almostEqual(const Color_<M,T>& v0, const Color_<M,T>& v1, const T& ep = epsilon<T>) {
	return ColorTypeUtil<T>::bool_and2(v0, v1,
		[&](const T& e0, const T& e1) -> bool { return almostEqual(e0, e1, ep); });
}

template<ColorModel M, class T> AX_INLINE constexpr
bool exactlyEqual(const Color_<M,T>& v0, const Color_<M,T>& v1) {
	return ColorTypeUtil<T>::bool_and2(v0, v1,
		[&](const T& e0, const T& e1) -> bool { return exactlyEqual(e0, e1); });
}

} // namespace ax::Math