module;

export module AxRender.ColorHSVA;
export import AxRender.ColorRGBA;

export namespace ax {

template<class T, VecSIMD SIMD>
class Color_<ColorModel::HSVA, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 4;	
public:
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T h, s, v, a; };
	};

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSIMD	kVecSIMD		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::RGBA;
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
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(h) {};
		AX_META_FIELD(s) {};
		AX_META_FIELD(v) {};
		AX_META_FIELD(a) {};
		using OwnFields = Tuple<h,s,v,a>;
	};	
	
	AX_INLINE explicit Color_() = default;
	AX_INLINE          Color_(T h_, T s_, T v_, T a_ = kElemOne) : _simd(h_,s_,v_,a_) {}

	AX_INLINE		T* data()		{ return _simd.data(); }
	AX_INLINE const T* data() const	{ return _simd.data(); }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr    FixedSpan<T, kElementCount> fixedSpan() const { return    FixedSpan<T, kElementCount>(data()); }
	AX_INLINE constexpr MutFixedSpan<T, kElementCount> fixedSpan()       { return MutFixedSpan<T, kElementCount>(data()); }
	AX_INLINE constexpr    Span<T> span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MutSpan<T> span()			{ return fixedSpan(); }

	AX_INLINE constexpr void set(T h_, T s_, T v_, T a_ = kElemOne) { _simd = SIMD_Data(h_,s_,v_,a_); }
	AX_INLINE bool	operator==	(const This& rhs) const { return _simd == rhs._simd; }
};

} // namespace ax