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


template<class T>
struct ColorConverter<ColorRGBA_<T>, ColorRGB_<T>> {
	static void conv(ColorRGBA_<T>& dst, const ColorRGB_<T>& src) {
		dst.set(src);
	}
};

template<class T>
struct ColorConverter<ColorRGB_<T>, ColorRGBA_<T>> {
	static void conv(ColorRGB_<T>& dst, const ColorRGBA_<T>& src) {
		dst.set(src.r, src.g, src.b);
	}
};


} // namespace ax