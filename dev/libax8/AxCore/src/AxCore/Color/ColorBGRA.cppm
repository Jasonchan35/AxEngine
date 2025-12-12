module;

export module AxCore.Color:ColorBGRA;
export import :ColorRGBA;

export namespace ax {

template<class T>
class Color_<ColorModel::BGRA, T> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 4;
public:
	using Element = T;
	T b,g,r,a;

	static constexpr ColorModel	kColorModel		= ColorModel::BGRA;
	static constexpr ColorType	kColorType		= ColorType_make<T>(kColorModel);
	static constexpr Int		kElementCount	= N;
	static constexpr Int		kAlphaBits		= AX_SIZEOF(a);

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	// AX_TYPE_INFO(Color_, NoBaseClass) {
	// 	static StrView s_name() {
	// 		static auto s = Fmt("ColorBGRA_<{}>", ax_typeof<T>::s_name());
	// 		return s;
	// 	}
	//
	// 	AX_FIELD_INFO(b) {};
	// 	AX_FIELD_INFO(g) {};
	// 	AX_FIELD_INFO(r) {};
	// 	AX_FIELD_INFO(a) {};
	// };

	AX_INLINE explicit Color_() = default;
	AX_INLINE          Color_(T b_, T g_, T r_, T a_ = kElemOne) { set(b_,g_,r_,a_); }

	AX_INLINE		 T* data()			{ return &b; }
	AX_INLINE const  T* data() const	{ return &b; }

	using CSpan =    Span<Element>;
	using MSpan = MutSpan<Element>;

	using CFixedSpan =    FixedSpan<Element, kElementCount>;
	using MFixedSpan = MutFixedSpan<Element, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr void set(T b_, T g_, T r_, T a_ = kElemOne) { b=b_; g=g_; r=r_; a=a_; }

	AX_INLINE bool	operator==	(const This& rhs) const { return b == rhs.b && g == rhs.g && r == rhs.r && a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return b != rhs.b || g != rhs.g || r != rhs.r || a != rhs.a; }

	static const This& kZero		() { static This s(kElemZero(), kElemZero(), kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero(), kElemZero(), kElemZero()); return s; }
	static const This& kWhite		() { static This s(kElemOne (), kElemOne (), kElemOne ()); return s; }
	static const This& kRed			() { static This s(kElemZero(), kElemZero(), kElemOne ()); return s; }
	static const This& kGreen		() { static This s(kElemZero(), kElemOne (), kElemZero()); return s; }
	static const This& kBlue		() { static This s(kElemOne (), kElemZero(), kElemZero()); return s; }
	static const This& kYellow		() { static This s(kElemZero(), kElemOne (), kElemOne ()); return s; }
	static const This& kCyan		() { static This s(kElemOne (), kElemOne (), kElemZero()); return s; }
	static const This& kMagenta		() { static This s(kElemOne (), kElemZero(), kElemOne ()); return s; }
	static const This& kGray		() { static This s(kElemHalf(), kElemHalf(), kElemHalf()); return s; }
	static const This& kDarkRed		() { static This s(kElemZero(), kElemZero(), kElemHalf()); return s; }
	static const This& kDarkGreen	() { static This s(kElemZero(), kElemHalf(), kElemZero()); return s; }
	static const This& kDarkBlue	() { static This s(kElemHalf(), kElemZero(), kElemZero()); return s; }
	static const This& kDarkYellow	() { static This s(kElemZero(), kElemHalf(), kElemHalf()); return s; }
	static const This& kDarkCyan	() { static This s(kElemHalf(), kElemHalf(), kElemZero()); return s; }
	static const This& kDarkMagenta	() { static This s(kElemHalf(), kElemZero(), kElemHalf()); return s; }
};

} // namespace ax