module;

export module AxCore.Color:ColorLA;
export import :ColorType;

export namespace ax {

template<class T, VecSIMD SIMD>
class Color_<ColorModel::L, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 1;
public:
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T luma; };
	};

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSIMD	kVecSIMD		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::L;
	static constexpr ColorElem	kColorElem		= ColorElem_get<T>;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	= N;
	static constexpr Int		kAlphaBits		= 0;
	
	using ColorR	= ColorR_   <T, SIMD>;
	using ColorRG	= ColorRG_  <T, SIMD>;
	using ColorRGB	= ColorRGB_ <T, SIMD>;
	using ColorRGBA = ColorRGBA_<T, SIMD>;
	using ColorL	= ColorL_   <T, SIMD>;
	using ColorLA	= ColorLA_  <T, SIMD>;
	using Element = T;

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(luma) {};
		using OwnFields = Tuple<luma>;
	};

	AX_INLINE explicit Color_() = default;
	AX_INLINE          Color_(T luma_) { set(luma_); }

	AX_INLINE constexpr       T* data()			{ return &luma; }
	AX_INLINE constexpr const T* data() const	{ return &luma; }

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

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

template<class T, VecSIMD SIMD>
class Color_<ColorModel::LA, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 2;
public:
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T luma, a; };
	};

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSIMD	kVecSIMD		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::LA;
	static constexpr ColorElem	kColorElem		= ColorElem_get<T>;
	static constexpr ColorType	kColorType		= ColorType_make(kColorModel, kColorElem);
	static constexpr Int		kElementCount	= N;
	static constexpr Int		kAlphaBits		= AX_SIZEOF(a);
	
	using ColorR	= ColorR_   <T, SIMD>;
	using ColorRG	= ColorRG_  <T, SIMD>;
	using ColorRGB	= ColorRGB_ <T, SIMD>;
	using ColorRGBA = ColorRGBA_<T, SIMD>;
	using ColorL	= ColorL_   <T, SIMD>;
	using ColorLA	= ColorLA_  <T, SIMD>;
	using Element = T;

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(luma) {};
		AX_META_FIELD(a) {};
		using OwnFields = Tuple<luma, a>;
	};
	
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

	AX_INLINE constexpr bool	operator==	(const This& rhs) const { return luma == rhs.luma && a == rhs.a; }
	AX_INLINE constexpr bool	operator!=	(const This& rhs) const { return luma != rhs.luma || a != rhs.a; }

	static const This& kZero		() { static This s(kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero()); return s; }
	static const This& kWhite		() { static This s(kElemOne ()); return s; }
	static const This& kGray		() { static This s(kElemHalf()); return s; }
};

} // namespace ax