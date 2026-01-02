module;

export module AxCore.Color:ColorRGBA;
export import :ColorType;

export namespace ax {

template<class T, VecSimd SIMD>
class Color_<ColorModel::R, T, SIMD> {
	static constexpr Int N = 1;
public:
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T r; };
	};
	AX_META_TYPE(Color_, NoBaseClass) {
		AX_META_FIELD(r) {};
		using OwnFields = Tuple<r>;
	};

	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSimd	kVecSimd		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::R;
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
	
	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	AX_INLINE Color_() = default;
	AX_INLINE explicit Color_(T r_) : _simd(r) {}

	AX_INLINE constexpr 	  T* data()			{ return &r; }
	AX_INLINE constexpr const T* data() const	{ return &r; }

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr void set(T r_) { _simd = SimdData(r_); }

	AX_INLINE bool	operator==	(const This& rhs) const { return _simd == rhs._simd; }

	static const This& kZero		() { static This s(kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero()); return s; }
	static const This& kRed			() { static This s(kElemOne ()); return s; }
	static const This& kDarkRed		() { static This s(kElemHalf()); return s; }

	Num1_<T>	toNum() const { return Num1_<T>(r); }
};

template<class T, VecSimd SIMD>
class Color_<ColorModel::RG, T, SIMD> {
	static constexpr Int N = 2;
public:
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T r, g; };
	};
	AX_META_TYPE(Color_, NoBaseClass) {
		AX_META_FIELD(r) {};
		AX_META_FIELD(g) {};
		using OwnFields = Tuple<r,g>;
	};

	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSimd	kVecSimd		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::RG;
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

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	AX_INLINE Color_() = default;
	AX_INLINE explicit constexpr Color_(T r_, T g_) { set(r_,g_); }

	AX_INLINE constexpr 	  T* data()			{ return _simd.data(); }
	AX_INLINE constexpr const T* data() const	{ return _simd.data(); }

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr void set(T r_, T g_) { r=r_; g=g_; }

	AX_INLINE bool	operator==	(const This& rhs) const { return _simd == rhs._simd; }

//---
	static const This& kZero		() { static This s(kElemZero(), kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero(), kElemZero()); return s; }
	static const This& kRed			() { static This s(kElemOne (), kElemZero()); return s; }
	static const This& kGreen		() { static This s(kElemZero(), kElemOne ()); return s; }
	static const This& kYellow		() { static This s(kElemOne (), kElemOne ()); return s; }
	static const This& kDarkRed		() { static This s(kElemHalf(), kElemZero()); return s; }
	static const This& kDarkGreen	() { static This s(kElemZero(), kElemHalf()); return s; }
	static const This& kDarkYellow	() { static This s(kElemHalf(), kElemHalf()); return s; }

	Num2_<T>	toNum() const { return Num2_<T>(r, g); }
};

