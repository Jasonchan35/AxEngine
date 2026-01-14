module;

export module AxCore.Mat; // Matrix
export import AxCore.Vec;
export import AxCore.BBox;
export import AxCore.Rect;

export namespace ax {

template<Int N, Int M, class T, VecSimd SIMD> class Mat_;

template <class T, VecSimd SIMD = VecSimd_Default> using Mat3_		= Mat_<3, 3, T, SIMD>;
template <class T, VecSimd SIMD = VecSimd_Default> using Mat4_		= Mat_<4, 4, T, SIMD>;
template <class T, VecSimd SIMD = VecSimd_Default> using Mat4x3_	= Mat_<4, 3, T, SIMD>; 

using Mat4f			= Mat4_<f32>;
using Mat4f_SSE		= Mat4_<f32, VecSimd::SSE>;
using Mat4f_Basic	= Mat4_<f32, VecSimd::Basic>;
using Mat4d			= Mat4_<f64>;
using Mat4d_SSE		= Mat4_<f64, VecSimd::SSE>;
using Mat4d_Basic	= Mat4_<f64, VecSimd::Basic>;

// column major matrix
template<class T, VecSimd SIMD>
class Mat_<4,4,T,SIMD> {
	using This = Mat_;
	
	static constexpr bool _use_SSE			= SIMD == VecSimd::SSE;
	static constexpr bool _is_f32			= Type_IsSame<T, f32>;
	static constexpr bool _is_f64			= Type_IsSame<T, f64>;
	static constexpr bool _use_SSE_m128_ps	= _use_SSE && _is_f32;
	static constexpr bool _use_SSE_m256_pd	= _use_SSE && _is_f64;
public:
	using Vec1		= Vec1_< T, SIMD>;
	using Vec2		= Vec2_< T, SIMD>;
	using Vec3		= Vec3_< T, SIMD>;
	using Vec4		= Vec4_< T, SIMD>;
	using Mat4		= Mat4_< T, SIMD>;
	using Rect2		= Rect2_<T, SIMD>;

	static constexpr Int kRowCount = 4;
	static constexpr Int kColCount = 4;
	static constexpr Int kElementCount = 16;

	Vec4 cx, cy, cz, cw;
	
	Mat_() = default;
	AX_NODISCARD AX_INLINE constexpr Mat_(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_)
		: cx(cx_)
		, cy(cy_)
		, cz(cz_)
		, cw(cw_) {
	}
	AX_NODISCARD AX_INLINE constexpr Mat_(	const T& xx, const T& xy, const T& xz, const T& xw,
											const T& yx, const T& yy, const T& yz, const T& yw,
											const T& zx, const T& zy, const T& zz, const T& zw,
											const T& wx, const T& wy, const T& wz, const T& ww )
		: cx(xx, xy, xz, xw)
		, cy(yx, yy, yz, yw)
		, cz(zx, zy, zz, zw)
		, cw(wx, wy, wz, ww)
	{}
	
	template<VecSimd R_SIMD>
	AX_NODISCARD constexpr Mat_(const Mat_<4,4,T,R_SIMD>& r) : Mat_(r.cx, r.cy, r.cz, r.cw) {}

	template<class CH>
	void onFormat(Format_<CH> & fmt) const {
		fmt << cx; fmt.newline();
		fmt << cy; fmt.newline();
		fmt << cz; fmt.newline();
		fmt << cw; fmt.newline();
	}

	AX_INLINE constexpr void set(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_) {
		cx = cx_;
		cy = cy_;
		cz = cz_;
		cw = cw_;
	}
	
	AX_INLINE constexpr void set( 
			const T& xx, const T& xy, const T& xz, const T& xw,
			const T& yx, const T& yy, const T& yz, const T& yw,
			const T& zx, const T& zy, const T& zz, const T& zw,
			const T& wx, const T& wy, const T& wz, const T& ww )
	{
		cx.set(xx, xy, xz, xw);
		cy.set(yx, yy, yz, yw);
		cz.set(zx, zy, zz, zw);
		cw.set(wx, wy, wz, ww);
	}
			T* data()		{ return cx.data(); }
	const	T* data() const { return cx.data(); }
	
