module;

module AxRender;
import :ColorRGBA;
import :ColorHSVA;
import :ColorPacked;
import :ColorDXT;

namespace ax {

/*---- The explicit instantiation ---*/

//e.g. Color_<ColorModel::RGBA, f32>;
// #define E(MODEL,...) \
// 	template class Color_<ColorModel::MODEL, UNorm8 >; \
// 	template class Color_<ColorModel::MODEL, UNorm16>; \
// 	template class Color_<ColorModel::MODEL, f16    >; \
// 	template class Color_<ColorModel::MODEL, f32    >; \
// 	template class Color_<ColorModel::MODEL, f64    >; \
// //---
// 	AX_ColorModel_Basic_ENUM_LIST(E)
// #undef E

// template class Color_<ColorModel::RGBA, UNorm8 >;
// template class Color_<ColorModel::RGBA, UNorm16>;
// template class Color_<ColorModel::RGBA, f16    >;
// template class Color_<ColorModel::RGBA, f32    >;
// template class Color_<ColorModel::RGBA, f64    >;

// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC1 		>;
// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC2 		>;
// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC3 		>;
// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC4 		>;
// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC5 		>;
// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC6h		>;
// template class Color_<ColorModel::DXT,		ColorElem_DXT_BC7 		>;
// 
// template class Color_<ColorModel::Packed,	ColorElem_R5G6B5     	>;
// template class Color_<ColorModel::Packed,	ColorElem_R5G5B5A1   	>;
// template class Color_<ColorModel::Packed,	ColorElem_R11G11B10  	>;
// template class Color_<ColorModel::Packed,	ColorElem_R10G10B10A2	>;

template<class T>
struct ColorTypeInfo_Creator : public ColorTypeInfo {
	ColorTypeInfo_Creator() {
		kSizeInBytes		= AX_SIZEOF(T);

		kColorModel			= T::kColorModel;
		kColorElem			= T::kColorElem;
		kColorType			= T::kColorType;

		kElementCount		= T::kElementCount;
		kAlphaBits			= T::kAlphaBits;

		if constexpr (T::kColorModel == ColorModel::DXT) {
			kCompressedBlockSize	= T::kCompressedBlockSize;
			kUncompressedType		= T::kUncompressedType;
		}

		static_assert(T::kColorType == ColorType_make(T::kColorModel, T::kColorElem));
	}
};

const ColorTypeInfo& ColorTypeInfo::s_get(ColorType t) {
#if 1 //TODO
	static ColorTypeInfo none;
	return none;
#else
	switch (t) {
	#define E(T,...) \
		case ColorType::T: { static ColorTypeInfo_Creator<Color##T> s; return s; } break;
	//----
		AX_ColorType_ENUM_LIST_WITHOUT_NONE(E)
	#undef E
		default: {
			AX_ASSERT(false);
			static ColorTypeInfo none;
			return none;
		} break;
	}
#endif
}

} // namespace