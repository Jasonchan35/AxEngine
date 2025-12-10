module;
#include "libpng16/png.h"

export module AxRender:ImageIO_Reader_PNG;
export import :ImageIO;

export namespace ax::AxRender {

class ImageIO_Reader_PNG : public NonCopyable {
	using This = ImageIO_Reader_PNG;
public:
	~ImageIO_Reader_PNG();

	void load(ImageIO::Callback callback, ByteSpan inData);

private:
	static	void _s_onRead(png_structp png, png_bytep dest, png_size_t len) noexcept;
			void _onRead(png_bytep dest, png_size_t len);
			void _setReadFilter(ColorType out_type, Int in_type, Int in_bit, bool in_palette_has_alpha);
			bool _error_longjmp_restore_point();

	png_structp		_png  = nullptr;
	png_infop		_info = nullptr;

	ByteSpan		_inData;
	const Byte*		_readPtr = nullptr;
};

} //namespace
