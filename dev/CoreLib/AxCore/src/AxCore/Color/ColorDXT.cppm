module;

export module AxCore.Color:ColorDXT;
export import :ColorRGBA;

export namespace ax {

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC1, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = void;

	static constexpr ColorModel kColorModel		  = ColorModel::DXT;
	static constexpr ColorElem	kColorElem		  = ColorElem::DXT_BC1;
	static constexpr ColorType	kColorType		  = ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	  = 4; // R5 G6 B5 A1
	static constexpr Int		kAlphaBits		  = 1;

	static constexpr Int		kCompressedBlockSize = 8;
	static constexpr ColorType	kUncompressedType = ColorType::RGBAb;
};

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC2, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::DXT;
	static constexpr ColorElem	kColorElem	= ColorElem::DXT_BC2;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount	   = 4; // R5 G6 B5 (BC1) + A4
	static constexpr Int kAlphaBits		   = 4;
	static constexpr Int kCompressedBlockSize = 16;
	static constexpr ColorType	kUncompressedType = ColorType::RGBAb;
};

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC3, SIMD> {
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::DXT;
	static constexpr ColorElem	kColorElem	= ColorElem::DXT_BC3;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount	   = 4; // R5 G6 B5 (BC1) + A8 (BC4)
	static constexpr Int kAlphaBits		   = 8;
	static constexpr Int kCompressedBlockSize = 16;
	static constexpr ColorType	kUncompressedType = ColorType::RGBAb;
};

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC4, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::DXT;
	static constexpr ColorElem	kColorElem	= ColorElem::DXT_BC4;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount	   = 1; // Grayscale 8 bit
	static constexpr Int kAlphaBits		   = 0;
	static constexpr Int kCompressedBlockSize = 8;
	static constexpr ColorType	kUncompressedType = ColorType::Lb;
};

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC5, SIMD> {
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::DXT;
	static constexpr ColorElem	kColorElem	= ColorElem::DXT_BC5;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount	   = 2; // R8 (BC4) + G8 (BC4)
	static constexpr Int kAlphaBits		   = 0;
	static constexpr Int kCompressedBlockSize = 16;
	static constexpr ColorType	kUncompressedType = ColorType::RGb;
};

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC6h, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::DXT;
	static constexpr ColorElem	kColorElem	= ColorElem::DXT_BC6h;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount	   = 4; // RGBh
	static constexpr Int kAlphaBits		   = 0;

	static constexpr Int kCompressedBlockSize = 16;
	static constexpr ColorType	kUncompressedType = ColorType::RGBh;
};

template<VecSIMD SIMD>
class Color_<ColorModel::DXT, ColorElem_DXT_BC7, SIMD> {
public:
	using Element = void;

	static constexpr ColorModel kColorModel = ColorModel::DXT;
	static constexpr ColorElem	kColorElem	= ColorElem::DXT_BC7;
	static constexpr ColorType	kColorType	= ColorType_make(kColorModel, kColorElem);

	static constexpr Int kElementCount	   = 4; // RGBb or RGBAb
	static constexpr Int kAlphaBits		   = 8;

	static constexpr Int kCompressedBlockSize = 16;
	static constexpr ColorType	kUncompressedType = ColorType::RGBAb;

};

} // namespace ax