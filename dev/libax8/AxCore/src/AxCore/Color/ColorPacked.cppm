module;

export module AxCore.Color:ColorPacked;
export import :ColorRGBA;

export namespace ax {

template<>
class Color_<ColorModel::Packed, ColorElem_R5G6B5> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	union {
		struct {
			u16 r : 5;
			u16 g : 6;
			u16 b : 5;
		};
		u16 value;
	};
	
	static constexpr Int		kElementCount	= 3;
	static constexpr ColorModel kColorModel 	= ColorModel::Packed;
	static constexpr ColorElem	kColorElem		= ColorElem::R5G6B5;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kAlphaBits		= 0;
};

template<>
class Color_<ColorModel::Packed, ColorElem_R5G5B5A1> {
public:
	union {
		struct {
			u16 r : 5;
			u16 g : 5;
			u16 b : 5;
			u16 a : 1;
		};
		u16 value;
	};

	static constexpr Int		kElementCount	= 4;
	static constexpr ColorModel kColorModel		= ColorModel::Packed;
	static constexpr ColorElem	kColorElem		= ColorElem::R5G5B5A1;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kAlphaBits		= 1;
};

template<>
class Color_<ColorModel::Packed, ColorElem_R11G11B10> {
public:
	union {
		struct {
			u32 r : 11;
			u32 g : 11;
			u32 b : 10;
		};
		u32 value;
	};
	
	static constexpr Int		kElementCount	= 3;
	static constexpr ColorModel kColorModel = ColorModel::Packed;
	static constexpr ColorElem	kColorElem	= ColorElem::R11G11B10;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kAlphaBits	= 0;
};

template<>
class Color_<ColorModel::Packed, ColorElem_R10G10B10A2> {
public:
	union {
		struct {
			u32 r : 10;
			u32 g : 10;
			u32 b : 10;
			u32 a : 2;
		};
		u32 value;
	};	
	
	using Element = void;

	static constexpr Int		kElementCount	= 4;
	static constexpr ColorModel kColorModel 	= ColorModel::Packed;
	static constexpr ColorElem	kColorElem		= ColorElem::R10G10B10A2;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kAlphaBits		= 2;
};


} // namespace ax