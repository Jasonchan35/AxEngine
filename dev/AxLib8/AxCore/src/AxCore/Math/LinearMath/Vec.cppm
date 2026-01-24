module;
export module AxCore.LinearMath:Vec;
export import AxCore.VecSimd;
export import AxCore.Random;
export import AxCore.MetaType;
export import AxCore.NormInt;

export namespace  ax {

template<Int N, class T, VecSimd SIMD = VecSimd_Default> class Vec_;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec1_ = Vec_<1, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec2_ = Vec_<2, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec3_ = Vec_<3, T, SIMD>;
template<class T, VecSimd SIMD = VecSimd_Default> using Vec4_ = Vec_<4, T, SIMD>;

template<class T> constexpr bool Type_IsVec = false;
template<Int N, class T, VecSimd SIMD> constexpr bool Type_IsVec<Vec_<N,T,SIMD>> = true;

#define AX_Vec_USING(T, SUFFIX) \
	using Vec1##SUFFIX			= Vec1_<T>;                   \
	using Vec1##SUFFIX##_SSE	= Vec1_<T, VecSimd::SSE>;     \
	using Vec1##SUFFIX##_Basic	= Vec1_<T, VecSimd::Basic>;   \
	using Vec2##SUFFIX			= Vec2_<T>;                   \
	using Vec2##SUFFIX##_SSE	= Vec2_<T, VecSimd::SSE>;     \
	using Vec2##SUFFIX##_Basic	= Vec2_<T, VecSimd::Basic>;   \
	using Vec3##SUFFIX			= Vec3_<T>;                   \
	using Vec3##SUFFIX##_SSE	= Vec3_<T, VecSimd::SSE>;     \
	using Vec3##SUFFIX##_Basic	= Vec3_<T, VecSimd::Basic>;   \
	using Vec4##SUFFIX			= Vec4_<T>;                   \
	using Vec4##SUFFIX##_SSE	= Vec4_<T, VecSimd::SSE>;     \
	using Vec4##SUFFIX##_Basic	= Vec4_<T, VecSimd::Basic>;   \
//----
AX_Vec_USING(Int, i);
AX_Vec_USING(f16, h);
AX_Vec_USING(f32, f);
AX_Vec_USING(f64, d);

AX_Vec_USING(i8,  i8);
AX_Vec_USING(i16, i16);
AX_Vec_USING(i32, i32);
AX_Vec_USING(i64, i64);

AX_Vec_USING(u8,  u8);
AX_Vec_USING(u16, u16);
AX_Vec_USING(u32, u32);
AX_Vec_USING(u64, u64);

AX_Vec_USING(UNorm8,  unorm8);
AX_Vec_USING(UNorm16, unorm16);
AX_Vec_USING(UNorm32, unorm32);

AX_Vec_USING(SNorm8,  snorm8);
AX_Vec_USING(SNorm16, snorm16);
AX_Vec_USING(SNorm32, snorm32);


template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator+(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) + vec;
}

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator-(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) - vec;
}

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator*(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) * vec;
}

template <Int N, class T, VecSimd SIMD>
AX_NODISCARD AX_INLINE constexpr Vec_<N, T, SIMD> operator/(const T& t, const Vec_<N, T, SIMD>& vec) {
	return Vec_<N, T, SIMD>::s_all(t) / vec;
}

template<class T, VecSimd SIMD>
class Vec_<1, T, SIMD> {
	static constexpr Int N = 1;
public:
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData _simd;
		struct { T x; };
		T e[kElementCount];
	};
	AX_META_TYPE(Vec_, NoBaseClass) {
		AX_META_FIELD(x) {};
		using OwnFields = Tuple<x>;
	};	
	using _NumLimit = VecSimd_NumLimit<This, T>;
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
//	AX_INLINE constexpr Vec_(const Num1& v) : _simd(v.e00) {}
	AX_INLINE constexpr Vec_(const T& x_) : _simd(x_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	template<VecSimd R_SIMD>
	AX_INLINE Vec_(const Vec_<1,T,R_SIMD>& v) : Vec_(v.x) {} 
	
	
	AX_INLINE constexpr void set(const T& x_) { *this = This(x_); }

	constexpr Vec_<N,T,VecSimd::Basic> to_Basic() const { return Vec_<N,T,VecSimd::Basic>(x); }

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); } 

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec, T epsilon = Math::epsilon_<T>) const { 
		return _simd.almostEqual(vec._simd, epsilon); 
	}
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(T epsilon = Math::epsilon_<T>()) const { return _simd.almostZero(epsilon); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }
	
	AX_NODISCARD AX_INLINE constexpr This operator-() const	{ return -_simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }

	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr       T* data()       { return _simd.data(); }
	AX_INLINE constexpr const T* data() const { return _simd.data(); }

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