	AX_INLINE static constexpr This s_zero()		{ return This(Vec4::s_zero(), Vec4::s_zero(), Vec4::s_zero(), Vec4::s_zero()); }
	AX_INLINE static constexpr This s_identity()	{ return This(	1,0,0,0,
																	0,1,0,0,
																	0,0,1,0,
																	0,0,0,1); }

	constexpr void set_identity()	{ *this = s_identity(); }

	AX_NODISCARD constexpr This transpose() const;

	constexpr void setCol(Int i, const Vec4& v) { (&cx)[i] = v; }
	
	AX_NODISCARD AX_INLINE constexpr Vec4 col(Int i) const {
		AX_ASSERT(i >= 0 || i < kColCount);
		return (&cx)[i];
	}
	AX_NODISCARD AX_INLINE constexpr Vec4 row(Int i) const {
		AX_ASSERT(i >= 0 || i < kRowCount);
		return Vec4(cx.data()[i], cy.data()[i], cz.data()[i], cw.data()[i]);
	}

	AX_NODISCARD AX_INLINE constexpr bool operator==(const This& r) const	{ return cx == r.cx && cy == r.cy && cz == r.cz && cw == r.cw; }
	AX_NODISCARD AX_INLINE constexpr bool operator!=(const This& r) const	{ return cx != r.cx || cy != r.cy || cz != r.cz || cw != r.cw; }

	AX_NODISCARD constexpr static	This s_perspective		(T fovy_rad, T aspect, T zNear, T zFar);
	AX_NODISCARD constexpr static	This s_ortho			(T left, T right, T bottom, T top, T zNear, T zFar);
	AX_NODISCARD constexpr static	This s_lookAt			(const Vec3 & eye, const Vec3 & aim, const Vec3 & up);
	
	AX_NODISCARD constexpr static	This s_translate		(const Vec3& v);
	AX_NODISCARD constexpr static	This s_translate		(const Vec2& v)			{ return s_translate(Vec3(v, 0)); }

	AX_NODISCARD constexpr static	This s_rotateRad		(const Vec3 & v);
	AX_NODISCARD constexpr static	This s_rotateRadX		(T rad);
	AX_NODISCARD constexpr static	This s_rotateRadY		(T rad);
	AX_NODISCARD constexpr static	This s_rotateRadZ		(T rad);

	AX_NODISCARD constexpr static	This s_rotateDeg		(const Vec3 & v)		{ return s_rotateRad( radians(v)); }
	AX_NODISCARD constexpr static	This s_rotateDegX		(T deg)					{ return s_rotateRadX(radians(deg)); }
	AX_NODISCARD constexpr static	This s_rotateDegY		(T deg)					{ return s_rotateRadY(radians(deg)); }
	AX_NODISCARD constexpr static	This s_rotateDegZ		(T deg)					{ return s_rotateRadZ(radians(deg)); }

	AX_NODISCARD constexpr static	This s_scale			(T s)					{ return s_scale({s,s,s}); }
	AX_NODISCARD constexpr static	This s_scale			(const Vec2 & v)		{ return s_scale(Vec3(v, 1)); }
	AX_NODISCARD constexpr static	This s_scale			(const Vec3 & v);

	AX_NODISCARD constexpr static	This s_shear			(const Vec3 & v);

	AX_NODISCARD constexpr static	This s_TRS_rad			(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale);
	AX_NODISCARD constexpr static	This s_TRS_deg			(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale) {
		return s_TRS_rad(translate, Math::radians(rotate), scale);
	}

	AX_NODISCARD constexpr static	This s_translateScale	(const Vec3 & translate, const Vec3 & scale);
	AX_NODISCARD constexpr static	This s_translateScale	(const Vec3 & translate, const T &    scale) { return s_translateScale(translate, Vec3(scale)); }

