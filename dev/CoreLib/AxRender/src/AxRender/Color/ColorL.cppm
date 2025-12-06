module;

export module AxRender.ColorL;
export import AxRender.ColorType;

export namespace ax {

template<class T>
class Color_<ColorModel::L, T> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = T;
	T luma;

	using ColorR	= ColorR_<T>;
	using ColorRG	= ColorRG_<T>;
	using ColorRGB	= ColorRGB_<T>;
	using ColorRGBA = ColorRGBA_<T>;
	using ColorL	= ColorL_<T>;
	using ColorLA	= ColorLA_<T>;

	static constexpr ColorModel	kColorModel		= ColorModel::L;
	static constexpr ColorElem	kColorElem		= ColorElem_get<T>;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	= 1;
	static constexpr Int		kAlphaBits		= 0;

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	// struct MetaTypeInit : public AX_META_TYPE_INIT(This) {
	// 	static NameId s_name() {
	// 		static NameId s = AX_NAMEID(Fmt("ColorL_<{}>", MetaTypeOf<T>::s_name()));
	// 		return s;
	// 	}
	// 	struct luma : public AX_META_FIELD_INIT(luma) {};
	// 	using OwnFields = Tuple<luma>;
	// };

	AX_INLINE explicit Color_() = default;
	AX_INLINE          Color_(T luma_) { set(luma_); }

	AX_INLINE constexpr       T* data()			{ return &luma; }
	AX_INLINE constexpr const T* data() const	{ return &luma; }

	using CSpan =    Span<Element>;
	using MSpan = MutSpan<Element>;

	using CFixedSpan =    FixedSpan<Element, kElementCount>;
	using MFixedSpan = MutFixedSpan<Element, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr void set(T luma_) { luma=luma_; }

	AX_INLINE constexpr bool	operator==	(const This& rhs) const { return luma == rhs.luma; }
	AX_INLINE constexpr bool	operator!=	(const This& rhs) const { return luma != rhs.luma; }

	static const This& kZero		() { static This s(kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero()); return s; }
	static const This& kWhite		() { static This s(kElemOne ()); return s; }
	static const This& kGray		() { static This s(kElemHalf()); return s; }
};

} // namespace ax