module;

export module AxRender.ColorLA;
export import AxRender.ColorL;

export namespace ax {

template<class T>
class Color_<ColorModel::LA, T> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = T;
	T luma, a;

	using ColorR	= ColorR_<T>;
	using ColorRG	= ColorRG_<T>;
	using ColorRGB	= ColorRGB_<T>;
	using ColorRGBA = ColorRGBA_<T>;
	using ColorL	= ColorL_<T>;
	using ColorLA	= ColorLA_<T>;

	static constexpr ColorModel	kColorModel		= ColorModel::LA;
	static constexpr ColorElem	kColorElem		= ColorElem_get<T>;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	= 2;
	static constexpr Int		kAlphaBits		= AX_SIZEOF(a);

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	// AX_TYPE_INFO(Color_, NoBaseClass) {
	// 	static StrView s_name() {
	// 		static auto s = Fmt("ColorLA_<{}>", ax_typeof<T>::s_name());
	// 		return s;
	// 	}
	//
	// 	AX_FIELD_INFO(luma) {};
	// 	AX_FIELD_INFO(a) {};
	// };

	AX_INLINE Color_() = default;
	AX_INLINE explicit constexpr Color_(T luma_, T a_ = kElemOne) { set(luma_,a_); }

	AX_INLINE constexpr 	  T* data()			{ return &luma; }
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

	AX_INLINE constexpr void set(T luma_, T a_ = kElemOne) { luma=luma_; a=a_; }

AX_PRAGMA_GCC(diagnostic push)
AX_PRAGMA_GCC(diagnostic ignored "-Wfloat-equal")
	AX_INLINE constexpr bool	operator==	(const This& rhs) const { return luma == rhs.luma && a == rhs.a; }
	AX_INLINE constexpr bool	operator!=	(const This& rhs) const { return luma != rhs.luma || a != rhs.a; }
AX_PRAGMA_GCC(diagnostic pop)

	static const This& kZero		() { static This s(kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero()); return s; }
	static const This& kWhite		() { static This s(kElemOne ()); return s; }
	static const This& kGray		() { static This s(kElemHalf()); return s; }
};

} // namespace ax