module;
#include "libpng16/png.h"
module AxRender;
import :ImageIO_Reader_PNG;

namespace ax::AxRender {

ImageIO_Reader_PNG::~ImageIO_Reader_PNG() {
	if (_info) {
		png_destroy_info_struct(_png, &_info);
		_info = nullptr;
	}
	if (_png) {
		png_destroy_read_struct(&_png, nullptr, nullptr);
		_png = nullptr;
	}
}

bool ImageIO_Reader_PNG::_error_longjmp_restore_point() {
	// !!! call this function before any libpng C-function that might longjmp()
	// to avoid any C++ destructor or exception try/catch block happen in between

	#if AX_COMPILER_VC
		#pragma warning(push) 
		#pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
	#endif

	// longjmp() to here from pngReportError()
	return setjmp(png_jmpbuf(_png)) != 0;

	#if AX_COMPILER_VC
		#pragma warning(pop) 
	#endif
}

void ImageIO_Reader_PNG::load(ImageIO::Callback callback, ByteSpan inData) {
	_inData = inData;
	_readPtr = inData.data();
	AX_ASSERT(!_png);

	if (inData.size() < 8 || png_sig_cmp(inData.data(), 0, 8)) {
		throw Error_Undefined();
	}

	_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!_png) {
		throw Error_Undefined();
	}

	_info = png_create_info_struct(_png);
	if (!_info) {
		throw Error_Undefined();
	}

	png_set_read_fn( _png, this, &_s_onRead );

	//-----
	if (_error_longjmp_restore_point()) {
		throw Error_Undefined();
	}
	png_read_info( _png, _info );

	png_uint_32 in_width  = 0;
	png_uint_32 in_height = 0;
	int in_bit = 0;
	int in_color_type = 0;
	int in_interlace = 0;
	bool in_palette_has_alpha = false;

	//-----
	if (_error_longjmp_restore_point()) {
		throw Error_Undefined();
	}
	png_get_IHDR( _png, _info, &in_width, &in_height, &in_bit, &in_color_type, &in_interlace, nullptr, nullptr );	

	auto out_color_type = ColorType::None;
	if (in_bit == 8) {
		switch (in_color_type) {
			case PNG_COLOR_TYPE_RGB:		out_color_type = ColorType::RGBAb;	break; //DirectX doesn't support RGB, so convert to RGBA instead
			case PNG_COLOR_TYPE_RGB_ALPHA:	out_color_type = ColorType::RGBAb;	break;
			case PNG_COLOR_TYPE_GRAY:		out_color_type = ColorType::Lb;		break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:	out_color_type = ColorType::LAb;	break;
			case PNG_COLOR_TYPE_PALETTE:	{
				//get palette transparency table
				png_bytep		trans_alpha = nullptr;
				int				num_trans   = 0;
				png_color_16p	trans_color = nullptr;

				//-----
				if (_error_longjmp_restore_point()) {
					throw Error_Undefined();
				}
				png_get_tRNS(_png, _info, &trans_alpha, &num_trans, &trans_color);

				out_color_type = ColorType::RGBAb;
					
				if (trans_alpha) {
					in_palette_has_alpha = true;
//						out_color_type = ColorType::RGBAb;
				}else{
//						out_color_type = ColorType::RGBb;
				}
					
			}break;
		}
	} else if (in_bit == 16) {
		switch (in_color_type) {
			case PNG_COLOR_TYPE_RGB:		out_color_type = ColorType::RGBAs;	break; //DirectX doesn't support RGB, so convert to RGBA instead
			case PNG_COLOR_TYPE_RGB_ALPHA:	out_color_type = ColorType::RGBAs;	break;
			case PNG_COLOR_TYPE_GRAY:		out_color_type = ColorType::Ls;		break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:	out_color_type = ColorType::LAs;	break;
		}
	}

	if (out_color_type == ColorType::None) {
		throw Error_Undefined();
	}

