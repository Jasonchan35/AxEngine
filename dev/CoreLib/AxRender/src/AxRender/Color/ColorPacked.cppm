module;

export module AxRender.ColorPacked;
export import AxRender.ColorRGBA;

export namespace ax {

template<>
class Color_<ColorModel::Packed, ColorElem_EnumAsType<ColorElem::R5G6B5> > {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::Packed;
	static constexpr ColorElem	kColorElem	= ColorElem::R5G6B5;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount = 3;
	static constexpr Int kAlphaBits = 0;

	u16 value;
};

template<>
class Color_<ColorModel::Packed, ColorElem_EnumAsType<ColorElem::R5G5B5A1> > {
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::Packed;
	static constexpr ColorElem	kColorElem	= ColorElem::R5G5B5A1;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount = 4;
	static constexpr Int kAlphaBits = 1;

	u16 value;
};

template<>
class Color_<ColorModel::Packed, ColorElem_EnumAsType<ColorElem::R11G11B10> > {
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::Packed;
	static constexpr ColorElem	kColorElem	= ColorElem::R11G11B10;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount = 3;
	static constexpr Int kAlphaBits = 1;

	u32 value;
};

template<>
class Color_<ColorModel::Packed, ColorElem_EnumAsType<ColorElem::R10G10B10A2> > {
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::Packed;
	static constexpr ColorElem	kColorElem	= ColorElem::R10G10B10A2;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount = 4;
	static constexpr Int kAlphaBits = 1;

	u32 value;
};


} // namespace ax