template<class T, VecSimd SIMD>
class Vec_<2, T, SIMD> {
	static constexpr Int N = 2;
public:
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
		
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y; };
		T e[kElementCount];
	};
	
	AX_META_TYPE(Vec_, NoBaseClass) {
		AX_META_FIELD(x) {};
		AX_META_FIELD(y) {};
		using OwnFields = Tuple<x,y>;
	};	
	using _NumLimit = VecSimd_NumLimit<This, T>;
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
//	AX_INLINE constexpr Vec_(const Num2& v) : _simd(v.e00, v.e01) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_) : _simd(x_, y_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	template<VecSimd R_SIMD>
	AX_INLINE Vec_(const Vec_<2,T,R_SIMD>& v) : Vec_(v.x, v.y) {} 

	AX_INLINE constexpr void set(const T& x_, const T& y_) { *this = This(x_,y_); }

	constexpr Vec_<N,T,VecSimd::Basic> to_Basic() const { return Vec_<N,T,VecSimd::Basic>(x,y); }

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); } 

	AX_NODISCARD AX_INLINE constexpr Vec2 xy() const { return Vec2(x,y); }
	AX_NODISCARD AX_INLINE constexpr Vec2 yx() const { return Vec2(y,x); }

	AX_NODISCARD AX_INLINE constexpr Vec3 xy0 () const { return Vec3(x,y,0); }
	AX_NODISCARD AX_INLINE constexpr Vec3 yx0 () const { return Vec3(y,x,0); }
	
	AX_NODISCARD AX_INLINE constexpr Vec4 xy01() const { return Vec4(x,y,0,1); }
	AX_NODISCARD AX_INLINE constexpr Vec4 xy00() const { return Vec4(x,y,0,0); }
	
	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec, T epsilon = Math::epsilon_<T>()) const { 
		return _simd.almostEqual(vec._simd, epsilon); 
	}
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(T epsilon = Math::epsilon_<T>()) const { return _simd.almostZero(epsilon); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }

	AX_NODISCARD AX_INLINE constexpr This operator-() const	{ return -_simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	AX_INLINE constexpr T dot(const This& r) const { return _simd.dot(r); }
	
	AX_INLINE constexpr This rotateLeft90() const  { return This( y,-x); }
	AX_INLINE constexpr This rotateRight90()const  { return This(-y, x); }

	AX_NODISCARD AX_INLINE constexpr T    lengthSq() const { return dot(*this); }
	AX_NODISCARD AX_INLINE constexpr T    length() const { return Math::sqrt(lengthSq()); }
	AX_NODISCARD AX_INLINE constexpr T    invLength() const { return Math::rsqrt(lengthSq()); }
	AX_NODISCARD AX_INLINE constexpr T    distance(const This& r) const { return (*this - r).length(); }
	AX_NODISCARD AX_INLINE constexpr T    distanceSq(const This& r) const { return (*this - r).lengthSq(); }
	AX_NODISCARD AX_INLINE constexpr This normalize() const {
		auto d = length();
		if (Math::almostZero(d)) return s_zero();
		return *this / d;
	}

	AX_INLINE constexpr void normalizeSelf() { *this = normalize(); };

	static This s_randomUnitVector(RandomDevice& dev = RandomDevice::s_default()) {
		auto theta = dev.fromRange<T>(0, 1) * 2 * Math::PI_<T>;
		This v;
		Math::sincos(theta, v.x, v.y);
		return v;
	}

	static Vec2_<T, SIMD> s_randomInCircle(RandomDevice& dev = RandomDevice::s_default()) {
		auto v = s_randomUnitVector(dev);
		auto r = dev.fromRange<T>(0, 1);
		return v * Math::sqrt(r); // distribution in square root along radius
	}
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr       T* data()       { return _simd.data(); }
	AX_INLINE constexpr const T* data() const { return _simd.data(); }

	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

template<class T, VecSimd SIMD>
class Vec_<3, T, SIMD> {
	static constexpr Int N = 3;
public:
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;
	
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y, z; };
		T e[kElementCount];
	};
	
	AX_META_TYPE(Vec_, NoBaseClass) {
		AX_META_FIELD(x) {};
		AX_META_FIELD(y) {};
		AX_META_FIELD(z) {};
		using OwnFields = Tuple<x,y,z>;
	};
	
	using _NumLimit = VecSimd_NumLimit<This, T>;

	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;

	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