template<class T, VecSimd SIMD>
class Color_<ColorModel::RGB, T, SIMD> {
	static constexpr Int N = 3;
public:
	using Num3 = Num3_<T>;
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T r, g, b; };
	};
	AX_META_TYPE(Color_, NoBaseClass) {
		AX_META_FIELD(r) {};
		AX_META_FIELD(g) {};
		AX_META_FIELD(b) {};
		using OwnFields = Tuple<r,g,b>;
	};	

	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSimd	kVecSimd		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::RGB;
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

	using ElemLimit = ColorElemLimit<T>;
	AX_INLINE static constexpr T kElemZero () { return ElemLimit::kZero(); }
	AX_INLINE static constexpr T kElemOne  () { return ElemLimit::kOne();  }
	AX_INLINE static constexpr T kElemHalf () { return ElemLimit::kHalf(); }

	AX_INLINE Color_() = default;
	AX_INLINE explicit constexpr Color_(T r_, T g_, T b_) : r(r_), g(g_), b(b_) {}

	AX_INLINE constexpr void set(T r_, T g_, T b_) { r = r_; g = g_; b = b_; }
	AX_INLINE constexpr void set(const ColorRGB& v) { set(v.r, v.g, v.b); }

	constexpr Num3 toNum() const { return Num3(r,g,b); }
	constexpr operator Num3() const { return toNum(); }
		
	AX_INLINE constexpr 	  T* data()			{ return &r; }
	AX_INLINE constexpr const T* data() const	{ return &r; }

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

	static const This& kZero		() { static This s(kElemZero(), kElemZero(), kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero(), kElemZero(), kElemZero()); return s; }
	static const This& kWhite		() { static This s(kElemOne (), kElemOne (), kElemOne ()); return s; }
	static const This& kRed			() { static This s(kElemOne (), kElemZero(), kElemZero()); return s; }
	static const This& kGreen		() { static This s(kElemZero(), kElemOne (), kElemZero()); return s; }
	static const This& kBlue		() { static This s(kElemZero(), kElemZero(), kElemOne ()); return s; }
	static const This& kYellow		() { static This s(kElemOne (), kElemOne (), kElemZero()); return s; }
	static const This& kCyan		() { static This s(kElemZero(), kElemOne (), kElemOne ()); return s; }
	static const This& kMagenta		() { static This s(kElemOne (), kElemZero(), kElemOne ()); return s; }
	static const This& kGray		() { static This s(kElemHalf(), kElemHalf(), kElemHalf()); return s; }
	static const This& kDarkRed		() { static This s(kElemHalf(), kElemZero(), kElemZero()); return s; }
	static const This& kDarkGreen	() { static This s(kElemZero(), kElemHalf(), kElemZero()); return s; }
	static const This& kDarkBlue	() { static This s(kElemZero(), kElemZero(), kElemHalf()); return s; }
	static const This& kDarkYellow	() { static This s(kElemHalf(), kElemHalf(), kElemZero()); return s; }
	static const This& kDarkCyan	() { static This s(kElemZero(), kElemHalf(), kElemHalf()); return s; }
	static const This& kDarkMagenta	() { static This s(kElemHalf(), kElemZero(), kElemHalf()); return s; }

	void toHexString(IString& s) const;

	template<class R, VecSimd R_SIMD> static constexpr This s_cast(const ColorRGB_<R, R_SIMD>& rhs) {
		return SimdData::s_cast(rhs);
	}
};

template<class T, VecSimd SIMD>
class Color_<ColorModel::RGBA, T, SIMD> {
	static constexpr Int N = 4;
public:
	using Num4 = Num4_<T>;
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T r, g, b, a; };
	};
	AX_META_TYPE(Color_, NoBaseClass) {
		AX_META_FIELD(r) {};
		AX_META_FIELD(g) {};
		AX_META_FIELD(b) {};
		AX_META_FIELD(a) {};
		using OwnFields = Tuple<r,g,b,a>;
	};

	using _NumLimit = VecSimd_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSimd	kVecSimd		= SIMD;
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

