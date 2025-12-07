module;

export module AxRender.ColorRGBA;
export import AxRender.ColorType;

export namespace ax {

template<class T, VecSIMD SIMD>
class Color_<ColorModel::R, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 1;
public:
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T r; };
	};

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSIMD	kVecSIMD		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::RGBA;
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

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(r) {};
		using OwnFields = Tuple<r>;
	};

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

	AX_INLINE constexpr void set(T r_) { _simd = SIMD_Data(r_); }

	AX_INLINE bool	operator==	(const This& rhs) const { return _simd == rhs._simd; }

	static const This& kZero		() { static This s(kElemZero()); return s; }
	static const This& kBlack		() { static This s(kElemZero()); return s; }
	static const This& kRed			() { static This s(kElemOne ()); return s; }
	static const This& kDarkRed		() { static This s(kElemHalf()); return s; }
};

template<class T, VecSIMD SIMD>
class Color_<ColorModel::RG, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 2;
public:
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T r, g; };
	};

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSIMD	kVecSIMD		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::RGBA;
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

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(r) {};
		AX_META_FIELD(g) {};
		using OwnFields = Tuple<r,g>;
	};

//---
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
};

template<class T, VecSIMD SIMD>
class Color_<ColorModel::RGB, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 3;
public:
	using Num4 = Num4_<T>;
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T r, g, b; };
	};

	using _NumLimit = VecSIMD_NumLimit<This, T>;
	using ElementType = T;
	static constexpr VecSIMD	kVecSIMD		= SIMD;
	static constexpr ColorModel	kColorModel		= ColorModel::RGBA;
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

	struct MetaTypeInit : AX_META_TYPE() {
		AX_META_FIELD(r) {};
		AX_META_FIELD(g) {};
		AX_META_FIELD(b) {};
		using OwnFields = Tuple<r,g,b>;
	};

	AX_INLINE Color_() = default;
	AX_INLINE explicit constexpr Color_(T r_, T g_, T b_) : r(r_), g(g_), b(b_) {}

	AX_INLINE constexpr void set(T r_, T g_, T b_) { r = r_; g = g_; b = b_; }
	AX_INLINE constexpr void set(const ColorRGB& v) { set(v.r, v.g, v.b); }

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

	template<class R, VecSIMD R_SIMD> static constexpr This s_cast(const ColorRGB_<R, R_SIMD>& rhs) {
		return _simd.s_cast(rhs);
	}
};

template<class T, VecSIMD SIMD>
class Color_<ColorModel::RGBA, T, SIMD> {
	AX_TYPE_INFO(Color_, NoBaseClass)
	static constexpr Int N = 4;
public:
	using Num4 = Num4_<T>;
	using SIMD_Data = VecSIMD_Data_<N,T,SIMD>; 
	union {
		SIMD_Data	_simd;
		struct { T r, g, b, a; };
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
		AX_META_FIELD(r) {};
		AX_META_FIELD(g) {};
		AX_META_FIELD(b) {};
		AX_META_FIELD(a) {};
		using OwnFields = Tuple<r,g,b,a>;
	};

//---
	AX_INLINE Color_() = default;
	AX_INLINE explicit constexpr Color_(const T& r_, const T& g_, const T& b_, const T& a_ = kElemOne())
		: _simd(r_, g_, b_, a_) {}

	AX_INLINE explicit constexpr Color_(const ColorRGB& v, T a_ = kElemOne()) 
		: Color_(v.r, v.g, v.b, a) {}

	AX_INLINE constexpr void set(const T& r_, const T& g_, const T& b_, const T& a_ = kElemOne())
		{ _simd = SIMD_Data(r_, g_, b_, a_); }

	AX_INLINE constexpr void set(const ColorRGB& v, T a_ = kElemOne()) 
		{ set(v.r, v.g, v.b, a); }

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

	AX_INLINE constexpr bool operator==	(const This& rhs) const { _simd == rhs._simd; }

	// template<VecSIMD R_SIMD>
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

	Num4_<T>	to_Num() const { return Num4_<T>(r, g, b, a); }
	
	void toHexString(IString& s) const;

	template<class R, VecSIMD R_SIMD> static constexpr This s_cast(const ColorRGBA_<R, R_SIMD>& rhs) {
		return _simd.s_cast(rhs);
	}
};

template <class T, VecSIMD SIMD> inline
void Color_<ColorModel::RGB, T, SIMD>::toHexString(IString& s) const {
	s.clear();
	s.reserve(10);
	auto tmp = ColorRGBb::s_cast(*this);
	s.append("#");
	s.append(CharUtil::byteToHex<Char>(tmp.r.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.g.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.b.v_int));
}

template<class T, VecSIMD SIMD> inline
void Color_<ColorModel::RGBA, T, SIMD>::toHexString(IString& s) const {
	s.clear();
	s.reserve(10);
	auto tmp = ColorRGBAb::s_cast(*this);
	s.append("#");
	s.append(CharUtil::byteToHex<Char>(tmp.r.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.g.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.b.v_int));
	s.append(CharUtil::byteToHex<Char>(tmp.a.v_int));
}


} // namespace ax