	AX_NODISCARD constexpr This inverse				() const;
	AX_NODISCARD constexpr This inverse3x3			() const;
	AX_NODISCARD constexpr This inverse3x3Transpose	() const;

	AX_NODISCARD constexpr Vec3 unprojectPointFromInverse	(const Vec3& screenPos, const Rect2& viewport) const;
	AX_NODISCARD constexpr Vec3 unprojectPointSlow			(const Vec3& screenPos, const Rect2& viewport) const {
		return inverse().unprojectPointFromInverse(screenPos, viewport);
	}

			//bool operator==			(const This &r) const	{ return cx == r.cx && cy == r.cy && cw == r.cw && cz == r.cz; }
			//bool operator!=			(const This &r) const	{ return cx != r.cx || cy != r.cy || cw != r.cw || cz != r.cz; }

	AX_NODISCARD constexpr bool almostEqual	(const This &r) const;

	AX_NODISCARD constexpr This operator+	(const T& s) const { return This(cx + s, cy + s, cz + s, cw + s); }
	AX_NODISCARD constexpr This operator-	(const T& s) const { return This(cx - s, cy - s, cz - s, cw - s); }
	AX_NODISCARD constexpr This operator*	(const T& s) const { return This(cx * s, cy * s, cz * s, cw * s); }
	AX_NODISCARD constexpr This operator/	(const T& s) const { return This(cx / s, cy / s, cz / s, cw / s); }
	AX_NODISCARD constexpr This operator*	(const This& m) const { return mulMatrix(m); }

				 constexpr void operator+=	(const T& s) { cx += s; cy += s; cz += s; cw += s; }
				 constexpr void operator-=	(const T& s) { cx -= s; cy -= s; cz -= s; cw -= s; }
				 constexpr void operator*=	(const T& s) { cx *= s; cy *= s; cz *= s; cw *= s; }
				 constexpr void operator/=	(const T& s) { cx /= s; cy /= s; cz /= s; cw /= s; }
				 constexpr void operator*=	(const This& m) { *this = mulMatrix(m); }
	
	AX_NODISCARD constexpr This mulMatrix	(const This& m) const;
	AX_NODISCARD constexpr Vec4 mulPoint	(const Vec4& v) const;
	AX_NODISCARD constexpr Vec3 mulPoint	(const Vec3& v) const;

	AX_NODISCARD constexpr Vec3 mulPoint4x3	(const Vec3& v) const;
	AX_NODISCARD constexpr Vec3 mulVector	(const Vec3& v) const;
	AX_NODISCARD constexpr Vec3 mulNormal	(const Vec3& v) const;

