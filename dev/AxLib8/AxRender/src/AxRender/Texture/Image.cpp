module AxRender;
import :ImageIO;

namespace ax /*::AxRender*/ {

ImageFileType ImageFileType_fromFileExt(StrView ext) {
	if (ext.equals("dds" , StrCase::Ignore)) return ImageFileType::DDS;
	if (ext.equals("png" , StrCase::Ignore)) return ImageFileType::PNG;
	if (ext.equals("jpeg", StrCase::Ignore)) return ImageFileType::JPEG;
	if (ext.equals("jpg" , StrCase::Ignore)) return ImageFileType::JPEG;

	return ImageFileType::None;
}

void Image::clear() {
	_info = {};
	_pixelData.clear();
}

void Image::create(ColorType colorType, Vec2i size) {
	CreateDesc desc;
	desc.info.colorType = colorType;
	desc.info.size = size;
	create(desc);
}

void Image::create(const CreateDesc& desc) {
	clear();
	_info = desc.info;

	Int dataSize = desc.dataSize <= 0 ? _info.computeDataSize() : desc.dataSize;
	_pixelData.resize(dataSize);
}

void Image::subImage(Vec2i pos, const ImageInfo& srcInfo, ByteSpan srcPixelData) {
	if (srcInfo.colorType != _info.colorType) {
		throw Error_Undefined();
	}

	auto pixelSizeInBytes = _info.pixelSizeInBytes();

	if (srcInfo.pixelSizeInBytes() != pixelSizeInBytes) {
		throw Error_Undefined();
	}

	auto start = pos;
	auto end   = pos + srcInfo.size;

	auto zero  = Vec2i(0,0);

	Math::clamp_itself(start, zero, size());
	Math::clamp_itself(end,   zero, size());
	if (end.x <= start.x || end.y <= start.y) return;

	Byte* dst = &rowBytes(start.y)[start.x];
	const Byte* src    = srcPixelData.data();
	const Byte* srcEnd = srcPixelData.end();

	Int dstStride = strideInBytes();
	Int srcStride = srcInfo.strideInBytes;

	if (pos.x == 0 && _info.size.x == srcInfo.size.x && _info.strideInBytes == srcInfo.strideInBytes) {
		//direct copy all rows
		auto n = pixelSizeInBytes * _info.size.x * srcInfo.size.y;
		if (src + n > srcEnd) {
			throw Error_Undefined();
		}
		
		MemUtil::rawCopy(dst, src, n);
		return;
	}

	auto rows = end.y - start.y;
	Int rowSizeInBytes = pixelSizeInBytes * (end.x - start.x);

	for (auto y = 0; y < rows; y++) {
		if (src + rowSizeInBytes > srcEnd) {
			throw Error_Undefined();
		}
		MemUtil::rawCopy(dst, src, rowSizeInBytes);
		dst += dstStride;
		src += srcStride;
	}	
}

void Image::copy(const Image& src) {
	clear();
	_info = src._info;
	_pixelData = src._pixelData;
}

void Image::loadFile(StrView filename, ImageFileType fileType) {
	ImageIO::loadFile([this](ImageIO_ReadHandler& handler){
		this->create(handler.desc);
		handler.readPixelsTo(this->_pixelData);
	}, filename, fileType);
}

void Image::loadMem(ByteSpan inData, ImageFileType fileType) {
	ImageIO::loadMem([this](ImageIO_ReadHandler& handler){
		this->create(handler.desc);
		handler.readPixelsTo(this->_pixelData);
	}, inData, fileType);
}

Int ImageInfo::computeDataSize() {
	if (mipLevels <= 1)
		mipLevels = 1;

	auto expectedStride = size.x * pixelSizeInBytes();

	if (strideInBytes > 0 && strideInBytes < expectedStride) {
		throw Error_Undefined(Fmt("ImageInfo strideInByte({}) < expected({})", strideInBytes, expectedStride));
	} else {
		strideInBytes = expectedStride;
	}

	auto curSize = strideInBytes * size.y;

	Int dataSize = 0;
	for (Int i = 0; i < mipLevels; i++) {
		dataSize += curSize;
		curSize /= 4;
	}

	return dataSize;
}

} // namespace