//	AX_INLINE constexpr Vec_(const Num3& v) : _simd(v.e00, v.e01, v.e02) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_, const T& z_) : _simd(x_, y_, z_) {}
	AX_INLINE constexpr Vec_(const Vec2& v_, const T& z_) : _simd(v_.x, v_.y, z_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	template<VecSimd R_SIMD>
	AX_INLINE Vec_(const Vec_<3,T,R_SIMD>& v) : Vec_(v.x, v.y, v.z) {} 

	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_) { *this = This(x_,y_,z_); }
	AX_INLINE constexpr void set(const Vec2& v_, const T& z_) { *this = This(v_.x,v_.y,z_); }

	constexpr Vec_<N,T,VecSimd::Basic> to_Basic() const { return Vec_<N,T,VecSimd::Basic>(x,y,z); }

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); }

	AX_NODISCARD AX_INLINE constexpr Vec2 xy() const { return Vec2(x,y); }
	
	AX_NODISCARD AX_INLINE constexpr Vec4 xyz0() const { return Vec4(x,y,z,0); }
	AX_NODISCARD AX_INLINE constexpr Vec4 xyz1() const { return Vec4(x,y,z,1); }
	
	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec, T epsilon = Math::epsilon_<T>()) const { 
		return _simd.almostEqual(vec._simd, epsilon); 
	}
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(T epsilon = Math::epsilon_<T>()) const { return _simd.almostZero(epsilon); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }

	AX_NODISCARD AX_INLINE constexpr This operator-() const	{ return -_simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }

	AX_INLINE constexpr T dot(const This& v) const { return _simd.dot(v._simd); }
	AX_INLINE constexpr This cross(const This& v) const { return This(y*v.z - z*v.y, z*v.x - x*v.z,x*v.y - y*v.x); }

	AX_NODISCARD AX_INLINE	constexpr bool isParallel(const This& r) const { return cross(r).almostZero(); }

	AX_NODISCARD AX_INLINE constexpr T    lengthSq() const { return dot(*this); }
	AX_NODISCARD AX_INLINE constexpr T    length() const { return Math::sqrt(lengthSq()); }
	AX_NODISCARD AX_INLINE constexpr T    invLength() const { return Math::rsqrt(lengthSq()); }
	AX_NODISCARD AX_INLINE constexpr T    distance(const This& r) const { return (*this - r).length(); }
	AX_NODISCARD AX_INLINE constexpr T    distanceSq(const This& r) const { return (*this - r).lengthSq(); }
	AX_NODISCARD AX_INLINE constexpr This normalize() const {
		auto d = length();
		if (Math::almostZero(d)) return s_zero();
		return *this / d;
	}

	AX_INLINE constexpr void normalizeSelf() { *this = normalize(); };
	
	static This s_randomUnitVector(RandomDevice& dev = RandomDevice::s_default()) {
		auto longitude = dev.fromRange<T>(0, 1) * 2 * Math::PI_<T>;
		auto latitude  = acos(dev.fromRange<T>(0, 1) * 2 - 1);

		T sinLongitude, cosLongitude;
		T sinLatitude,  cosLatitude;

		Math::sincos(longitude, sinLongitude, cosLongitude);
		Math::sincos(latitude, sinLatitude, cosLatitude);

		Vec3_<T, SIMD> v(sinLatitude * cosLongitude, sinLatitude * sinLongitude, cosLatitude);
		return v;
	}

	static This s_randomInSphere(RandomDevice& dev = RandomDevice::s_default()) {
		auto v = s_randomUnitVector(dev);
		auto r = dev.fromRange<T>(0, 1);
		return v * cbrt(r);
	}	
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr       T* data()       { return _simd.data(); }
	AX_INLINE constexpr const T* data() const { return _simd.data(); }
	
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

template<class T, VecSimd SIMD>
class Vec_<4, T, SIMD> {
	static constexpr Int N = 4;
public:
	using ElementType = T;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
		
	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y, z, w; };
		T e[kElementCount];
	};
	
	AX_META_TYPE(Vec_, NoBaseClass) {
		AX_META_FIELD(x) {};
		AX_META_FIELD(y) {};
		AX_META_FIELD(z) {};
		AX_META_FIELD(w) {};	
		using OwnFields = Tuple<x,y,z,w>;
	};
	using _NumLimit = VecSimd_NumLimit<This, T>;
	
	AX_INLINE constexpr Vec_() = default;
	AX_INLINE constexpr Vec_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Vec_(TagAll_T, const T& vec) : _simd(SimdData::s_all(vec)) {}
