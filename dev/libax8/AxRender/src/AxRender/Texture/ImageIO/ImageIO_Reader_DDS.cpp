module AxRender;
import :ImageIO_DDS_Header;
import :ImageIO_DDS;

namespace ax::AxRender {

void ImageIO_Reader_DDS::_readHeader(BinIO_Reader& de, DDS_HEADER& hdr) {
	de.io_fixed_le(hdr.dwSize);
	de.io_fixed_le(hdr.dwHeaderFlags);
	de.io_fixed_le(hdr.dwHeight);
	de.io_fixed_le(hdr.dwWidth);
	de.io_fixed_le(hdr.dwPitchOrLinearSize);
	de.io_fixed_le(hdr.dwDepth);
	de.io_fixed_le(hdr.dwMipMapCount);
	de.advance(sizeof(hdr.dwReserved1[0]) * 11);

	de.io_fixed_le(hdr.ddspf.dwSize);
	de.io_fixed_le(hdr.ddspf.dwFlags);
	de.io_fixed_le(hdr.ddspf.dwFourCC);
	de.io_fixed_le(hdr.ddspf.dwRGBBitCount);
	de.io_fixed_le(hdr.ddspf.dwRBitMask);
	de.io_fixed_le(hdr.ddspf.dwGBitMask);
	de.io_fixed_le(hdr.ddspf.dwBBitMask);
	de.io_fixed_le(hdr.ddspf.dwABitMask);

	de.io_fixed_le(hdr.dwSurfaceFlags);
	de.io_fixed_le(hdr.dwCubemapFlags);
	de.advance(sizeof(hdr.dwReserved2[0]) * 3);

	if (hdr.dwSize != sizeof(hdr))
		throw Error_Undefined();

	if (hdr.ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
		throw Error_Undefined();

	if (!(hdr.ddspf.dwFlags & DDS_FOURCC))
		throw Error_Undefined();
}

void ImageIO_Reader_DDS::_readHeader10(BinIO_Reader& de, DDS_HEADER_DXT10& hdr10) {
	uint32_t tmp32;

	de.io_fixed_le(tmp32);
	hdr10.dxgiFormat = static_cast<DXGI_FORMAT>(tmp32);

	de.io_fixed_le(tmp32);
	hdr10.resourceDimension = static_cast<D3D10_RESOURCE_DIMENSION>(tmp32);

	de.io_fixed_le(hdr10.miscFlag);
	de.io_fixed_le(hdr10.arraySize);
	de.io_fixed_le(hdr10.miscFlags2);
}

void ImageIO_Reader_DDS::load(ImageIO::Callback callback, ByteSpan inData) {
	BinIO_Reader de(inData);

	uint32_t sign;
	de.io_fixed_le(sign);

	if (sign != CharUtil::FourCC("DDS "))
		throw Error_Undefined();

	DDS_HEADER hdr;
	_readHeader(de, hdr);

	auto colorType = ColorType::None;

	if (hdr.ddspf.dwFourCC == CharUtil::FourCC("DX10")) {
		DDS_HEADER_DXT10 hdr10;
		_readHeader10(de, hdr10);

		if (hdr10.resourceDimension != D3D10_RESOURCE_DIMENSION_TEXTURE2D)
			throw Error_Undefined();

		switch (hdr10.dxgiFormat)
		{
			case DXGI_FORMAT_BC1_UNORM:  colorType = ColorType::DXT_BC1; break;
			case DXGI_FORMAT_BC2_UNORM:  colorType = ColorType::DXT_BC2; break;
			case DXGI_FORMAT_BC3_UNORM:  colorType = ColorType::DXT_BC3; break;
			case DXGI_FORMAT_BC4_UNORM:  colorType = ColorType::DXT_BC4; break;
			case DXGI_FORMAT_BC5_UNORM:  colorType = ColorType::DXT_BC5; break;
			case DXGI_FORMAT_BC7_UNORM:  colorType = ColorType::DXT_BC7; break;
			default: throw Error_Undefined();
		}
	}else{
		switch (hdr.ddspf.dwFourCC) {
			case CharUtil::FourCC("DXT1"): colorType = ColorType::DXT_BC1; break;
			case CharUtil::FourCC("DXT2"): colorType = ColorType::DXT_BC2; break;
			case CharUtil::FourCC("DXT3"): colorType = ColorType::DXT_BC2; break;
			case CharUtil::FourCC("DXT4"): colorType = ColorType::DXT_BC3; break;
			case CharUtil::FourCC("DXT5"): colorType = ColorType::DXT_BC3; break;

			case CharUtil::FourCC("BC4U"): colorType = ColorType::DXT_BC4; break;
			case CharUtil::FourCC("BC5U"): colorType = ColorType::DXT_BC5; break;
			case CharUtil::FourCC("DXT7"): colorType = ColorType::DXT_BC7; break;
			default: throw Error_Undefined();
		}
	}

	if (hdr.dwWidth % 4 || hdr.dwHeight % 4)
		throw Error_Undefined();

	Int mipLevels = 1;

	if (hdr.dwMipMapCount > 1) {
		uint32_t w = hdr.dwWidth;
		uint32_t h = hdr.dwHeight;
		for (uint32_t i = 0; i < hdr.dwMipMapCount; i++) {
			if (w & 0x3 || h & 0x3)
				break; // bypass non-multiple of 4 level

			w >>= 1;
			h >>= 1;
			mipLevels = i + 1;
		}
	}

	Int blockSize = ColorTypeInfo::s_get(colorType).kCompressedBlockSize;
	Int strideInBytes = Math::max(1U, ((hdr.dwWidth + 3)/4)) * blockSize;

	ImageIO_ReadResult result;

	result.desc.info.colorType		= colorType;
	result.desc.info.size			= Vec3i(hdr.dwWidth, hdr.dwHeight, 0);
	result.desc.info.mipLevels		= mipLevels;
	result.desc.info.strideInBytes	= strideInBytes;
	result.desc.dataSize			= de.remain();

	auto srcSpan = ByteSpan(de.cur(), de.remain());

	result.copyPixelsFunc = [&](MutByteSpan outSpan) {
		outSpan.copyValues(srcSpan);
	};

	callback.invoke(result);
}

} // namespace