module;
export module AxRender:Image;
export import :DataType;

export namespace ax::AxRender {

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
	Vec3i		size = AxTag::Zero;
	Int			strideInBytes = 0;
	Int			mipLevels = 1;

	ImageInfo() =default;

	ImageInfo(ColorType colorType_, const Vec2i& size_, Int mipLevels_ = 1) {
		colorType = colorType_;
		size	  = Vec3i(size_, 0);
		mipLevels = mipLevels_;
		computeDataSize();
	}

	Int			pixelSizeInBytes() const { return ColorTypeInfo::s_get(colorType).kSizeInBytes; }
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

			void	create(ColorType colorType, Vec2i size);
			void	create(const CreateDesc& desc);
			void	clear();

			void	loadFile(StrView filename, ImageFileType fileType = ImageFileType::None);
			void	loadMem(ByteSpan inData,   ImageFileType fileType);

	AX_INLINE	ColorType		colorType		() const { return _info.colorType; }
	AX_INLINE	Vec2i			size			() const { return _info.size.xy(); }
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


	template<class COLOR> void fill(const COLOR& color);

	AX_INLINE	MutByteSpan	rowBytes(Int y)			{ return MutByteSpan(&_pixelData[y * _info.strideInBytes], width() * _info.pixelSizeInBytes()); }
	AX_INLINE	   ByteSpan	rowBytes(Int y) const	{ return    ByteSpan(&_pixelData[y * _info.strideInBytes], width() * _info.pixelSizeInBytes()); }

	MutByteSpan	pixelData()			{ return _pixelData; }
	   ByteSpan	pixelData() const	{ return _pixelData; }

	void copy(const Image& src);

protected:
	void _typeCheck(ColorType colorType) const { if (colorType != _info.colorType) throw Error_Undefined(); }

	Info			_info;
	Array<Byte>		_pixelData;
};

template<class COLOR> inline
void Image::fill(const COLOR& color) {
	_typeCheck(COLOR::kColorType);
	Int ny = _info.size.y;
	for (Int y = 0; y < ny; y++) {
		for (auto& p : row_noTypeCheck<COLOR>(y)) {
			p = color;
		}
	}
}

} // namespace