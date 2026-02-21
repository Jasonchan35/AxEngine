module;

export module AxCore.LinearMath:Quat; // Quaternion
export import :Vec;

export namespace ax {

template<Int N, class T, VecSimd SIMD> class Quat_;
template<class T, VecSimd SIMD = VecSimd_Default> using Quat4_ = Quat_<4, T, SIMD>;

using Quat4h		= Quat4_<f16>;
using Quat4h_SSE	= Quat4_<f16, VecSimd::SSE>;
using Quat4h_Basic	= Quat4_<f16, VecSimd::Basic>;
using Quat4f		= Quat4_<f32>;
using Quat4f_SSE	= Quat4_<f32, VecSimd::SSE>;
using Quat4f_Basic	= Quat4_<f32, VecSimd::Basic>;
using Quat4d		= Quat4_<f64>;
using Quat4d_SSE	= Quat4_<f64, VecSimd::SSE>;
using Quat4d_Basic	= Quat4_<f64, VecSimd::Basic>;

template<class T, VecSimd SIMD>
class Quat_<4, T, SIMD> {
	static constexpr Int N = 4;
	using This = Quat_;
public:
	using _NumLimit = VecSimd_NumLimit<This, T>;
	static constexpr Int kElementCount = N;
	static constexpr VecSimd kVecSimd = SIMD;

	using SimdData = VecSimd_Data_<N,T,SIMD>; 
	union {
		SimdData	_simd;
		struct { T x, y, z, w; };
		T e[N];
	};

	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
	using Mat4 = Mat4_<T, SIMD>;
	
