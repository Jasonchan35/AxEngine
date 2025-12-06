module;

export module AxRender.ColorA;
export import AxRender.ColorType;

export namespace ax {

template<class T>
class Color_<ColorModel::A, T> {
	AX_TYPE_INFO(Color_, NoBaseClass)
public:
	using Element = T;
	T a;

	static constexpr ColorModel	kColorModel		= ColorModel::A;
	static constexpr ColorElem	kColorElem		= ColorElem_get<T>;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	= 1;
	static constexpr Int		kAlphaBits		= AX_SIZEOF(a);

	using ElemLimit = ColorElemLimit<T>;
	inline static const T& kElemZero () { return ElemLimit::kZero(); }
	inline static const T& kElemOne  () { return ElemLimit::kOne();  }
	inline static const T& kElemHalf () { return ElemLimit::kHalf(); }

	// struct MetaTypeInit : AX_META_TYPE_INIT(This) {
	// 	static NameId s_name() {
	// 		static NameId s = AX_NAMEID(Fmt("ColorA_<{}>", MetaTypeOf<T>::s_name()));
	// 		return s;
	// 	}
	// 	struct a : public AX_META_FIELD_INIT(a) {};
	// 	using OwnFields = Tuple<a>;
	// };

//---

	AX_INLINE constexpr explicit Color_() = default;
	AX_INLINE constexpr          Color_(T a_) { set(a_); }

	AX_INLINE constexpr       T* data()			{ return &a; }
	AX_INLINE constexpr const T* data() const	{ return &a; }

	AX_INLINE constexpr void set(T a_) { a=a_; }

	using CSpan =    Span<Element>;
	using MSpan = MutSpan<Element>;

	using CFixedSpan =    FixedSpan<Element, kElementCount>;
	using MFixedSpan = MutFixedSpan<Element, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr bool	operator==	(const This& rhs) const { return a == rhs.a; }
	AX_INLINE constexpr bool	operator!=	(const This& rhs) const { return a != rhs.a; }

	static const This& kZero		() { static This s(kElemZero()); return s; }
};

} // namespace ax