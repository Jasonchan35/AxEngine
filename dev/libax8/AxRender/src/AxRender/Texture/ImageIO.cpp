module AxRender;
import :ImageIO_Reader_DDS;
import :ImageIO_Reader_PNG;
//import :ImageIO_Reader_JPEG;

namespace ax::AxRender {

void ImageIO::loadFile(Callback callback, StrView filename, ImageFileType fileType) {
	FileMemMap	inData(filename);

	if (fileType == ImageFileType::None) {
		auto ext = FilePath::extension(filename);

		fileType = ImageFileType_fromFileExt(ext);
		if (fileType == ImageFileType::None) {
			throw Error_Undefined(Fmt("Unsupported image file extension '{}'", ext));
		}
	}

	loadMem(callback, inData, fileType);
}

void ImageIO::loadMem(Callback callback, ByteSpan inData, ImageFileType fileType) {
	switch (fileType) {
//		case ImageFileType::JPEG:	{ ImageIO_Reader_JPEG rd; rd.load(callback, inData); } break;
		case ImageFileType::PNG:	{ ImageIO_Reader_PNG  rd; rd.load(callback, inData); } break;
		case ImageFileType::DDS:	{ ImageIO_Reader_DDS  rd; rd.load(callback, inData); } break;

		default: throw Error_Undefined(Fmt("Unsupported image file format '{}'", fileType));
	}
}

} // namespace