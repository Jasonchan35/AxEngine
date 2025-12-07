module;

module AxRender.ColorType;
import AxRender.ColorRGBA;
import AxRender.ColorHSVA;
import AxRender.ColorPacked;
import AxRender.ColorDXT;

namespace ax {

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

// const ColorTypeInfo& ColorTypeInfo::s_get(ColorType t) {
// 	switch (t) {
// 	#define E(T,...) \
// 		case ColorType::T: { static ColorTypeInfo_Creator<Color##T> s; return s; } break;
// 	//----
// 		AX_ColorType_ENUM_LIST_WITHOUT_NONE(E)
// 	#undef E
// 		default: {
// 			AX_ASSERT(false);
// 			static ColorTypeInfo none;
// 			return none;
// 		} break;
// 	}
// }


} // namespace