	AX_NODISCARD constexpr Vec3 dirX() const { return Vec3(cx.x, cy.x, cz.x).normalize(); }
	AX_NODISCARD constexpr Vec3 dirY() const { return Vec3(cx.y, cy.y, cz.y).normalize(); }
	AX_NODISCARD constexpr Vec3 dirZ() const { return Vec3(cx.z, cy.z, cz.z).normalize(); }
	AX_NODISCARD constexpr Vec3 position() const { return cw.xyz(); }
};

template<class T, VecSimd SIMD> constexpr
bool Mat_<4,4,T,SIMD>::almostEqual(const This &r) const {
	return cx.almostEqual(r.cx)
		&& cy.almostEqual(r.cy)
		&& cw.almostEqual(r.cw)
		&& cz.almostEqual(r.cz);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::mulPoint(const Vec4& v) const -> Vec4 {
	if constexpr (_use_SSE_m128_ps) {
		__m128 res = _mm_mul_ps(  cx._simd.mm, _mm_set1_ps(v.x));
		       res = _mm_fmadd_ps(cy._simd.mm, _mm_set1_ps(v.y), res);
		       res = _mm_fmadd_ps(cz._simd.mm, _mm_set1_ps(v.z), res);
		       res = _mm_fmadd_ps(cw._simd.mm, _mm_set1_ps(v.w), res);
		return Vec4(res);
	} else if constexpr (_use_SSE_m256_pd) {
		__m256d res = _mm256_mul_pd(  cx._simd.mm, _mm256_set1_pd(v.x));
		        res = _mm256_fmadd_pd(cy._simd.mm, _mm256_set1_pd(v.y), res);
		        res = _mm256_fmadd_pd(cz._simd.mm, _mm256_set1_pd(v.z), res);
		        res = _mm256_fmadd_pd(cw._simd.mm, _mm256_set1_pd(v.w), res);
		return Vec4(res);
	}

	return Vec4(cx * v.x 
			  + cy * v.y 
			  + cz * v.z 
			  + cw * v.w);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::mulPoint(const Vec3& v) const -> Vec3 {
	return mulPoint(Vec4(v, 1)).toVec3();
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr
auto Mat_<4, 4, T, SIMD>::mulPoint4x3(const Vec3& v) const -> Vec3 {
	return mulPoint(Vec4(v, 1)).xyz();
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::mulVector(const Vec3& v) const -> Vec3 {
	return Vec3(cx.xyz_direct() * v.x 
			  + cy.xyz_direct() * v.y 
			  + cz.xyz_direct() * v.z);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::mulNormal(const Vec3& v) const -> Vec3 {
	return inverse3x3Transpose().mulVector(v);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::transpose() const -> This {
	return This(
		cx.x, cy.x, cz.x, cw.x,
		cx.y, cy.y, cz.y, cw.y,
		cx.z, cy.z, cz.z, cw.z,
		cx.w, cy.w, cz.w, cw.w
	);
}


template<class T, VecSimd SIMD> constexpr 
auto Mat_<4, 4, T, SIMD>::mulMatrix(const This& m) const -> This {
	if constexpr (_use_SSE_m128_ps) {
		This o;
		for (int i = 0; i < 4; i++) {
			auto c = m.col(i);
			__m128 res = _mm_mul_ps(  cx._simd.mm, _mm_set1_ps(c.x));
			       res = _mm_fmadd_ps(cy._simd.mm, _mm_set1_ps(c.y), res);
			       res = _mm_fmadd_ps(cz._simd.mm, _mm_set1_ps(c.z), res);
			       res = _mm_fmadd_ps(cw._simd.mm, _mm_set1_ps(c.w), res);
			o.setCol(i, Vec4(res));
		}
		return o;
	} else if constexpr (_use_SSE_m256_pd) {
		This o;
		for (int i = 0; i < 4; ++i) {
			auto c = m.col(i);
			__m256d res = _mm256_mul_pd(  cx._simd.mm, _mm256_set1_pd(c.x));
			        res = _mm256_fmadd_pd(cy._simd.mm, _mm256_set1_pd(c.y), res);
			        res = _mm256_fmadd_pd(cz._simd.mm, _mm256_set1_pd(c.z), res);
			        res = _mm256_fmadd_pd(cw._simd.mm, _mm256_set1_pd(c.w), res);
			o.setCol(i, Vec4(res));
		}
		return o;
	}
	
	This o;
	T e0, e1, e2, e3;

	e0 = cx.x, e1 = cy.x, e2 = cz.x, e3 = cw.x;
	o.cx.x = e0*m.cx.x + e1*m.cx.y + e2*m.cx.z + e3*m.cx.w;
	o.cy.x = e0*m.cy.x + e1*m.cy.y + e2*m.cy.z + e3*m.cy.w;
	o.cz.x = e0*m.cz.x + e1*m.cz.y + e2*m.cz.z + e3*m.cz.w;
	o.cw.x = e0*m.cw.x + e1*m.cw.y + e2*m.cw.z + e3*m.cw.w;

	e0 = cx.y, e1 = cy.y, e2 = cz.y, e3 = cw.y;
	o.cx.y = e0*m.cx.x + e1*m.cx.y + e2*m.cx.z + e3*m.cx.w;
	o.cy.y = e0*m.cy.x + e1*m.cy.y + e2*m.cy.z + e3*m.cy.w;
	o.cz.y = e0*m.cz.x + e1*m.cz.y + e2*m.cz.z + e3*m.cz.w;
	o.cw.y = e0*m.cw.x + e1*m.cw.y + e2*m.cw.z + e3*m.cw.w;

	e0 = cx.z, e1 = cy.z, e2 = cz.z, e3 = cw.z;
	o.cx.z = e0*m.cx.x + e1*m.cx.y + e2*m.cx.z + e3*m.cx.w;
	o.cy.z = e0*m.cy.x + e1*m.cy.y + e2*m.cy.z + e3*m.cy.w;
	o.cz.z = e0*m.cz.x + e1*m.cz.y + e2*m.cz.z + e3*m.cz.w;
	o.cw.z = e0*m.cw.x + e1*m.cw.y + e2*m.cw.z + e3*m.cw.w;

	e0 = cx.w, e1 = cy.w, e2 = cz.w, e3 = cw.w;
	o.cx.w = e0*m.cx.x + e1*m.cx.y + e2*m.cx.z + e3*m.cx.w;
	o.cy.w = e0*m.cy.x + e1*m.cy.y + e2*m.cy.z + e3*m.cy.w;
	o.cz.w = e0*m.cz.x + e1*m.cz.y + e2*m.cz.z + e3*m.cz.w;
	o.cw.w = e0*m.cw.x + e1*m.cw.y + e2*m.cw.z + e3*m.cw.w;

	return o;
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::inverse() const -> This {
#if 0
	T wtmp[4][8];
	T m0, m1, m2, m3, s;
	T *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = cx.x;
	r0[1] = cx.y;
	r0[2] = cx.z;
	r0[3] = cx.w;
	r0[4] = T(1);
	r0[5] = r0[6] = r0[7] = T(0);

	r1[0] = cy.x;
	r1[1] = cy.y;
	r1[2] = cy.z;
	r1[3] = cy.w;
	r1[5] = T(1);
	r1[4] = r1[6] = r1[7] = T(0);

	r2[0] = cz.x;
	r2[1] = cz.y;
	r2[2] = cz.z;
	r2[3] = cz.w;
	r2[6] = T(1);
	r2[4] = r2[5] = r2[7] = T(0);

	r3[0] = cw.x;
	r3[1] = cw.y;
	r3[2] = cw.z;
	r3[3] = cw.w;
	r3[7] = T(1);
	r3[4] = r3[5] = r3[6] = T(0);

	// choose pivot - or die
	if (ax_abs(r3[0]) > ax_abs(r2[0])) ax_swap(r3, r2);
	if (ax_abs(r2[0]) > ax_abs(r1[0])) ax_swap(r2, r1);
	if (ax_abs(r1[0]) > ax_abs(r0[0])) ax_swap(r1, r0);
	if (T(0) == r0[0]) return makeIdentity();

	// eliminate first variable
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];

	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4]; if (s != T(0)) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5]; if (s != T(0)) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6]; if (s != T(0)) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7]; if (s != T(0)) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	// choose pivot - or die
	if (ax_abs(r3[1]) > ax_abs(r2[1])) ax_swap(r3, r2);
	if (ax_abs(r2[1]) > ax_abs(r1[1])) ax_swap(r2, r1);
	if (T(0) == r1[1]) {
		AX_ASSERT(false);
		return makeIdentity();
	}

	// eliminate second variable
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];

	s = r1[4]; if (T(0) != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (T(0) != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (T(0) != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (T(0) != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	// choose pivot - or die
	if (ax_abs(r3[2]) > ax_abs(r2[2])) ax_swap(r3, r2);
	if (T(0) == r2[2]) {
		AX_ASSERT(false);
		return makeIdentity();
	}

	// eliminate third variable
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3];
	r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5];
	r3[6] -= m3 * r2[6],
	r3[7] -= m3 * r2[7];

	// last check
	if (T(0) == r3[3]) {
		AX_ASSERT(false);
		return makeIdentity();
	}

	s = T(1) / r3[3];    // now back substitute row 3
	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;

	m2 = r2[3];          // now back substitute row 2
	s = T(1) / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2);
	r2[5] = s * (r2[5] - r3[5] * m2);
	r2[6] = s * (r2[6] - r3[6] * m2);
	r2[7] = s * (r2[7] - r3[7] * m2);

	m1 = r1[3];
	r1[4] -= r3[4] * m1;
	r1[5] -= r3[5] * m1;
	r1[6] -= r3[6] * m1;
	r1[7] -= r3[7] * m1;

	m0 = r0[3];
	r0[4] -= r3[4] * m0;
	r0[5] -= r3[5] * m0;
	r0[6] -= r3[6] * m0;
	r0[7] -= r3[7] * m0;

	m1 = r1[2];                 // now back substitute row 1
	s = T(1) / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1);
	r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1);
	r1[7] = s * (r1[7] - r2[7] * m1);

	m0 = r0[2];
	r0[4] -= r2[4] * m0;
	r0[5] -= r2[5] * m0;
	r0[6] -= r2[6] * m0;
	r0[7] -= r2[7] * m0;

	m0 = r0[1];                 // now back substitute row 0
	s = T(1) / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0);
	r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0);
	r0[7] = s * (r0[7] - r1[7] * m0);

	return This(r0[4], r0[5], r0[6], r0[7],
				r1[4], r1[5], r1[6], r1[7],
				r2[4], r2[5], r2[6], r2[7],
				r3[4], r3[5], r3[6], r3[7]);
