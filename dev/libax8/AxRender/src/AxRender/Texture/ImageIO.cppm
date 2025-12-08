module;
export module AxRender:ImageIO;
export import :Image;

export namespace ax::AxRender {

class ImageIO_ReadResult : public NonCopyable {
	using This = ImageIO_ReadResult;
public:
	using CopyPixelsFunc = Delegate_<void (MutByteSpan span)>;

	Image_CreateDesc	desc;
	CopyPixelsFunc		copyPixelsFunc;
	
	void copyPixelsTo(MutByteSpan outSpan) const {
		if (outSpan.size() < desc.dataSize) {
			throw Error_Undefined();
		}
		copyPixelsFunc.invoke(outSpan);
	}
};

struct ImageIO : public NonCopyable {
public:
	ImageIO() = delete;

	using Callback = Delegate_<void (ImageIO_ReadResult& req)>;

	static void loadFile(Callback callback, StrView filename, ImageFileType fileType = ImageFileType::None);
	static void loadMem (Callback callback, ByteSpan inData,  ImageFileType fileType);
};

} // namespace
