#pragma once

namespace  ax {

#if 0
#pragma mark ----------- ColorElem -----------------------
#endif

/*
R = Red
G = Green
B = Blue
A = Alpha

H = Hue
S = Saturation
V = Value

L = Luminance / Gray

f = float
b = Byte
*/
#define AX_ColorElem_Number_ENUM_LIST(E) \
	E(f16       ,, h    ) /* half   */ \
	E(f32       ,, f    ) /* float  */ \
	E(f64       ,, d    ) /* double */ \
	E(UNorm8    ,, b    ) /* byte   */ \
	E(UNorm16   ,, s    ) \
//	E(Int8      ,, i8  ) \
//	E(Int16     ,, i16 ) \
//	E(Int32     ,, i32 ) \
//	E(UInt8     ,, u8  ) \
//	E(UInt16    ,, u16 ) \
//	E(UInt32    ,, u32 ) \
//---

#define AX_ColorElem_DXT_ENUM_LIST(E) \
	E(DXT_BC1   ,, _BC1 ) \
	E(DXT_BC2   ,, _BC2 ) \
	E(DXT_BC3   ,, _BC3 ) \
	E(DXT_BC4   ,, _BC4 ) \
	E(DXT_BC5   ,, _BC5 ) \
	E(DXT_BC6h  ,, _BC6h) \
	E(DXT_BC7   ,, _BC7 ) \
//----

#define AX_ColorElem_Packed_ENUM_LIST(E) \
	E(R5G6B5, )                            \
	E(R5G5B5A1, )                          \
	E(R11G11B10, )                         \
	E(R10G10B10A2, )                       \
//----

#define AX_ColorElem_ENUM_LIST_WITHOUT_NONE(E) \
	AX_ColorElem_Number_ENUM_LIST(E) \
	AX_ColorElem_DXT_ENUM_LIST(E) \
	AX_ColorElem_Packed_ENUM_LIST(E) \
//-------
#define AX_ColorElem_ENUM_LIST(E) \
	E(None,,) \
	AX_ColorElem_ENUM_LIST_WITHOUT_NONE(E) \
//-------

#if 0
#pragma mark ----------- ColorModel -----------------------
#endif

#define AX_ColorModel_ENUM_LIST_WITHOUT_NONE(E) \
	AX_ColorModel_Basic_ENUM_LIST(E) \
	E(Packed,) \
	E(DXT,) \
	AX_ColorElem_Packed_ENUM_LIST(E) \
//----

#define AX_ColorModel_ENUM_LIST(E) \
	E(None,) \
	AX_ColorModel_ENUM_LIST_WITHOUT_NONE(E) \
//---

#if 0
#pragma mark ----------- ColorType -----------------------
#endif

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
	E(HSVAb, = ColorType_int(ColorModel::HSVA, ColorElem::UNorm8 )) \
	E(HSVAs, = ColorType_int(ColorModel::HSVA, ColorElem::UNorm16)) \
	E(HSVAh, = ColorType_int(ColorModel::HSVA, ColorElem::f16    )) \
	E(HSVAf, = ColorType_int(ColorModel::HSVA, ColorElem::f32    )) \
	E(HSVAd, = ColorType_int(ColorModel::HSVA, ColorElem::f64    )) \
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
//
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

#define AX_ColorModel_Basic_ENUM_LIST(E) \
	E(RGBA, )                                   \
	E(RGB, )                                    \
	E(RG, )                                     \
	E(R, )                                      \
	E(LA, )                                     \
	E(L, )                                      \
	E(A, )                                      \
	E(HSVA, )                                   \
	E(BGR, )                                    \
	E(BGRA, )                                   \
//-------

} // namespace

