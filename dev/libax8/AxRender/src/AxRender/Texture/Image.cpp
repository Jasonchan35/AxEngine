module AxRender;
import :ImageIO;

namespace ax::AxRender {

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
	desc.info.size.set(size, 0);
	create(desc);
}

void Image::create(const CreateDesc& desc) {
	clear();
	_info = desc.info;

	Int dataSize = desc.dataSize <= 0 ? _info.computeDataSize() : desc.dataSize;
	_pixelData.resize(dataSize);
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