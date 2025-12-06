#pragma once

namespace  ax {

/*
R = Red
G = Green
B = Blue
A = Alpha

H = Hue
S = Saturation
B = Brightness

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

#define AX_ColorElem_Combined_ENUM_LIST(E) \
	E(R5G6B5, )                            \
	E(R5G5B5A1, )                          \
	E(R11G11B10, )                         \
	E(R10G10B10A2, )                       \
//----

#define AX_ColorElem_ENUM_LIST_WITHOUT_NONE(E) \
	AX_ColorElem_Number_ENUM_LIST(E) \
	AX_ColorElem_DXT_ENUM_LIST(E) \
	AX_ColorElem_Combined_ENUM_LIST(E) \
//-------
#define AX_ColorElem_ENUM_LIST(E) \
	E(None,,) \
	AX_ColorElem_ENUM_LIST_WITHOUT_NONE(E) \
//-------

} // namespace

