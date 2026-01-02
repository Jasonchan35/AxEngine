module;

export module AxRender:ImagePainter;
export import :Image;

export namespace ax {

template<class COLOR>
class ImagePainter_  : public NonCopyable {
public:
	using Vec2    = Vec2f;
	using Rect2   = Rect2f;
	using Margin2 = Margin2f;
	using Color   = COLOR;

	ImagePainter_(Image& img) : _img(img) { _checkType(); }

	void fill(const Color& color) { _img.fill(color); }

	void drawAACircle				(const Vec2& center, float radius, const Color& color);
	void drawAACircleOutline		(const Vec2& center, float radius, const Color& color, float outlineWidth);

	void drawAARoundedRect			(const Rect2& rect, float radius, const Color& color);
	void drawAARoundedRectOutline	(const Rect2& rect, float radius, const Color& color, float outlineWidth);

private:
	void _checkType() {
		if (_img.colorType() != Color::kColorType)
			throw Error_Undefined();
	}

	template<bool OUTLINE> void _drawAACircleImpl(const Vec2& center, float radius, const Color& color, float outlineWidth);
	template<bool OUTLINE> void _drawAARoundedRectImpl(const Rect2& rect, float radius, const Color& color, float outlineWidth);

	AX_INLINE MutSpan<Color> _row(Int y) { return _img.template row_noCheck<Color>(y); }

	Image& _img;
};

template<class COLOR> inline
ImagePainter_<COLOR> Image::painter() {
	return ImagePainter_<COLOR>(*this);
}

} // namespace