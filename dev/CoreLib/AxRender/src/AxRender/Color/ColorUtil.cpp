module;

module AxRender.ColorUtil;

namespace ax {

template<class DST, class SRC> inline
void ColorUtil::_convertSpan(MutByteSpan dstSpan, ByteSpan srcSpan) {
	auto dstPixelCount = dstSpan.size() / AX_SIZEOF(DST);
	auto srcPixelCount = srcSpan.size() / AX_SIZEOF(SRC);
	if (dstPixelCount < srcPixelCount) throw Error_Undefined();

	auto* dstData = reinterpret_cast<      DST*>(dstSpan.data());
	auto* srcData = reinterpret_cast<const SRC*>(srcSpan.data());

	using Conv = ColorConverter<DST, SRC>;
	for (Int i = 0; i < srcPixelCount; i++) {
		Conv::conv(*dstData, *srcData);

AX_PRAGMA_GCC(diagnostic push)
AX_PRAGMA_GCC(diagnostic ignored "-Wunsafe-buffer-usage")
		++srcData;
		++dstData;
AX_PRAGMA_GCC(diagnostic pop)
	}
};

void ColorUtil::convertSpanByType(ColorType dstType, MutByteSpan dstSpan, ColorType srcType, ByteSpan srcSpan) {
	if (dstType == srcType) {
		dstSpan.copyValues(srcSpan);
		return;
	}

#define DISPATCH_DST(DST, SRC) \
	case DST::kColorType: { _convertSpan<DST, SRC>(dstSpan, srcSpan); } break; \
//------

#define DISPATCH_SRC(SRC) \
	case SRC::kColorType: { \
		switch (dstType) { \
			DISPATCH_DST(ColorRGBb,  SRC); \
			DISPATCH_DST(ColorRGBAb, SRC); \
			default: throw Error_Undefined(); \
		} \
	} break; \
//------

	// switch (srcType) {
	// 	DISPATCH_SRC(ColorRGBb);
	// 	DISPATCH_SRC(ColorRGBAb);
	// 	default: throw Error_Undefined();
	// }

#undef DISPATCH_DST
#undef DISPATCH_SRC
}

} // namespace ax