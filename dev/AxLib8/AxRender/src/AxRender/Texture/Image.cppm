module;
export module AxRender:Image;
export import :RenderDataType;

export namespace ax {

template<class COLOR> class ImagePainter_;

#define AX_RENDER_ImageFileType_ENUM_LIST(E) \
	E(None,	) \
	E(PNG,	) \
	E(JPEG,	) \
	E(DDS,	) \
//----
AX_ENUM_FLAGS_CLASS(AX_RENDER_ImageFileType_ENUM_LIST, ImageFileType, u16)

ImageFileType ImageFileType_fromFileExt(StrView ext);

class ImageInfo {
public:
	ColorType	colorType = ColorType::None;
	Vec2i		size = TagZero;
	Int			strideInBytes = 0;
	Int			mipLevels = 1;

	ImageInfo() =default;

	ImageInfo(ColorType colorType_, const Vec2i& size_, Int mipLevels_ = 1)
		: colorType(colorType_), size(size_), mipLevels(mipLevels_)
	{
		computeDataSize();
	}

	Int			pixelSizeInBytes() const { return ColorTypeInfo::s_get(colorType).sizeInBytes; }
	Int			computeDataSize();
};

struct Image_CreateDesc : public NonCopyable {
	ImageInfo	info;
	Int			dataSize = 0;
};

//! Image for any color type
class Image : public NonCopyable {
public:
	using Info = ImageInfo;
	using CreateDesc = Image_CreateDesc;

			void	create(ColorType colorType, Vec2i size, Int strideInByte);
			void	create(ColorType colorType, Vec2i size);
			void	create(const CreateDesc& desc);
			void	clear();

			void	loadFile(StrView filename, ImageFileType fileType = ImageFileType::None);
			void	loadMem(ByteSpan inData,   ImageFileType fileType);

	AX_INLINE	ColorType		colorType		() const { return _info.colorType; }
	AX_INLINE	Vec2i			size			() const { return _info.size; }
	AX_INLINE	Int				width			() const { return _info.size.x; }
	AX_INLINE	Int				height			() const { return _info.size.y; }
	AX_INLINE	Int				mipLevels		() const { return _info.mipLevels; }
	AX_INLINE	Int				strideInBytes	() const { return _info.strideInBytes; }
	AX_INLINE	Int				dataSize		() const { return _pixelData.sizeInBytes(); }

	AX_INLINE	const Info&		info			() const { return _info; }

	template<class COLOR> AX_INLINE	MutSpan<COLOR>	row(Int y)			{ _typeCheck(COLOR::kColorType); return row_noTypeCheck<COLOR>(y); }
	template<class COLOR> AX_INLINE	   Span<COLOR>	row(Int y) const	{ _typeCheck(COLOR::kColorType); return row_noTypeCheck<COLOR>(y); }

	template<class COLOR> AX_INLINE MutSpan<COLOR>	row_noTypeCheck(Int y)			{ return MutSpan<COLOR>(reinterpret_cast<      COLOR*>(rowBytes(y).data()), width()); }
	template<class COLOR> AX_INLINE    Span<COLOR>	row_noTypeCheck(Int y) const	{ return    Span<COLOR>(reinterpret_cast<const COLOR*>(rowBytes(y).data()), width()); }

	template<class COLOR> AX_INLINE		  COLOR&	pixel(Int x, Int y)			{ return row<COLOR>(y)[x]; }
	template<class COLOR> AX_INLINE	const COLOR&	pixel(Int x, Int y) const	{ return row<COLOR>(y)[x]; }

	void subImage(Vec2i pos, const ImageInfo& srcInfo, ByteSpan srcPixelData);
	

	template<class COLOR> void fill(const COLOR& color);

	AX_INLINE	MutByteSpan	rowBytes(Int y)			{ return MutByteSpan(&_pixelData[y * _info.strideInBytes], width() * _info.pixelSizeInBytes()); }
	AX_INLINE	   ByteSpan	rowBytes(Int y) const	{ return    ByteSpan(&_pixelData[y * _info.strideInBytes], width() * _info.pixelSizeInBytes()); }

	MutByteSpan	pixelData()			{ return _pixelData; }
	   ByteSpan	pixelData() const	{ return _pixelData; }

	void copy(const Image& src);
	
	template<class COLOR> ImagePainter_<COLOR> painter();

protected:
	void _typeCheck(ColorType colorType) const { if (colorType != _info.colorType) throw Error_Undefined(); }

	Info			_info;
	Array<Byte>		_pixelData;
};

//! Image for specific color type
template<class COLOR>
class Image_ : public Image {
	using Base = Image;
public:
	using Color = COLOR;

	void create(Vec2i size, Int strideInBytes)	{ Base::create(Color::kColorType, size, strideInBytes); }
	void create(Vec2i size)						{ Base::create(Color::kColorType, size); }

	MutSpan<COLOR>	row(Int y)				{ return Base::row<COLOR>(y); }
	Span<COLOR>	row(Int y) const			{ return Base::row<COLOR>(y); }

	const COLOR&		pixel(Int x, Int y) const	{ return Base::pixel<COLOR>(y)[x]; }
	COLOR&		pixel(Int x, Int y)			{ return Base::pixel<COLOR>(y)[x]; }

	void			fill(const Color& color)	{ Base::fill<COLOR>(color);	}

	ImagePainter_<COLOR> painter() { return Base::painter<COLOR>(); }
};

template<class COLOR> inline
void Image::fill(const COLOR& color) {
	_typeCheck(COLOR::kColorType);
	for (Int y=0; y<_info.size.y; y++) {
		row_noTypeCheck<COLOR>(y).fillValues(color);
	}
}

using ImageRb		= Image_<ColorRb>;
using ImageRf		= Image_<ColorRf>;
using ImageRGBAb	= Image_<ColorRGBAb>;
using ImageRGBAf	= Image_<ColorRGBAf>;

} // namespace