module;

export module AxRender.ColorUtil;
export import AxRender.ColorRGBA;
export import AxRender.ColorLA;
export import AxRender.ColorPacked;
export import AxRender.ColorDXT;

export namespace ax {

struct ColorUtil {
	ColorUtil() = delete;
	static void convertSpanByType(ColorType dstType, MutByteSpan dstSpan, ColorType srcType, ByteSpan srcSpan);

private:

	template<class DST, class SRC>
	static void _convertSpan(MutByteSpan dstSpan, ByteSpan srcSpan);
};

template<class DST, class SRC>
struct ColorConverter;

template<class COLOR>
struct ColorConverter<COLOR, COLOR> {
	static void conv(COLOR& dst, const COLOR& src) { dst = src; }
};

template<class T, VecSIMD SIMD>
struct ColorConverter<ColorRGBA_<T,SIMD>, ColorRGB_<T,SIMD>> {
	static void conv(ColorRGBA_<T,SIMD>& dst, const ColorRGB_<T,SIMD>& src) {
		dst.set(src);
	}
};

template<class T, VecSIMD SIMD>
struct ColorConverter<ColorRGB_<T,SIMD>, ColorRGBA_<T,SIMD>> {
	static void conv(ColorRGB_<T,SIMD>& dst, const ColorRGBA_<T,SIMD>& src) {
		dst.set(src.r, src.g, src.b);
	}
};


} // namespace ax