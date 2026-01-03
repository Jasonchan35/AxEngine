module;

export module AxCore.Quat; // Quaternion
export import AxCore.Mat;

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
	};

	using Vec2 = Vec2_<T, SIMD>;
	using Vec3 = Vec3_<T, SIMD>;
	using Vec4 = Vec4_<T, SIMD>;
	
	AX_INLINE constexpr Quat_() = default;
	AX_INLINE constexpr Quat_(const SimdData & simd) : _simd(simd) {}
//	AX_INLINE constexpr Quat_(Tag::All_, const T& vec) : _simd(SimdData::s_all(vec)) {}
	AX_INLINE constexpr Quat_(const T& x_, const T& y_, const T& z_, const T& w_) : _simd(x_, y_, z_, w_) {}

	AX_NODISCARD AX_INLINE constexpr static This s_all (const T& t) { return SimdData::s_all(t); } 
	AX_NODISCARD AX_INLINE constexpr static This s_zero() { return SimdData::s_zero(); } 
	AX_NODISCARD AX_INLINE constexpr static This s_one () { return SimdData::s_one(); } 

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool almostEqual(const Vec_<N, T, R_SIMD>& vec) const { return _simd.almostEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool almostZero(  const This& rhs) const { return _simd.almostZero(rhs._simd); }
	AX_NODISCARD AX_INLINE constexpr bool exactlyEqual(const This& vec) const { return _simd.exactlyEqual(vec._simd); }
	AX_NODISCARD AX_INLINE constexpr bool operator==(  const This& vec) const { return _simd == vec._simd; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static This s_cast(const Vec_<N, R, R_SIMD>& vec) { return SimdData::s_cast(vec._simd); }
	template<class CH> constexpr void onFormat(Format_<CH> & fmt) const { return _simd.onFormat(fmt); }
	
	AX_INLINE constexpr void set(const T& x_, const T& y_, const T& z_, const T& w_) { x=x_; y=y_; z=z_; w=w_; }

	AX_NODISCARD AX_INLINE static constexpr This   s_identity ()	{ return This(T(0), T(0), T(0), T(1)); }
						AX_INLINE constexpr void set_identity() { *this = s_identity; }

	AX_NODISCARD AX_INLINE static constexpr This   s_angleAxis(T rad, const Vec3& axis);
						AX_INLINE constexpr void set_angleAxis(T rad, const Vec3& axis) { *this = s_angleAxis(rad, axis); }

	AX_NODISCARD constexpr T	angle() const { return Math::acos(w) * T(2); }
	AX_NODISCARD constexpr Vec3	axis () const;

	AX_NODISCARD static	constexpr This s_euler(const Vec3& r);
	AX_NODISCARD static	constexpr This s_euler_x(T rad)	{ T s, c; sincos(rad * T(0.5), s, c); return This(s,0,0,c); }
	AX_NODISCARD static	constexpr This s_euler_y(T rad)	{ T s, c; sincos(rad * T(0.5), s, c); return This(0,s,0,c); }
	AX_NODISCARD static	constexpr This s_euler_z(T rad)	{ T s, c; sincos(rad * T(0.5), s, c); return This(0,0,s,c); }

	AX_NODISCARD static	constexpr This s_euler_deg_x(T deg)	{ return s_euler_x(radians(deg)); }
	AX_NODISCARD static	constexpr This s_euler_deg_y(T deg)	{ return s_euler_y(radians(deg)); }
	AX_NODISCARD static	constexpr This s_euler_deg_z(T deg)	{ return s_euler_z(radians(deg)); }

			constexpr void 	set_euler(const Vec3& r)	{ *this = s_euler(r); }
			constexpr void 	set_euler_x(T rad) { *this = s_euler_x(rad); }
			constexpr void 	set_euler_y(T rad) { *this = s_euler_y(rad); }
			constexpr void 	set_euler_z(T rad) { *this = s_euler_z(rad); }
			constexpr void 	set_euler_deg_x(T deg) { *this = s_euler_deg_x(deg); }
			constexpr void 	set_euler_deg_y(T deg) { *this = s_euler_deg_y(deg); }
			constexpr void 	set_euler_deg_z(T deg) { *this = s_euler_deg_z(deg); }

	AX_NODISCARD constexpr Vec3	euler() const	{ return Vec3(euler_x(), euler_y(), euler_z()); }
	AX_NODISCARD constexpr T	euler_x() const;
	AX_NODISCARD constexpr T	euler_y() const;
	AX_NODISCARD constexpr T	euler_z() const;

	AX_NODISCARD constexpr This conjugate() const { return This(-x, -y, -z, w); }
	AX_NODISCARD constexpr This inverse() const;

	AX_NODISCARD constexpr T dot(const This& r) const;
	
	AX_NODISCARD constexpr This operator*(const This& q) const;
	AX_NODISCARD constexpr Vec3 operator*(const Vec3& v) const;
	
	template<class V> AX_INLINE void fromCast(const V& v) { *this = s_cast(v); }	
};

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
	return (x * r.x + y * r.y) + (z * r.z + w * r.w);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::axis() const -> Vec3 {
	T a = T(1) - w * w;
	if (a <= 0) return Vec3(0, 0, 1);
	return Vec3(x, y, z) * Math::rsqrt_fast(a);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::s_euler(const Vec3& r) -> This {
	Vec3_<T, SIMD> s, c;
	Math::sincos(r * T(0.5), s, c);
	return This(s.x * c.y * c.z - c.x * s.y * s.z,
	            c.x * s.y * c.z + s.x * c.y * s.z,
	            c.x * c.y * s.z - s.x * s.y * c.z,
	            c.x * c.y * c.z + s.x * s.y * s.z);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::euler_z() const -> T {
	auto a = T(2) * (x * y + w * z);
	auto b = w * w + x * x - y * y - z * z;
	return Math::atan2(a, b);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::euler_y() const -> T {
	auto v = T(-2) * (x * z - w * y);
	auto a = Math::clamp(v, T(-1), T(1));
	return Math::asin(a);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::euler_x() const -> T {
	auto a = T(2) * (y * z + w * x);
	auto b = w * w - x * x - y * y + z * z;
	return Math::atan2(a, b);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::inverse() const -> This {
	T d = x * x + y * y + z * z + w * w;
	return This(-x, -y, -z, w) / d;
}

// operator * (Vec3, Quat4)
template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr
Vec_<3, T, SIMD> operator*(const Vec_<3, T, SIMD> & vec, const Quat4_<T, SIMD>& q) {
	return q.inverse() * vec;
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::operator*(const Vec3_<T, SIMD>& v) const -> Vec3 {
	Vec3 qv(x, y, z);
	auto uv  = qv.cross(v);
	auto uuv = qv.cross(uv);
	return v + (uv * w + uuv) * T(2);
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr auto Quat_<4, T, SIMD>::operator*(const This& q) const -> This {
	return This(x * q.w +  w * q.x + z * q.y - y * q.z,
				y * q.w +  w * q.y + x * q.z - z * q.x,
				z * q.w +  w * q.z + y * q.x - x * q.y,
				w * q.w -  x * q.x - y * q.y - z * q.z);
}

template<class T, VecSimd SIMD> constexpr AX_INLINE auto Quat_<4, T, SIMD>::s_angleAxis(T rad, const Vec3& axis) -> This {
	T s, c;
	sincos(rad * T(0.5), s, c);
	return This(axis.x * s, axis.y * s, axis.z * s, c);
}

} // namespace

