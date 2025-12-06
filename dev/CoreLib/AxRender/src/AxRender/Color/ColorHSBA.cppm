module;

export module AxRender.ColorHSBA;
export import AxRender.ColorRGBA;

export namespace ax {

template<class T>
class Color_<ColorModel::HSBA, T> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = T;
	T h,s,b,a;

	static constexpr ColorModel	kColorModel		= ColorModel::HSBA;
	static constexpr ColorElem	kColorElem		= ColorElem_get<T>;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	= 4;
	static constexpr Int		kAlphaBits		= AX_SIZEOF(a);

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	// AX_TYPE_INFO(Color_, NoBaseClass) {
	// 	static StrView s_name() {
	// 		static auto s = Fmt("ColorHSBA_<{}>", ax_typeof<T>::s_name());
	// 		return s;
	// 	}
	//
	// 	AX_FIELD_INFO(h) {};
	// 	AX_FIELD_INFO(s) {};
	// 	AX_FIELD_INFO(b) {};
	// 	AX_FIELD_INFO(a) {};
	// };

//---

	AX_INLINE explicit Color_() = default;
	AX_INLINE          Color_(T h_, T s_, T v_, T a_ = kElemOne) { set(h_,s_,v_,a_); }

	AX_INLINE		T* data()		{ return &h; }
	AX_INLINE const T* data() const	{ return &h; }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr    FixedSpan<Element, kElementCount> fixedSpan() const { return    FixedSpan<Element, kElementCount>(data()); }
	AX_INLINE constexpr MutFixedSpan<Element, kElementCount> fixedSpan()       { return MutFixedSpan<Element, kElementCount>(data()); }
	AX_INLINE constexpr    Span<Element> span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MutSpan<Element> span()			{ return fixedSpan(); }

	AX_INLINE constexpr void set(T h_, T s_, T b_, T a_ = kElemOne) { h=h_; s=s_; b=b_; a=a_; }

	AX_INLINE bool	operator==	(const This& rhs) const { return h == rhs.h && s == rhs.s && b == rhs.b && a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return h != rhs.h || s != rhs.s || b != rhs.b || a != rhs.a; }
};

} // namespace ax