	AX_INLINE constexpr Quat_() = default;
	AX_INLINE constexpr Quat_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Quat_(Tag::All_, const T& vec) : _simd(SimdData::s_all(vec)) {}
	AX_INLINE constexpr Quat_(const T& x_, const T& y_, const T& z_, const T& w_) : _simd(x_, y_, z_, w_) {}

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); } 

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Quat4_<T, R_SIMD>& vec, T epsilon = Math::epsilon_<T>) const { 
		return _simd.almostEqual(vec._simd, epsilon); 
	}
	
	
	AX_NODISCARD AX_INLINE constexpr bool almostZero(  const This& rhs) const { return _simd.almostZero(rhs._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Quat4_<R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io_fixed_span(fixedSpan()); }
	
	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) { x=x_; y=y_; z=z_; w=w_; }

	AX_NODISCARD AX_INLINE static constexpr This  s_identity()	{ return This(T(0), T(0), T(0), T(1)); }
						AX_INLINE constexpr void setIdentity()	{ *this = s_identity; }

	AX_NODISCARD AX_INLINE static constexpr This  s_angleAxis(T deg, const Vec3& axis) { return s_angleAxis_radians(Math::radians(deg), axis); }
	AX_NODISCARD AX_INLINE static constexpr This  s_angleAxis_radians(T rad, const Vec3& axis);
	
						AX_INLINE constexpr void setAngleAxis(T deg, const Vec3& axis)			{ *this = s_angleAxis_radians(Math::radians(deg), axis);  }
						AX_INLINE constexpr void setAngleAxis_radians(T rad, const Vec3& axis)	{ *this = s_angleAxis(rad, axis); }
	
	AX_NODISCARD AX_INLINE static constexpr This   s_fromDirToDir(const Vec3& from, const Vec3& to);
	AX_NODISCARD AX_INLINE static constexpr This   s_lookAt(const Vec3& direction, const Vec3& up);
	
	AX_NODISCARD constexpr T	angle() const { return Math::acos(w) * T(2); }
	AX_NODISCARD constexpr Vec3	axis () const;

	AX_NODISCARD static	constexpr This s_euler(const Vec3& r)	{ return s_euler_radians(Math::radians(r)); }
	AX_NODISCARD static	constexpr This s_euler(T x, T y, T z)	{ return s_euler(Vec3(x,y,z)); }
	
	AX_NODISCARD static	constexpr This s_euler_radians(const Vec3& r)	{ return s_eulerYXZ_radians(r); }
	AX_NODISCARD static	constexpr This s_euler_radians(T x, T y, T z)	{ return s_euler_radians(Vec3(x,y,z)); }

	AX_NODISCARD static	constexpr This s_eulerXYZ(const Vec3& r)	{ return s_eulerXYZ_radians(Math::radians(r)); }
	AX_NODISCARD static	constexpr This s_eulerXZY(const Vec3& r)	{ return s_eulerXZY_radians(Math::radians(r)); }
	AX_NODISCARD static	constexpr This s_eulerYXZ(const Vec3& r)	{ return s_eulerYXZ_radians(Math::radians(r)); }
	AX_NODISCARD static	constexpr This s_eulerYZX(const Vec3& r)	{ return s_eulerYZX_radians(Math::radians(r)); }
	AX_NODISCARD static	constexpr This s_eulerZXY(const Vec3& r)	{ return s_eulerZXY_radians(Math::radians(r)); }
	AX_NODISCARD static	constexpr This s_eulerZYX(const Vec3& r)	{ return s_eulerZYX_radians(Math::radians(r)); }
	
	AX_NODISCARD static	constexpr This s_eulerXYZ_radians(const Vec3& r);
	AX_NODISCARD static	constexpr This s_eulerXZY_radians(const Vec3& r);
	AX_NODISCARD static	constexpr This s_eulerYXZ_radians(const Vec3& r);
	AX_NODISCARD static	constexpr This s_eulerYZX_radians(const Vec3& r);
	AX_NODISCARD static	constexpr This s_eulerZXY_radians(const Vec3& r);
	AX_NODISCARD static	constexpr This s_eulerZYX_radians(const Vec3& r);
	
	AX_NODISCARD static	constexpr This s_eulerX(T deg)		{ return s_eulerX_radians(Math::radians(deg)); }
	AX_NODISCARD static	constexpr This s_eulerY(T deg)		{ return s_eulerY_radians(Math::radians(deg)); }
	AX_NODISCARD static	constexpr This s_eulerZ(T deg)		{ return s_eulerZ_radians(Math::radians(deg)); }

	AX_NODISCARD static	constexpr This s_eulerX_radians(T rad)	{ T s, c; Math::sincos(rad * T(0.5), s, c); return This(s,0,0,c); }
	AX_NODISCARD static	constexpr This s_eulerY_radians(T rad)	{ T s, c; Math::sincos(rad * T(0.5), s, c); return This(0,s,0,c); }
	AX_NODISCARD static	constexpr This s_eulerZ_radians(T rad)	{ T s, c; Math::sincos(rad * T(0.5), s, c); return This(0,0,s,c); }

	constexpr void  setEuler(const Vec3& r) { *this = s_euler(r); }
	constexpr void  setEuler(T x_, T y_, T z_) { *this = s_euler(Vec3(x_,y_,z_)); }
	constexpr void 	setEulerX(T deg) { *this = s_eulerX(deg); }
	constexpr void 	setEulerY(T deg) { *this = s_eulerY(deg); }
	constexpr void 	setEulerZ(T deg) { *this = s_eulerZ(deg); }

	constexpr void  setEuler_radians(const Vec3& r) { *this = s_euler_radians(r); }
	constexpr void  setEuler_radians(T x_, T y_, T z_) { *this = s_euler_radians(Vec3(x_,y_,z_)); }
	constexpr void 	setEulerX_radians(T rad) { *this = s_eulerX_radians(rad); }
	constexpr void 	setEulerY_radians(T rad) { *this = s_eulerY_radians(rad); }
	constexpr void 	setEulerZ_radians(T rad) { *this = s_eulerZ_radians(rad); }

	constexpr void 	setEulerXYZ(const Vec3& r)	{ *this = s_eulerXYZ(r); }
	constexpr void 	setEulerXZY(const Vec3& r)	{ *this = s_eulerXZY(r); }
	constexpr void 	setEulerYXZ(const Vec3& r)	{ *this = s_eulerYXZ(r); }
	constexpr void 	setEulerYZX(const Vec3& r)	{ *this = s_eulerYZX(r); }
	constexpr void 	setEulerZXY(const Vec3& r)	{ *this = s_eulerZXY(r); }
	constexpr void 	setEulerZYX(const Vec3& r)	{ *this = s_eulerZYX(r); }
		
	constexpr void 	setEulerXYZ_radians(const Vec3& r)	{ *this = s_eulerXYZ_radians(r); }
	constexpr void 	setEulerXZY_radians(const Vec3& r)	{ *this = s_eulerXZY_radians(r); }
	constexpr void 	setEulerYXZ_radians(const Vec3& r)	{ *this = s_eulerYXZ_radians(r); }
	constexpr void 	setEulerYZX_radians(const Vec3& r)	{ *this = s_eulerYZX_radians(r); }
	constexpr void 	setEulerZXY_radians(const Vec3& r)	{ *this = s_eulerZXY_radians(r); }
	constexpr void 	setEulerZYX_radians(const Vec3& r)	{ *this = s_eulerZYX_radians(r); }
	
	AX_NODISCARD constexpr Vec3	euler() const	 { return Math::degrees(euler_radians()); }
	AX_NODISCARD constexpr Vec3	eulerXYZ() const { return Math::degrees(eulerXYZ_radians()); }
	AX_NODISCARD constexpr Vec3	eulerXZY() const { return Math::degrees(eulerXZY_radians()); }
	AX_NODISCARD constexpr Vec3	eulerYXZ() const { return Math::degrees(eulerYXZ_radians()); }
	AX_NODISCARD constexpr Vec3	eulerYZX() const { return Math::degrees(eulerYZX_radians()); }
	AX_NODISCARD constexpr Vec3	eulerZXY() const { return Math::degrees(eulerZXY_radians()); }
	AX_NODISCARD constexpr Vec3	eulerZYX() const { return Math::degrees(eulerZYX_radians()); }
	
	AX_NODISCARD constexpr Vec3	euler_radians() const { return eulerYXZ_radians(); }
	AX_NODISCARD constexpr Vec3	eulerXYZ_radians() const;
	AX_NODISCARD constexpr Vec3	eulerXZY_radians() const;
	AX_NODISCARD constexpr Vec3	eulerYXZ_radians() const;
	AX_NODISCARD constexpr Vec3	eulerYZX_radians() const;
	AX_NODISCARD constexpr Vec3	eulerZXY_radians() const;
	AX_NODISCARD constexpr Vec3	eulerZYX_radians() const;
	
	AX_NODISCARD constexpr This normalize() const;
	AX_NODISCARD constexpr This conjugate() const { return This(-x, -y, -z, w); }
	AX_NODISCARD constexpr This inverse() const;

	AX_NODISCARD constexpr Vec3 dirX() const	{ return operator*(Vec3(1,0,0)); }
	AX_NODISCARD constexpr Vec3 dirY() const	{ return operator*(Vec3(0,1,0)); }
	AX_NODISCARD constexpr Vec3 dirZ() const	{ return operator*(Vec3(0,0,1)); }
	
	AX_NODISCARD constexpr T dot(const This& r) const;

	AX_NODISCARD constexpr This operator* (const This& q) const;
	             constexpr void operator*=(const This& q) { *this = *this * q; }
	
	AX_NODISCARD constexpr Vec3 operator*(const Vec3& v) const;
	
	template<class V> AX_INLINE void castFrom(const V& v) { *this = s_cast(v); }
	
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