#else
	T coef00 = cz.z * cw.w - cw.z * cz.w;
	T coef02 = cy.z * cw.w - cw.z * cy.w;
	T coef03 = cy.z * cz.w - cz.z * cy.w;

	T coef04 = cz.y * cw.w - cw.y * cz.w;
	T coef06 = cy.y * cw.w - cw.y * cy.w;
	T coef07 = cy.y * cz.w - cz.y * cy.w;

	T coef08 = cz.y * cw.z - cw.y * cz.z;
	T coef10 = cy.y * cw.z - cw.y * cy.z;
	T coef11 = cy.y * cz.z - cz.y * cy.z;

	T coef12 = cz.x * cw.w - cw.x * cz.w;
	T coef14 = cy.x * cw.w - cw.x * cy.w;
	T coef15 = cy.x * cz.w - cz.x * cy.w;

	T coef16 = cz.x * cw.z - cw.x * cz.z;
	T coef18 = cy.x * cw.z - cw.x * cy.z;
	T coef19 = cy.x * cz.z - cz.x * cy.z;

	T coef20 = cz.x * cw.y - cw.x * cz.y;
	T coef22 = cy.x * cw.y - cw.x * cy.y;
	T coef23 = cy.x * cz.y - cz.x * cy.y;

	Vec4 fac0(coef00, coef00, coef02, coef03);
	Vec4 fac1(coef04, coef04, coef06, coef07);
	Vec4 fac2(coef08, coef08, coef10, coef11);
	Vec4 fac3(coef12, coef12, coef14, coef15);
	Vec4 fac4(coef16, coef16, coef18, coef19);
	Vec4 fac5(coef20, coef20, coef22, coef23);

	Vec4 v0(cy.x, cx.x, cx.x, cx.x);
	Vec4 v1(cy.y, cx.y, cx.y, cx.y);
	Vec4 v2(cy.z, cx.z, cx.z, cx.z);
	Vec4 v3(cy.w, cx.w, cx.w, cx.w);

	Vec4 signA(+1, -1, +1, -1);
	Vec4 signB(-1, +1, -1, +1);

	Vec4 inv0(v1 * fac0 - v2 * fac1 + v3 * fac2);
	Vec4 inv1(v0 * fac0 - v2 * fac3 + v3 * fac4);
	Vec4 inv2(v0 * fac1 - v1 * fac3 + v3 * fac5);
	Vec4 inv3(v0 * fac2 - v1 * fac4 + v2 * fac5);

	Mat4 inv(inv0 * signA,
			 inv1 * signB,
			 inv2 * signA,
			 inv3 * signB);

	Vec4 dot0(cx * inv.row(0));
	T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);
	T oneOverDeterminant = T(1) / dot1;

	return inv * oneOverDeterminant;