//	AX_INLINE constexpr Vec_(const Num4_<T>& v) : _simd(v) {}
	AX_INLINE constexpr Vec_(const T& x_, const T& y_, const T& z_, const T& w_) : _simd(x_, y_, z_, w_) {}
	AX_INLINE constexpr Vec_(const Vec3& v_, const T& w_) : _simd(v_.x,v_.y,v_.z,w_) {}
	AX_INLINE constexpr Vec_(TagZero_T) : _simd(TagZero) {}

	template<VecSimd R_SIMD>
	AX_INLINE Vec_(const Vec_<4,T,R_SIMD>& v) : Vec_(v.x, v.y, v.z, v.w) {} 
	
	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) { *this = This(x_,y_,z_,w_); }
	AX_INLINE constexpr void set(const Vec3& v_, const T& w_) { *this = This(v_.x,v_.y,v_.z,w_); }

	constexpr Vec_<N,T,VecSimd::Basic> to_Basic() const { return Vec_<N,T,VecSimd::Basic>(x,y,z,w); }

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); }

	AX_INLINE constexpr Vec3	xyz() const	{ return Vec3(x,y,z); }
	AX_INLINE constexpr Vec3	xyz_div_w() const	{ return (*this / w).xyz(); } // de-homogenize
	AX_INLINE constexpr Vec4	xyz0() const	{ return Vec4(x,y,z,0); }
	AX_INLINE constexpr Vec4	xyz1() const	{ return Vec4(x,y,z,1); }

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec, T epsilon = Math::epsilon_<T>()) const { 
		return _simd.almostEqual(vec._simd, epsilon); 
	}
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(T epsilon = Math::epsilon_<T>()) const { return _simd.almostZero(epsilon); }

	AX_NODISCARD AX_INLINE constexpr This min(const This& vec) const { return _simd.min(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr This max(const This& vec) const { return _simd.max(vec._simd); }

	AX_NODISCARD AX_INLINE constexpr This operator-() const	{ return -_simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const This& vec) const { return _simd + vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const This& vec) const { return _simd - vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const This& vec) const { return _simd * vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const This& vec) const { return _simd / vec._simd; }
	AX_NODISCARD AX_INLINE constexpr This operator+(const T& t) const { return _simd + t; }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& t) const { return _simd - t; }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& t) const { return _simd * t; }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& t) const { return _simd / t; }

	AX_INLINE constexpr void operator+=(const This& vec) { _simd += vec._simd; }
	AX_INLINE constexpr void operator-=(const This& vec) { _simd -= vec._simd; }
	AX_INLINE constexpr void operator*=(const This& vec) { _simd *= vec._simd; }
	AX_INLINE constexpr void operator/=(const This& vec) { _simd /= vec._simd; }
	AX_INLINE constexpr void operator+=(const T& t) { _simd += t; }
	AX_INLINE constexpr void operator-=(const T& t) { _simd -= t; }
	AX_INLINE constexpr void operator*=(const T& t) { _simd *= t; }
	AX_INLINE constexpr void operator/=(const T& t) { _simd /= t; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }

	using MSpan      =      MutSpan<T>;
	using CSpan      =         Span<T>;
	using CFixedSpan =    FixedSpan<T, kElementCount>;
	using MFixedSpan = MutFixedSpan<T, kElementCount>;

	AX_INLINE constexpr       T* data()       { return _simd.data(); }
	AX_INLINE constexpr const T* data() const { return _simd.data(); }
	
	AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_simd.data()); }
	AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_simd.data()); }
	AX_INLINE constexpr CSpan span() const	{ return fixedSpan(); }
	AX_INLINE constexpr MSpan span()		{ return fixedSpan(); }
};

namespace Math {
	template<Int N, class T, VecSimd SIMD> inline
	void sincos(const Vec_<N,T,SIMD>& r, Vec_<N,T,SIMD>& out_sin, Vec_<N,T,SIMD>& out_cos) {
		for (Int i = 0; i < N; ++i) {
			Math::sincos(r.e[i], out_sin.e[i], out_cos.e[i]);
		}
	}

	template<Int N, class T, VecSimd SIMD> inline
	Vec_<N,T,SIMD> radians(const Vec_<N,T,SIMD>& deg) {
		Vec_<N,T,SIMD> o;
		for (Int i = 0; i < N; ++i) {
			o.e[i] = Math::radians(deg.e[i]);
		}
		return o;
	}

	template<Int N, class T, VecSimd SIMD> inline
	Vec_<N,T,SIMD> degrees(const Vec_<N,T,SIMD>& rad) {
		Vec_<N,T,SIMD> o;
		for (Int i = 0; i < N; ++i) {
			o.e[i] = Math::degrees(rad.e[i]);
		}
		return o;
	}

	template<Int N, class T, VecSimd SIMD> inline
	Vec_<N,T,SIMD> safeDiv(const Vec_<N,T,SIMD>& v, const T& d) {
		Vec_<N,T,SIMD> o;
		for (Int i = 0; i < N; ++i) {
			o.e[i] = Math::safeDiv(v.e[i], d);
		}
		return o;
	}
} // namespace Math




} // namespace 