template<class T, VecSimd SIMD> inline Vec3_<T> operator* (Vec3_<T,SIMD>& v, const Quat4_<T,SIMD>& quat) { return quat * v; }
template<class T, VecSimd SIMD> inline void     operator*=(Vec3_<T,SIMD>& v, const Quat4_<T,SIMD>& quat) { v = quat * v; }



template<class T, bool LONG_WAY = false> AX_NODISCARD AX_INLINE constexpr
Quat4_<T> slerp(const Quat4_<T> & a, const Quat4_<T> & b, T weight) {
	Quat4_<T> z = b;
	T cosTheta = a.dot(b);

	if constexpr (!LONG_WAY) {
		// If cosTheta < 0, the interpolation will take the long way around the sphere. 
		// To fix this, one quat must be negated.
		if (cosTheta < T(0)) {
			z        = -b;
			cosTheta = -cosTheta;
		}
	}

	// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
	if(cosTheta > T(1) - Math::epsilon_<T>()) {
		// Linear interpolation
		return ax_lerp(a, z, weight);
	} else {
		// Essential Mathematics, page 467
		T angle = acos(cosTheta);
		return (a * sin((T(1) - weight) * angle) + z * sin(weight * angle)) / sin(angle);
	}
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr
Quat4_<T, SIMD> slerp_longway(const Quat4_<T, SIMD> & a, const Quat4_<T, SIMD> & b, T weight) {
	return slerp<T, true>(a, b, weight);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::dot(const This& r) const -> T {
	return _simd.dot(r._simd);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::axis() const -> Vec3 {
	T a = T(1) - w * w;
	if (a <= 0) return Vec3(0, 0, 1);
	return Vec3(x, y, z) * Math::rsqrt_fast(a);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr 
auto Quat_<4, T, SIMD>::s_eulerXYZ_radians(const Vec3& r) -> This {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z + c.x * s.y * s.z,
				c.x * s.y * c.z - s.x * c.y * s.z,
				c.x * c.y * s.z + s.x * s.y * c.z,
				c.x * c.y * c.z - s.x * s.y * s.z); 
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::s_eulerXZY_radians(const Vec3& r) -> This {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z - c.x * s.y * s.z,
				c.x * s.y * c.z - s.x * c.y * s.z,
				c.x * c.y * s.z + s.x * s.y * c.z,
				c.x * c.y * c.z + s.x * s.y * s.z);
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::s_eulerYXZ_radians(const Vec3& r) -> This  {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z + c.x * s.y * s.z,
				c.x * s.y * c.z - s.x * c.y * s.z,
				c.x * c.y * s.z - s.x * s.y * c.z,
				c.x * c.y * c.z + s.x * s.y * s.z);
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::s_eulerYZX_radians(const Vec3& r)  -> This {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z + c.x * s.y * s.z,
				c.x * s.y * c.z + s.x * c.y * s.z,
				c.x * c.y * s.z - s.x * s.y * c.z,
				c.x * c.y * c.z - s.x * s.y * s.z);
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::s_eulerZXY_radians(const Vec3& r)  -> This {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z - c.x * s.y * s.z,
				c.x * s.y * c.z + s.x * c.y * s.z,
				c.x * c.y * s.z + s.x * s.y * c.z,
				c.x * c.y * c.z - s.x * s.y * s.z);
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::s_eulerZYX_radians(const Vec3& r)  -> This {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z - c.x * s.y * s.z,
				c.x * s.y * c.z + s.x * c.y * s.z,
				c.x * c.y * s.z - s.x * s.y * c.z,
				c.x * c.y * c.z + s.x * s.y * s.z);
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::eulerXYZ_radians() const -> Vec3 {
	T x2 = x * x;
	T y2 = y * y;
	T z2 = z * z;
	T w2 = w * w;
	T d = T(2) * (w * y + z * x) / (w2 + x2 + y2 + z2);
	Vec3 o;
	o.x = Math::atan2(T(2) * (w * x - y * z), w2 - x2 - y2 + z2);
	o.y = Math::asin(Math::clamp(d, T(-1), T(1)));
	o.z = Math::atan2(T(2) * (w * z - x * y), w2 + x2 - y2 - z2);
	return o;
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::eulerXZY_radians() const -> Vec3 {
	T x2 = x * x;
	T y2 = y * y;
	T z2 = z * z;
	T w2 = w * w;
	T d = T(2) * (w * z - x * y) / (w2 + x2 + y2 + z2);
	Vec3 o;
	o.x = Math::atan2(T(2) * (w * x + y * z) ,  w2 - x2 + y2 - z2);
	o.y = Math::atan2(T(2) * (w * y + x * z) ,  w2 + x2 - y2 - z2);
	o.z = Math::asin(Math::clamp(d, T(-1), T(1)));
	return o;
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::eulerYXZ_radians() const -> Vec3 {
	T x2 = x * x;
	T y2 = y * y;
	T z2 = z * z;
	T w2 = w * w;
	T d = T(2) * (w * x - y * z) / (w2 + x2 + y2 + z2);
	Vec3 o;
	o.x = Math::asin(Math::clamp(d, T(-1), T(1)));
	o.y = Math::atan2(T(2) * (w * y + x * z), w2 - x2 - y2 + z2);
	o.z = Math::atan2(T(2) * (w * z + x * y), w2 - x2 + y2 - z2);
	return o;
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::eulerYZX_radians() const -> Vec3 {
	T x2 = x * x;
	T y2 = y * y;
	T z2 = z * z;
	T w2 = w * w;
	T d = T(2) * (w * z + x * y) / (x2 + y2 + z2 + w2);
	Vec3 o;
	o.x = Math::atan2(T(2) * (w * x - y * z), w2 - x2 + y2 - z2);
	o.y = Math::atan2(T(2) * (w * y - x * z), w2 + x2 - y2 - z2);
	o.z = Math::asin(std::clamp(d, T(-1), T(1)));
	return o;
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::eulerZXY_radians() const -> Vec3 {
	T x2 = x * x;
	T y2 = y * y;
	T z2 = z * z;
	T w2 = w * w;
	T d = T(2) * (w * x + y * z) / (w2 + x2 + y2 + z2);
	Vec3 o;
	o.x = Math::asin(Math::clamp(d, T(-1), T(1)));
	o.y = Math::atan2(T(2) * (w * y - x * z), w2 - x2 - y2 + z2);
	o.z = Math::atan2(T(2) * (w * z - x * y), w2 - x2 + y2 - z2);
	return o;
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::eulerZYX_radians() const -> Vec3 {
	T x2 = x * x;
	T y2 = y * y;
	T z2 = z * z;
	T w2 = w * w;
	T d = T(-2) * (x * z - w * y) / (x2 + y2 + z2 + w2);
	Vec3 o;
	o.x = Math::atan2(T(2) * (w * x + y * z), w2 - x2 - y2 + z2);
	o.y = Math::asin(Math::clamp(d, T(-1), T(1)));
	o.z = Math::atan2(T(2) * (w * z + x * y), w2 + x2 - y2 - z2);
	return o;
}

template<class T, VecSimd SIMD> constexpr 
auto Quat_<4, T, SIMD>::normalize() const -> This {
	T magSq = _simd.dot(_simd);
	if (Math::almostZero(magSq)) {
		return *this;
	} else {
		T invMag = Math::rsqrt_fast(magSq);
		return _simd * invMag;
	}
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::inverse() const -> This {
	return conjugate()._simd / _simd.dot(_simd);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::operator*(const Vec3_<T, SIMD>& v) const -> Vec3 {
	Vec3 qv(x, y, z);
	auto uv  = qv.cross(v);
	auto uuv = qv.cross(uv);
	return v + (uv * w + uuv) * T(2);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::operator*(const This& q) const -> This {
	return This(w * q.x + x * q.w + y * q.z - z * q.y,
				w * q.y + y * q.w + z * q.x - x * q.z,
				w * q.z + z * q.w + x * q.y - y * q.x,
				w * q.w - x * q.x - y * q.y - z * q.z);
}

template<class T, VecSimd SIMD> constexpr AX_INLINE auto Quat_<4, T, SIMD>::s_angleAxis_radians(T rad, const Vec3& axis) -> This {
	T s, c;
	Math::sincos(rad * T(0.5), s, c);
	return This(axis.x * s, axis.y * s, axis.z * s, c);
}

template<class T, VecSimd SIMD>
constexpr typename Quat_<4, T, SIMD>::This Quat_<4, T, SIMD>::s_fromDirToDir(const Vec3& from, const Vec3& to) {
	float k_cos_theta = from.dot(to);
	float k = Math::sqrt(from.lengthSq() * to.lengthSq());

	if (k_cos_theta / k == -1) {
		Vec3 orthogonal = Orthogonal(from); 
		return This(0, orthogonal.x, orthogonal.y, orthogonal.z).normalize();
	}

	Vec3 axis = from.cross(to);
	return This(k_cos_theta + k, axis.x, axis.y, axis.z).normalize();
}

template<class T, VecSimd SIMD>
constexpr auto Quat_<4, T, SIMD>::s_lookAt(const Vec3& direction, const Vec3& up) -> This {
	auto forward = direction.normalize();
	auto right   = up.cross(direction).normalize();
	auto newUp   = direction.cross(right);
	return Mat4::s_direction(right, forward, newUp).toQuat();
}

} // namespace