#endif
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::inverse3x3() const -> This {
	T oneOverDeterminant = T(1) / (
		+ cx.x * (cy.y * cz.z - cz.y * cy.z)
		- cy.x * (cx.y * cz.z - cz.y * cx.z)
		+ cz.x * (cx.y * cy.z - cy.y * cx.z));

	return This(
		(cy.y * cz.z - cz.y * cy.z) *  oneOverDeterminant, // xx
		(cx.y * cz.z - cz.y * cx.z) * -oneOverDeterminant, // xy
		(cx.y * cy.z - cy.y * cx.z) *  oneOverDeterminant, // xz
		0,
		(cy.x * cz.z - cz.x * cy.z) * -oneOverDeterminant, // yx
		(cx.x * cz.z - cz.x * cx.z) *  oneOverDeterminant, // yy
		(cx.x * cy.z - cy.x * cx.z) * -oneOverDeterminant, // yz
		0,
		(cy.x * cz.y - cz.x * cy.y) *  oneOverDeterminant, // zx
		(cx.x * cz.y - cz.x * cx.y) * -oneOverDeterminant, // zy
		(cx.x * cy.y - cy.x * cx.y) *  oneOverDeterminant, // zz
		0,
		0,0,0,1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::inverse3x3Transpose() const -> This {
	T oneOverDeterminant = T(1) / (
		+ cx.x * (cy.y * cz.z - cz.y * cy.z)
		- cy.x * (cx.y * cz.z - cz.y * cx.z)
		+ cz.x * (cx.y * cy.z - cy.y * cx.z));

	return This(
		(cy.y * cz.z - cz.y * cy.z) *  oneOverDeterminant, // xx
		(cy.x * cz.z - cz.x * cy.z) * -oneOverDeterminant, // yx
		(cy.x * cz.y - cz.x * cy.y) *  oneOverDeterminant, // zx
		0,
		(cx.y * cz.z - cz.y * cx.z) * -oneOverDeterminant, // xy
		(cx.x * cz.z - cz.x * cx.z) *  oneOverDeterminant, // yy
		(cx.x * cz.y - cz.x * cx.y) * -oneOverDeterminant, // zy
		0,
		(cx.y * cy.z - cy.y * cx.z) *  oneOverDeterminant, // xz
		(cx.x * cy.z - cy.x * cx.z) * -oneOverDeterminant, // yz
		(cx.x * cy.y - cy.x * cx.y) *  oneOverDeterminant, // zz
		0,
		0,0,0,1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_rotateRad(const Vec3& rad) -> This {
	if (Math::almostZero(rad)) return s_identity();

	Vec3 s, c;
	sincos(rad.x, s.x, c.x);
	sincos(rad.y, s.y, c.y);
	sincos(rad.z, s.z, c.z);

	return This(
		(c.y*c.z), (s.x*s.y*c.z - c.x*s.z), (s.x*s.z + c.x*s.y*c.z), 0,
		(c.y*s.z), (c.x*c.z + s.x*s.y*s.z), (c.x*s.y*s.z - s.x*c.z), 0,
		(-s.y),    (s.x*c.y),               (c.x*c.y),               0,
		0,         0,                        0,                      1
	);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_rotateRadX(T rad) -> This {
	if (Math::almostZero(rad)) return s_identity();

	T s, c;
	sincos(rad, s, c);
	return This( 1, 0, 0, 0,
				 0, c, s, 0,
				 0,-s, c, 0,
				 0, 0, 0, 1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_rotateRadY(T rad) -> This {
	if (Math::almostZero(rad)) return s_identity();

	T s, c;
	sincos(rad, s, c);
	return This( c, 0,-s, 0,
				 0, 1, 0, 0,
				 s, 0, c, 0,
				 0, 0, 0, 1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::s_rotateRadZ(T rad) -> This {
	if (Math::almostZero(rad)) return s_identity();

	T s, c;
	sincos(rad, s, c);
	return This( c, s, 0, 0,
				-s, c, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::s_translateScale(const Vec3 & translate, const Vec3 & scale) -> This {
	return This( scale.x, 0, 0, 0,
				 0, scale.y, 0, 0,
				 0, 0, scale.z, 0,
				 translate.x, translate.y, translate.z, 1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::s_TRS_rad(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale) -> This {
	Vec3 s, c;
	sincos(rotate.x, s.x, c.x);
	sincos(rotate.y, s.y, c.y);
	sincos(rotate.z, s.z, c.z);

	return This(scale.x * (c.y*c.z),				scale.x * (c.y*s.z),               scale.x * (-s.y),    0,
				scale.y * (s.x*s.y*c.z - c.x*s.z),	scale.y * (c.x*c.z + s.x*s.y*s.z), scale.y * (s.x*c.y), 0,
				scale.z * (s.x*s.z + c.x*s.y*c.z),	scale.z * (c.x*s.y*s.z - s.x*c.z), scale.z * (c.x*c.y), 0,
				translate.x, translate.y, translate.z, 1);
}

template <class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_translate(const Vec3& v) -> This {
	return This( 1,   0,   0,   0,
				 0,   1,   0,   0,
				 0,   0,   1,   0,
				 v.x, v.y, v.z, 1);
}

template <class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_scale(const Vec3& s) -> This {
	return This( s.x, 0,   0,   0,
				 0,   s.y, 0,   0,
				 0,   0,   s.z, 0,
				 0,   0,   0,   1);
}

template <class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_shear(const Vec3& v) -> This {
	const T &xy = v.x;
	const T &xz = v.y;
	const T &yz = v.z;
	return This( 1,  0,  0, 0,
				 xy, 1,  0, 0,
				 xz, yz, 1, 0,
				 0,  0,  0, 1);
}


template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_perspective(T fovy_rad, T aspect, T zNear, T zFar) -> This {
	if (Math::almostZero(aspect)) {
		AX_ASSERT(false);
		return {};
	}

	T deltaZ = zFar - zNear;
	T tf = tan(fovy_rad / T(2));
		
	return This(
		1 / (aspect * tf), 0,      0,                           0,
		0,                 1 / tf, 0,                           0,
		0,                 0,      -(zFar + zNear) / deltaZ,   -1,
		0,                 0,      -2 * zNear * zFar / deltaZ,  0
	);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_ortho(T left, T right, T bottom, T top, T zNear, T zFar) -> This {
	T w = right - left;
	T h = top - bottom;
	T d = zFar - zNear;

	if (Math::almostZero(w) || Math::almostZero(h) || Math::almostZero(d)) {
		return s_identity();
	}

	return This(
		2/w, 0,    0,   0,
		0,   2/h,  0,   0,
		0,   0,   -2/d, 0,
		-(right+left) / w, -(top+bottom) / h, -(zFar+zNear) / d, 1
	);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_lookAt(const Vec3& eye, const Vec3& aim, const Vec3& up) -> This {
	auto outForward = (aim - eye).normalize();
	auto outSide    = outForward.cross(up).normalize();
	auto outUp      = outSide.cross(outForward);

	return This(
		outSide.x, outUp.x, -outForward.x, 0,
		outSide.y, outUp.y, -outForward.y, 0,
		outSide.z, outUp.z, -outForward.z, 0,
		-outSide.dot(eye), -outUp.dot(eye), outForward.dot(eye), 1
	);
}


template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::unprojectPointFromInverse(const Vec3& screenPos, const Rect2& viewport) const -> Vec3 {
	auto  tmp = Vec4(screenPos, 1);
	tmp.y = viewport.extents().y - tmp.y; // y is down

	tmp.x = (tmp.x - viewport.min.x) / viewport.extents().x * 2 - 1;
	tmp.y = (tmp.y - viewport.min.y) / viewport.extents().y * 2 - 1;

	auto vec = mulPoint(tmp);
	return vec.homogenize();
}

} // namespace