//---
	AX_INLINE Color_() = default;
	AX_INLINE explicit constexpr Color_(const T& r_, const T& g_, const T& b_, const T& a_ = kElemOne())
		: _simd(r_, g_, b_, a_) {}

	AX_INLINE explicit constexpr Color_(const ColorRGB& v, T a_ = kElemOne()) 
		: Color_(v.r, v.g, v.b, a) {}

	AX_INLINE constexpr void set(const T& r_, const T& g_, const T& b_, const T& a_ = kElemOne())
		{ _simd = SimdData(r_, g_, b_, a_); }

	AX_INLINE constexpr void set(const ColorRGB& v, T a_ = kElemOne()) 
		{ set(v.r, v.g, v.b, a); }

	constexpr Num4 toNum() const { return Num4(r,g,b,a); }
	constexpr operator Num4() const { return toNum(); }
	
	AX_INLINE constexpr 	  T* data()			{ return &r; }
	AX_INLINE constexpr const T* data() const	{ return &r; }

	using CSpan =    Span<T>;
	using MSpan = MutSpan<T>;

	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }

	AX_INLINE constexpr bool operator==	(const This& rhs) const { return _simd == rhs._simd; }

	// template<VecSimd R_SIMD>
	// AX_INLINE constexpr bool almostEqual( const ColorRGBA_<T, SIMD>& rhs) const { return _simd.almostEqual(rhs._simd); }
	AX_INLINE constexpr bool almostZero(  const This& rhs) const { return _simd.almostZero(rhs._simd); }
	AX_INLINE constexpr bool exactlyEqual(const This& rhs) const { return _simd.exactlyEqual(rhs._simd); }

	static const This& kZero		() { static This s(kElemZero(), kElemZero(), kElemZero(), kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero(), kElemZero(), kElemZero()); return s; }
	static const This& kWhite		() { static This s(kElemOne (), kElemOne (), kElemOne ()); return s; }
	static const This& kRed			() { static This s(kElemOne (), kElemZero(), kElemZero()); return s; }
	static const This& kGreen		() { static This s(kElemZero(), kElemOne (), kElemZero()); return s; }
	static const This& kBlue		() { static This s(kElemZero(), kElemZero(), kElemOne ()); return s; }
	static const This& kYellow		() { static This s(kElemOne (), kElemOne (), kElemZero()); return s; }
	static const This& kCyan		() { static This s(kElemZero(), kElemOne (), kElemOne ()); return s; }
	static const This& kMagenta		() { static This s(kElemOne (), kElemZero(), kElemOne ()); return s; }
	static const This& kGray		() { static This s(kElemHalf(), kElemHalf(), kElemHalf()); return s; }
	static const This& kDarkRed		() { static This s(kElemHalf(), kElemZero(), kElemZero()); return s; }
	static const This& kDarkGreen	() { static This s(kElemZero(), kElemHalf(), kElemZero()); return s; }
	static const This& kDarkBlue	() { static This s(kElemZero(), kElemZero(), kElemHalf()); return s; }
	static const This& kDarkYellow	() { static This s(kElemHalf(), kElemHalf(), kElemZero()); return s; }
	static const This& kDarkCyan	() { static This s(kElemZero(), kElemHalf(), kElemHalf()); return s; }
	static const This& kDarkMagenta	() { static This s(kElemHalf(), kElemZero(), kElemHalf()); return s; }

	This toPremultipliedAlpha	() const {
		auto scalar = ColorElemUtil::to_f32(a);
		return This(ColorElemUtil::mul_f32(r, scalar),
					ColorElemUtil::mul_f32(g, scalar),
					ColorElemUtil::mul_f32(b, scalar),
					a);
	}
	This toNonPremultipliedAlpha() const {
		auto scalar = ax_div(1.0, ColorElemUtil::toDouble(a));
		return This(ColorElemUtil::mulDouble(r, scalar),
					ColorElemUtil::mulDouble(g, scalar),
					ColorElemUtil::mulDouble(b, scalar),
					a);
	}
	
	
	void toHexString(IString& s) const;
	
	template<class R, VecSimd R_SIMD> static constexpr This s_cast(const ColorRGBA_<R, R_SIMD>& rhs) {
		return SimdData::s_cast(rhs._simd);
	}
};

template <class T, VecSimd SIMD> inline
void Color_<ColorModel::RGB, T, SIMD>::toHexString(IString& s) const {
	s.clear();
	s.ensureCapacity(10);
	auto tmp = ColorRGBb::s_cast(*this);
	s.append("#");
	s.append(CharUtil::byteToHex<Char>(tmp.r.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.g.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.b.v_int));
}

template<class T, VecSimd SIMD> inline
void Color_<ColorModel::RGBA, T, SIMD>::toHexString(IString& s) const {
	s.clear();
	s.ensureCapacity(10);
	auto tmp = ColorRGBAb::s_cast(*this);
	s.append("#");
	s.append(CharUtil::byteToHex<Char>(tmp.r.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.g.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.b.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.a.v_int));
}


} // namespace ax
