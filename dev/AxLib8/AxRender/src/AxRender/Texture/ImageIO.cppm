module;
export module AxRender:ImageIO;
export import :Image;

export namespace ax /*::AxRender*/ {

class ImageIO_ReadHandler : public NonCopyable {
	using This = ImageIO_ReadHandler;
public:
	using ReadPixelsToFunc = Delegate_<void (MutByteSpan span)>;

	Image_CreateDesc	desc;
	ReadPixelsToFunc	readPixelsFunc;
	
	void readPixelsTo(MutByteSpan outSpan) const {
		if (outSpan.size() < desc.dataSize) {
			throw Error_Undefined();
		}
		readPixelsFunc.invoke(outSpan);
	}
};

struct ImageIO : public NonCopyable {
public:
	ImageIO() = delete;

	using Callback = Delegate_<void (ImageIO_ReadHandler& handler)>;

	static void loadFile(const Callback& callback, StrView filename, ImageFileType fileType = ImageFileType::None);
	static void loadMem (const Callback& callback, ByteSpan inData,  ImageFileType fileType);
};

} // namespace
