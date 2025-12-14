module;

module AxCore.Color;
import :ColorType;
import :ColorRGBA;
import :ColorHSVA;
import :ColorPacked;
import :ColorDXT;

namespace ax {

template<class T>
struct ColorTypeInfo_Creator : public ColorTypeInfo {
	using Base = ColorTypeInfo;
	
	constexpr ColorTypeInfo_Creator() {
		Base::sizeInBytes	= AX_SIZEOF(T);
		Base::colorModel	= T::kColorModel;
		Base::colorElem		= T::kColorElem;
		Base::colorType		= T::kColorType;
		Base::elementCount	= T::kElementCount;
		Base::alphaBits		= T::kAlphaBits;

		if constexpr (T::kColorModel == ColorModel::DXT) {
			Base::compressedBlockSize	= T::kCompressedBlockSize;
			Base::uncompressedType		= T::kUncompressedType;
		}

		static_assert(T::kColorType == ColorType_make(T::kColorModel, T::kColorElem));
	}
};

const ColorTypeInfo& ColorTypeInfo::s_get(ColorType t) {
	// work around for MSVC
	//   - use of undefined type 'ax::Color_<ax::ColorModel::Packed,ax::ColorElem_R5G6B5,ax::VecSimd::SSE>'
	// by adding static_assert() below, somehow could pass the compilation 
	static_assert(Type_IsSame<Color_<ColorModel::Packed, ColorElem_R5G5B5A1   >, ColorR5G5B5A1   >);
	static_assert(Type_IsSame<Color_<ColorModel::Packed, ColorElem_R5G6B5     >, ColorR5G6B5     >);
	static_assert(Type_IsSame<Color_<ColorModel::Packed, ColorElem_R11G11B10  >, ColorR11G11B10  >);
	static_assert(Type_IsSame<Color_<ColorModel::Packed, ColorElem_R10G10B10A2>, ColorR10G10B10A2>);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC1    >, ColorDXT_BC1    >);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC2    >, ColorDXT_BC2    >);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC3    >, ColorDXT_BC3    >);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC4    >, ColorDXT_BC4    >);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC5    >, ColorDXT_BC5    >);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC6h   >, ColorDXT_BC6h   >);
	static_assert(Type_IsSame<Color_<ColorModel::DXT,    ColorElem_DXT_BC7    >, ColorDXT_BC7    >);
	//----
	
	switch (t) {
	#define E(T,...) case ColorType::T: { \
		static ColorTypeInfo_Creator<Color##T> s; return s; \
	} break; \
	//--------
	 	AX_ColorType_ENUM_LIST_WITHOUT_NONE(E)
	#undef E
		default: {
			AX_ASSERT(false);
			static ColorTypeInfo none;
			return none;
		} break;
	}
}

} // namespace