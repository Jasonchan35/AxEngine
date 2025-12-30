module;

export module AxCore.Color:ColorA;
export import :ColorType;

export namespace ax {

template<class T, VecSimd SIMD>
class Color_<ColorModel::A, T, SIMD> {
	static constexpr Int N = 1;	
public:
	using Num4 = Num4_<T>;
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T a; };
	};	
	AX_META_TYPE(Color_, NoBaseClass) {
		AX_META_FIELD(a) {};
		using OwnFields = Tuple<a>;
	};

	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSimd	kVecSimd		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::A;
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

	using ElemLimit = ColorElemLimit<T>;
	inline static const T& kElemZero () { return ElemLimit::kZero(); }
	inline static const T& kElemOne  () { return ElemLimit::kOne();  }
	inline static const T& kElemHalf () { return ElemLimit::kHalf(); }

//---

	AX_INLINE constexpr explicit Color_() = default;
	AX_INLINE constexpr          Color_(T a_) { set(a_); }

	AX_INLINE constexpr       T* data()			{ return &a; }
	AX_INLINE constexpr const T* data() const	{ return &a; }

	AX_INLINE constexpr void set(T a_) { a=a_; }

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr bool	operator==	(const This& rhs) const { return _simd == rhs._simd; }

	static const This& kZero		() { static This s(kElemZero()); return s; }
};

} // namespace ax