	//-----
	_setReadFilter(out_color_type, in_color_type, in_bit, in_palette_has_alpha);

	if (in_width <= 0 || in_height <= 0) {
		throw Error_Undefined();
	}

	Int width  = SafeCast(in_width);
	Int height = SafeCast(in_height);

	Int pixelSize = ColorTypeInfo::s_get(out_color_type).kSizeInBytes;
	Int strideInBytes = in_width * pixelSize;

	ImageIO_ReadResult result;

	result.desc.info.colorType		= out_color_type;
	result.desc.info.size			= Vec3i(width, height, 0);
	result.desc.info.mipLevels		= 1;
	result.desc.info.strideInBytes  = strideInBytes;
	result.desc.dataSize			= result.desc.info.computeDataSize();

	result.copyPixelsFunc = [&](MutByteSpan outSpan) {
		u8* outData = outSpan.data();

		Array<png_bytep, 4096> rows;
		rows.resize(height);

		for (Int y = 0; y < height; y++) {
			AX_PRAGMA_GCC(diagnostic push)
			AX_PRAGMA_GCC(diagnostic ignored "-Wunsafe-buffer-usage")
			rows[y] = reinterpret_cast<png_bytep>(outData + y * strideInBytes);
			AX_PRAGMA_GCC(diagnostic pop)
		}

		png_set_rows(_png, _info, rows.data());

		if (_error_longjmp_restore_point()) {
			throw Error_Undefined();
		}
		png_read_image(_png, rows.data());
	};

	callback.invoke(result);
}

void ImageIO_Reader_PNG::_s_onRead( png_structp png, png_bytep dest, png_size_t len ) {
	auto* p = reinterpret_cast<This*>(png_get_io_ptr( png ));
	if( !p ) {
		throw Error_Undefined();
	}
	p->_onRead(dest, len);
}

void ImageIO_Reader_PNG::_onRead(png_bytep dest, png_size_t len) {
AX_PRAGMA_GCC(diagnostic push)
AX_PRAGMA_GCC(diagnostic ignored "-Wunsafe-buffer-usage")
	Int n = SafeCast(len);
	if (_readPtr + n > _inData.end()) {
		throw Error_Undefined();
	}
	MemUtil::rawCopy(dest, _readPtr, n);
	_readPtr += n;
AX_PRAGMA_GCC(diagnostic pop)
}

void ImageIO_Reader_PNG::_setReadFilter(ColorType out_color_type, Int in_color_type, Int in_bit, bool in_palette_has_alpha) {
	auto& outColorTypeInfo = ColorTypeInfo::s_get(out_color_type);
	auto elementType = outColorTypeInfo.kColorElem;

	// 8 <-> 16 bit
	switch (elementType) {
		case ColorElem::UNorm8: {
			switch (in_bit) {
				case 8:		break;
				case 16:	png_set_scale_16(_png); break;
				default:	throw Error_Undefined();
			}
		}break;

		case ColorElem::UNorm16: {
			switch (in_bit) {
				case 8:		png_set_expand_16(_png); break;
				case 16:	break;
				default:	throw Error_Undefined();
			}
			#if AX_CPU_ENDIAN_LITTLE
				png_set_swap(_png);
			#endif
		}break;

		default: throw Error_Undefined();
	}

// palette
	if (in_color_type & PNG_COLOR_MASK_PALETTE) {
		png_set_palette_to_rgb(_png);
	}

// alpha channel
	if (outColorTypeInfo.kAlphaBits > 0) {
		// add alpha channel if missing
		if (in_color_type & PNG_COLOR_MASK_PALETTE) {
			if (!in_palette_has_alpha) {
				png_set_filler(_png, 0xffff, PNG_FILLER_AFTER);
			}
		}
		if (!(in_color_type & PNG_COLOR_MASK_ALPHA)) {
			png_set_filler(_png, 0xffff, PNG_FILLER_AFTER);
		}
	} else {
		png_set_strip_alpha(_png);
	}
}

} //namespace
