module;

module AxRender.ColorType;
import AxRender.ColorRGBA;
import AxRender.ColorHSBA;
import AxRender.ColorPacked;
import AxRender.ColorDXT;

namespace ax {

template<class T>
struct ColorTypeInfo_Creator : public ColorTypeInfo {
	ColorTypeInfo_Creator() {
		sizeInBytes			= AX_SIZEOF(T);

		colorModel			= T::kColorModel;
		colorElem			= T::kColorElem;
		colorType			= T::kColorType;

		elementCount		= T::kElementCount;
		alphaBits			= T::kAlphaBits;

		if constexpr (T::kColorModel == ColorModel::DXT) {
			compressedBlockSize = T::kCompressedBlockSize;
			uncompressedType	= T::kUncompressedType;
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