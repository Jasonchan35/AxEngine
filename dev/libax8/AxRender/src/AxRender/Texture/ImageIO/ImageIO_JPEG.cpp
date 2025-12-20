module;

#include <jpeglib.h>

module AxRender;
import :ImageIO_JPEG;

namespace ax /*::AxRender*/ {

void ImageIO_Reader_JPEG::s_output_message (j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];
	cinfo->err->format_message(cinfo, buffer);
	AX_LOG("jpeg error {}", buffer);
}

void ImageIO_Reader_JPEG::s_error_exit (j_common_ptr cinfo) {
	s_output_message(cinfo);
	cinfo->err->output_message(cinfo);
	auto* p = static_cast<ImageIO_Reader_JPEG*>(cinfo->client_data);
	longjmp(p->_setjmp_buffer, 1);
}

bool ImageIO_Reader_JPEG::error_exit_longjmp_restore_point() {
	// !!! call this function before any libjpeg C-function that might longjmp()
	// to avoid any C++ destructor or exception try/catch block happen in between
	AX_VC_WARNING_PUSH_AND_DISABLE(4611) // interaction between '_setjmp' and C++ object destruction is non-portable
	// longjmp() to here from error_exit()
	return setjmp(_setjmp_buffer) != 0;
	AX_VC_WARNING_POP()
}

boolean ImageIO_Reader_JPEG::s_fill_input_buffer (j_decompress_ptr cinfo) {
	AX_LOG("jpeg error fill_input_buffer");
	return FALSE;
}

void ImageIO_Reader_JPEG::s_skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	if (num_bytes > 0) {
AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
		cinfo->src->next_input_byte += ax_safe_cast_to<size_t>(num_bytes);
		cinfo->src->bytes_in_buffer -= ax_safe_cast_to<size_t>(num_bytes);
AX_GCC_WARNING_POP()
	}
}

ImageIO_Reader_JPEG::ImageIO_Reader_JPEG() {
	_jpeg  = {}; 
	_errMgr = {};
	_srcMgr = {};
	_jpeg.client_data = this;
}

ImageIO_Reader_JPEG::~ImageIO_Reader_JPEG() {
	jpeg_destroy_decompress(&_jpeg);
}

void ImageIO_Reader_JPEG::load(const ImageIO::Callback& callback, ByteSpan inData) {
	_jpeg.err = jpeg_std_error(&_errMgr);
	_errMgr.output_message = s_output_message;
	_errMgr.error_exit = s_error_exit;

	jpeg_create_decompress(&_jpeg);

	_srcMgr.init_source = s_init_source;
	_srcMgr.fill_input_buffer = s_fill_input_buffer;
	_srcMgr.skip_input_data = s_skip_input_data;
	_srcMgr.resync_to_restart = jpeg_resync_to_restart; // use default method
	_srcMgr.term_source = s_term_source;

	_srcMgr.bytes_in_buffer = ax_safe_cast_from(inData.size());
	_srcMgr.next_input_byte = reinterpret_cast<const JOCTET*>(inData.data());

	_jpeg.src = &_srcMgr;

	//-------
	if (error_exit_longjmp_restore_point()) {
		throw Error_Undefined("error jpeg_read_header");
	}
	if (int code = jpeg_read_header(&_jpeg, TRUE); code != JPEG_HEADER_OK) {
		throw Error_Undefined(Fmt("error jpeg_read_header return={}", code));
	}

	//-------
	if (error_exit_longjmp_restore_point()) {
		throw Error_Undefined("error jpeg_start_decompress");
	}
	if (jpeg_start_decompress(&_jpeg) != TRUE) {
		throw Error_Undefined("error jpeg_start_decompress return");
	}
	
	_jpeg.out_color_space = JCS_RGB;

	Int width  = ax_safe_cast_from(_jpeg.output_width);
	Int height = ax_safe_cast_from(_jpeg.output_height);

	if (width <= 0 || height <= 0) {
		throw Error_Undefined("jpeg image size is 0");
	}

	int srcPixelSize = _jpeg.output_components;

	auto out_color_type = ColorType::None;
	auto src_color_type = ColorType::None;

	switch (srcPixelSize) {
		case 1: src_color_type = ColorType::Lb;   out_color_type = ColorType::Lb;    break;
		case 3: src_color_type = ColorType::RGBb; out_color_type = ColorType::RGBAb; break;
	}

	if (src_color_type == ColorType::None || out_color_type == ColorType::None) {
		throw Error_Undefined();
	}

	auto& outInfo = ColorTypeInfo::s_get(out_color_type);

	auto srcStrideInBytes = width * srcPixelSize;
	auto dstStrideInBytes = width * outInfo.sizeInBytes;

	ImageIO_ReadHandler handler;

	handler.desc.info.colorType		= out_color_type;
	handler.desc.info.size			= Vec3i(width, height, 0);
	handler.desc.info.mipLevels		= 1;
	handler.desc.info.strideInBytes = dstStrideInBytes;
	handler.desc.dataSize			= handler.desc.info.computeDataSize();

	handler.readPixelsFunc = [&](MutByteSpan outSpan) {
		u8* outData = outSpan.data();

		// no conversion is needed, decode directly to image buffer
		if (src_color_type == out_color_type) {
			for (Int y = 0; y < height; y++) {

				AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
				JSAMPLE* scanline[1] = {
					reinterpret_cast<JSAMPLE*>(outData + y * dstStrideInBytes)
				};
				AX_GCC_WARNING_POP()

				//-------
				if (error_exit_longjmp_restore_point()) {
					throw Error_Undefined();
				}
				if (jpeg_read_scanlines(&_jpeg, scanline, 1) < 1) {
					throw Error_Undefined();
				}
			}
		} else {
			Array<u8, 4 * 4 * 1024> tmpRow;
			tmpRow.resize(srcStrideInBytes);
			JSAMPLE* scanline[1] = { reinterpret_cast<JSAMPLE*>(tmpRow.data()) };

			for (Int y = 0; y < height; y++) {
				if (error_exit_longjmp_restore_point()) {
					throw Error_Undefined();
				}
				if (jpeg_read_scanlines(&_jpeg, scanline, 1) < 1) {
					throw Error_Undefined();
				}

				AX_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
				auto outRow = MutByteSpan(outData + y * dstStrideInBytes, dstStrideInBytes);
				AX_GCC_WARNING_POP()

				ColorUtil::convertSpanByType(out_color_type, outRow, src_color_type, tmpRow);
			}
		}
	};

	callback.invoke(handler);

	if (jpeg_finish_decompress(&_jpeg) != TRUE) {
		throw Error_Undefined("jpeg_finish_decompress");
	}
}


} //namespace