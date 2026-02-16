module;

export module AxCore.LinearMath:Mat; // Matrix
export import :Vec;
export import :BBox;
export import :Rect;
export import :Quat;

export namespace ax {

using Mat4f			= Mat4_<f32>;
using Mat4f_SSE		= Mat4_<f32, VecSimd::SSE>;
using Mat4f_Basic	= Mat4_<f32, VecSimd::Basic>;
using Mat4d			= Mat4_<f64>;
using Mat4d_SSE		= Mat4_<f64, VecSimd::SSE>;
using Mat4d_Basic	= Mat4_<f64, VecSimd::Basic>;

using Mat3f			= Mat3_<f32>;
using Mat3f_SSE		= Mat3_<f32, VecSimd::SSE>;
using Mat3f_Basic	= Mat3_<f32, VecSimd::Basic>;
using Mat3d			= Mat3_<f64>;
using Mat3d_SSE		= Mat3_<f64, VecSimd::SSE>;
using Mat3d_Basic	= Mat3_<f64, VecSimd::Basic>;

template<class MAT, class T>
struct Mat_NumLimit {
	using T_NumLimit = NumLimit<T>;

	static constexpr bool isExact       =  T_NumLimit::isExact;
	static constexpr bool hasInfinity   =  T_NumLimit::hasInfinity;
	static constexpr MAT  infinity      () { return  MAT::s_all(T_NumLimit::infinity());    }
	static constexpr MAT  negInfinity   () { return  MAT::s_all(T_NumLimit::negInfinity()); }
	static constexpr MAT  lowest        () { return  MAT::s_all(T_NumLimit::lowest());      }
	static constexpr MAT  min           () { return  MAT::s_all(T_NumLimit::min());         }
	static constexpr MAT  max           () { return  MAT::s_all(T_NumLimit::max());         }
	static constexpr MAT  NaN           () { return  MAT::s_all(T_NumLimit::NaN());         }
	static constexpr T    epsilon       () { return  T_NumLimit::epsilon();                 }
};

// column major matrix
template<class T, VecSimd SIMD>
class Mat_<3,3,T,SIMD> {
	using This = Mat_;
	static constexpr bool _use_SSE			= SIMD == VecSimd::SSE;
	static constexpr bool _is_f32			= Type_IsSame<T, f32>;
	static constexpr bool _is_f64			= Type_IsSame<T, f64>;
	static constexpr bool _use_SSE_m128_ps	= _use_SSE && _is_f32;
	static constexpr bool _use_SSE_m256_pd	= _use_SSE && _is_f64;
public:
	using _NumLimit = Mat_NumLimit<This, T>;
	
	using Vec1		= Vec1_< T, SIMD>;
	using Vec2		= Vec2_< T, SIMD>;
	using Vec3		= Vec3_< T, SIMD>;
	using Vec4		= Vec4_< T, SIMD>;
	using Mat3		= Mat3_< T, SIMD>;
	using Mat4		= Mat4_< T, SIMD>;
	using Rect2		= Rect2_<T, SIMD>;
	using Quat4		= Quat4_<T, SIMD>;

	static constexpr Int kRowCount = 3;
	static constexpr Int kColCount = 3;
	static constexpr Int kElementCount = kRowCount * kColCount;

	union {
		struct { Vec3 cx, cy, cz; };
		struct {
			T xx, xy, xz;
			T yx, yy, yz;
			T zx, zy, zz;
		};
		T e[kElementCount];
	};
		
};

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
	using _NumLimit = Mat_NumLimit<This, T>;
	
	using Vec1		= Vec1_< T, SIMD>;
	using Vec2		= Vec2_< T, SIMD>;
	using Vec3		= Vec3_< T, SIMD>;
	using Vec4		= Vec4_< T, SIMD>;
	using Mat3		= Mat3_< T, SIMD>;
	using Mat4		= Mat4_< T, SIMD>;
	using Rect2		= Rect2_<T, SIMD>;
	using Quat4		= Quat4_<T, SIMD>;

	static constexpr Int kRowCount = 4;
	static constexpr Int kColCount = 4;
	static constexpr Int kElementCount = kRowCount * kColCount;

	union {
		struct { Vec4 cx, cy, cz, cw; };
		struct {
			T xx, xy, xz, xw;
			T yx, yy, yz, yw;
			T zx, zy, zz, zw;
			T wx, wy, wz, ww;
		};
		T e[kElementCount];
	};
	
	Mat_() = default;
	AX_NODISCARD AX_INLINE constexpr Mat_(const Vec4& cx_, const Vec4& cy_, const Vec4& cz_, const Vec4& cw_)
		: cx(cx_), cy(cy_), cz(cz_), cw(cw_) {}
	
	AX_NODISCARD AX_INLINE constexpr Mat_(	const T& xx_, const T& xy_, const T& xz_, const T& xw_,
											const T& yx_, const T& yy_, const T& yz_, const T& yw_,
											const T& zx_, const T& zy_, const T& zz_, const T& zw_,
											const T& wx_, const T& wy_, const T& wz_, const T& ww_ )
		: cx(xx_, xy_, xz_, xw_)
		, cy(yx_, yy_, yz_, yw_)
		, cz(zx_, zy_, zz_, zw_)
		, cw(wx_, wy_, wz_, ww_)
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
			const T& xx_, const T& xy_, const T& xz_, const T& xw_,
			const T& yx_, const T& yy_, const T& yz_, const T& yw_,
			const T& zx_, const T& zy_, const T& zz_, const T& zw_,
			const T& wx_, const T& wy_, const T& wz_, const T& ww_ )
	{
		cx.set(xx_, xy_, xz_, xw_);
		cy.set(yx_, yy_, yz_, yw_);
		cz.set(zx_, zy_, zz_, zw_);
		cw.set(wx_, wy_, wz_, ww_);
	}
			T* data()		{ return cx.data(); }
	const	T* data() const { return cx.data(); }

	AX_NODISCARD static constexpr This s_zero();

	AX_NODISCARD static constexpr This s_identity();
	template<class R, VecSimd R_SIMD>
	AX_NODISCARD static constexpr This s_cast(const Mat4_<R, R_SIMD>& v);

	constexpr void setIdentity()	{ *this = s_identity(); }

	AX_NODISCARD constexpr This transpose() const;

	constexpr void setCol(Int i, const Vec4& v) { (&cx)[i] = v; }
	
	AX_NODISCARD AX_INLINE constexpr Vec4 col(Int i) const {
		AX_ASSERT(i >= 0 || i < kColCount);
		return (&cx)[i];
	}
	AX_NODISCARD AX_INLINE constexpr Vec4 row(Int i) const {
		AX_ASSERT(i >= 0 || i < kRowCount);
		return Vec4(cx.e[i], cy.e[i], cz.e[i], cw.e[i]);
	}

	AX_NODISCARD AX_INLINE constexpr bool operator==(const This& r) const	{ return cx == r.cx && cy == r.cy && cz == r.cz && cw == r.cw; }
	AX_NODISCARD AX_INLINE constexpr bool operator!=(const This& r) const	{ return cx != r.cx || cy != r.cy || cz != r.cz || cw != r.cw; }

	AX_NODISCARD constexpr static This s_perspective(T verticalFieldOfViewInRadians, 
													 T width, T height, T nearClip, T farClip, 
													 const ProjectionDesc& desc);

	AX_NODISCARD constexpr static This s_ortho	(T left, T right, T bottom, T top, T nearClip, T farClip, 
													 const ProjectionDesc& desc);
	
	AX_NODISCARD constexpr static This s_lookAt	(const Vec3 & eye, const Vec3 & aim, const Vec3 & up, 
													 const ProjectionDesc& desc);
	
	AX_NODISCARD constexpr static This s_translate	(const Vec3& v);
	AX_NODISCARD constexpr static This s_translate	(const Vec2& v)		{ return s_translate(Vec3(v, 0)); }
	AX_NODISCARD constexpr static This s_translate	(T x, T y, T z)		{ return s_translate(Vec3(x,y,z)); }
	AX_NODISCARD constexpr static This s_translateAndScale	(const Vec3 & translate, const Vec3 & scale);
	AX_NODISCARD constexpr static This s_translateAndScale	(const Vec3 & translate, const T &    scale) {
		return s_translateAndScale(translate, Vec3(scale));
	}
	
	AX_NODISCARD constexpr static This s_rotateRad	(const Vec3 & v);
	AX_NODISCARD constexpr static This s_rotateRad	(T x, T y, T z)		{ return s_rotateRad(Vec3(x,y,z)); }
	AX_NODISCARD constexpr static This s_rotateRadX	(T rad);
	AX_NODISCARD constexpr static This s_rotateRadY	(T rad);
	AX_NODISCARD constexpr static This s_rotateRadZ	(T rad);

	AX_NODISCARD constexpr static This s_rotateDeg	(const Vec3 & v)	{ return s_rotateRad(radians(v)); }
	AX_NODISCARD constexpr static This s_rotateDeg	(T x, T y, T z)		{ return s_rotateRad(radians(Vec3(x,y,z))); }
	AX_NODISCARD constexpr static This s_rotateDegX	(T deg)				{ return s_rotateRadX(radians(deg)); }
	AX_NODISCARD constexpr static This s_rotateDegY	(T deg)				{ return s_rotateRadY(radians(deg)); }
	AX_NODISCARD constexpr static This s_rotateDegZ	(T deg)				{ return s_rotateRadZ(radians(deg)); }

	AX_NODISCARD constexpr static This s_quat		(const Quat4& q);

	AX_NODISCARD constexpr static This s_scale	(T s)				{ return s_scale(Vec3::s_all(s)); }
	AX_NODISCARD constexpr static This s_scale	(const Vec2 & v)	{ return s_scale(Vec3(v, 1)); }
	AX_NODISCARD constexpr static This s_scale	(const Vec3 & v);
	AX_NODISCARD constexpr static This s_scale	(T x, T y, T z)		{ return s_scale(Vec3(x,y,z)); }

	AX_NODISCARD constexpr static This s_shear	(const Vec3 & v);

	struct TRS {
		Vec3  position = Vec3::s_zero();
		Quat4 rotation = Quat4::s_identity();
		Vec3  scale    = Vec3::s_one();
	};
	
	AX_NODISCARD constexpr static This s_TRS		(const TRS& v);
	AX_NODISCARD constexpr static This s_TRS		(const Vec3 & translate, const Quat4 & rotate, const Vec3 & scale);
	AX_NODISCARD constexpr static This s_TRS_rad	(const Vec3 & translate, const Vec3  & rotate, const Vec3 & scale);
	AX_NODISCARD constexpr static This s_TRS_deg	(const Vec3 & translate, const Vec3  & rotate, const Vec3 & scale) {
		return s_TRS_rad(translate, Math::radians(rotate), scale);
	}

	AX_NODISCARD constexpr static This s_directionAndTranslate(const Vec3& dirX, const Vec3& dirY, const Vec3& dirZ,
																const Vec3& translate) {
		return This(dirX.x, dirX.y, dirX.z, 0,
					dirY.x, dirY.y, dirY.z, 0,
					dirZ.x, dirZ.y, dirZ.z, 0,
					translate.x, translate.y, translate.z, 1);
	}

	AX_NODISCARD constexpr static This s_direction(const Vec3& dirX, const Vec3& dirY, const Vec3& dirZ) {
		return s_directionAndTranslate(dirX, dirY, dirZ, Vec3::s_zero());
	}

	AX_NODISCARD constexpr This inverse				() const;
	AX_NODISCARD constexpr This inverse3x3			() const;
	AX_NODISCARD constexpr This inverse3x3Transpose	() const;
	
	AX_NODISCARD constexpr T    determinant			() const;
	
	AX_NODISCARD constexpr Quat4 toQuat() const;

	AX_NODISCARD constexpr Vec3 unprojectPointFromInverseMatrix	(const Vec3& screenPos, const Rect2& viewport) const;
	AX_NODISCARD constexpr Vec3 unprojectPointSlow				(const Vec3& screenPos, const Rect2& viewport) const {
		return inverse().unprojectPointFromInverseMatrix(screenPos, viewport);
	}

			//bool operator==			(const This &r) const	{ return cx == r.cx && cy == r.cy && cw == r.cw && cz == r.cz; }
			//bool operator!=			(const This &r) const	{ return cx != r.cx || cy != r.cy || cw != r.cw || cz != r.cz; }

	AX_NODISCARD constexpr bool almostEqual	(const This &r, T epsilon = Math::epsilon_<T>()) const;

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

	AX_NODISCARD constexpr Vec3 directionX() const { return Vec3(cx.x, cy.x, cz.x).normalize(); }
	AX_NODISCARD constexpr Vec3 directionY() const { return Vec3(cx.y, cy.y, cz.y).normalize(); }
	AX_NODISCARD constexpr Vec3 directionZ() const { return Vec3(cx.z, cy.z, cz.z).normalize(); }
	
	AX_NODISCARD constexpr Vec3 position() const { return cw.xyz(); }
				 constexpr void setPosition(const Vec3& pos) { cw.x = pos.x; cw.y = pos.y; cw.z = pos.z; }
	
	AX_NODISCARD constexpr Vec3  scale() const;
	AX_NODISCARD constexpr Quat4 rotation() const;

	constexpr TRS  getTRS() const;
	constexpr void setTRS(const TRS& trs) { *this = s_TRS(trs); }
	constexpr void setTRS(const Vec3& pos, const Quat4& quat, const Vec3& scale) { *this = s_TRS(pos, quat, scale); }
};

template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::This Mat_<4, 4, T, SIMD>::s_zero() {
	return This(Vec4::s_zero(), Vec4::s_zero(), Vec4::s_zero(), Vec4::s_zero());
}

template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::This Mat_<4, 4, T, SIMD>::s_identity() { 
	return This(1,0,0,0,
	            0,1,0,0,
	            0,0,1,0,
	            0,0,0,1); 
}

template<class T, VecSimd SIMD>
template<class R, VecSimd R_SIMD> constexpr
typename Mat_<4, 4, T, SIMD>::This Mat_<4, 4, T, SIMD>::s_cast(const Mat4_<R, R_SIMD>& v) {
	This o;
	for (Int i = 0; i < kElementCount; i++) {
		o.e[i] = static_cast<T>(v.e[i]);
	}
	return o;
}

template<class T, VecSimd SIMD> constexpr
bool Mat_<4,4,T,SIMD>::almostEqual(const This &r, T epsilon) const {
	return cx.almostEqual(r.cx, epsilon)
		&& cy.almostEqual(r.cy, epsilon)
		&& cw.almostEqual(r.cw, epsilon)
		&& cz.almostEqual(r.cz, epsilon);
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
	return mulPoint(Vec4(v, 1)).xyz_div_w();
}

template<class T, VecSimd SIMD> AX_NODISCARD AX_INLINE constexpr
auto Mat_<4, 4, T, SIMD>::mulPoint4x3(const Vec3& v) const -> Vec3 {
	return mulPoint(Vec4(v, 1)).xyz();
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::mulVector(const Vec3& v) const -> Vec3 {
	return Vec3(cx.xyz() * v.x 
			  + cy.xyz() * v.y 
			  + cz.xyz() * v.z);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::mulNormal(const Vec3& v) const -> Vec3 {
	return inverse3x3Transpose().mulVector(v);
}

template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::Vec3 Mat_<4, 4, T, SIMD>::scale() const {
	Vec3 scale;
	scale.x = cx.xyz().length();
	scale.y = cy.xyz().length();
	scale.z = cz.xyz().length();
	return scale;
}

template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::Quat4 Mat_<4, 4, T, SIMD>::rotation() const {
	Vec3 pos, scale;
	Quat4 quat;
	getTRS(pos, quat, scale);
	return quat;
}

template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::TRS Mat_<4, 4, T, SIMD>::getTRS() const {
	TRS o;
	o.position = cw.xyz();
	o.scale    = scale();
	if (determinant() < 0) { o.scale = -o.scale; }
	
	// remove scale
	Mat4 mat(Math::safeDiv(cx.xyz0(), o.scale.x),
			 Math::safeDiv(cy.xyz0(), o.scale.y),
			 Math::safeDiv(cz.xyz0(), o.scale.z),
			 Vec4(0,0,0,1));
	o.rotation = mat.toQuat();
	return o;
}

template<class T, VecSimd SIMD>
constexpr auto Mat_<4, 4, T, SIMD>::toQuat() const -> Quat4 {
	Quat4 quat; 
	T t = xx + yy + zz;
	if( t > T(0)) {
		T s = std::sqrt(1 + t) * T(2.0);
		quat.x = (zy - yz) / s;
		quat.y = (xz - zx) / s;
		quat.z = (yx - xy) / s;
		quat.w = T(0.25) * s;
		
	} else if( xx > yy && xx > zz ) {
		T s = std::sqrt(T(1) + xx - yy - zz) * T(2);
		quat.x = T(0.25) * s;
		quat.y = (yx + xy) / s;
		quat.z = (xz + zx) / s;
		quat.w = (zy - yz) / s;
		
	} else if( yy > zz) {
		T s = std::sqrt(T(1) + yy - xx - zz) * T(2);
		quat.x = (yx + xy) / s;
		quat.y = T(0.25) * s;
		quat.z = (zy + yz) / s;
		quat.w = (xz - zx) / s;
		
	} else {
		T s = std::sqrt(T(1) + zz - xx - yy) * T(2);
		quat.x = (xz + zx) / s;
		quat.y = (zy + yz) / s;
		quat.z = T(0.25) * s;
		quat.w = (yx - xy) / s;
	}
	return quat;
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::transpose() const -> This {
	return This(xx, yx, zx, wx,
				xy, yy, zy, wy,
				xz, yz, zz, wz,
				xw, yw, zw, ww);
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

	e0 = xx, e1 = yx, e2 = zx, e3 = wx;
	o.xx = e0 * m.xx + e1 * m.xy + e2 * m.xz + e3 * m.xw;
	o.yx = e0 * m.yx + e1 * m.yy + e2 * m.yz + e3 * m.yw;
	o.zx = e0 * m.zx + e1 * m.zy + e2 * m.zz + e3 * m.zw;
	o.wx = e0 * m.wx + e1 * m.wy + e2 * m.wz + e3 * m.ww;

	e0 = xy, e1 = yy, e2 = zy, e3 = wy;
	o.xy = e0 * m.xx + e1 * m.xy + e2 * m.xz + e3 * m.xw;
	o.yy = e0 * m.yx + e1 * m.yy + e2 * m.yz + e3 * m.yw;
	o.zy = e0 * m.zx + e1 * m.zy + e2 * m.zz + e3 * m.zw;
	o.wy = e0 * m.wx + e1 * m.wy + e2 * m.wz + e3 * m.ww;

	e0 = xz, e1 = yz, e2 = zz, e3 = wz;
	o.xz = e0 * m.xx + e1 * m.xy + e2 * m.xz + e3 * m.xw;
	o.yz = e0 * m.yx + e1 * m.yy + e2 * m.yz + e3 * m.yw;
	o.zz = e0 * m.zx + e1 * m.zy + e2 * m.zz + e3 * m.zw;
	o.wz = e0 * m.wx + e1 * m.wy + e2 * m.wz + e3 * m.ww;

	e0 = xw, e1 = yw, e2 = zw, e3 = ww;
	o.xw = e0 * m.xx + e1 * m.xy + e2 * m.xz + e3 * m.xw;
	o.yw = e0 * m.yx + e1 * m.yy + e2 * m.yz + e3 * m.yw;
	o.zw = e0 * m.zx + e1 * m.zy + e2 * m.zz + e3 * m.zw;
	o.ww = e0 * m.wx + e1 * m.wy + e2 * m.wz + e3 * m.ww;

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
	T coef00 = zz * ww - wz * zw;
	T coef02 = yz * ww - wz * yw;
	T coef03 = yz * zw - zz * yw;

	T coef04 = zy * ww - wy * zw;
	T coef06 = yy * ww - wy * yw;
	T coef07 = yy * zw - zy * yw;

	T coef08 = zy * wz - wy * zz;
	T coef10 = yy * wz - wy * yz;
	T coef11 = yy * zz - zy * yz;

	T coef12 = zx * ww - wx * zw;
	T coef14 = yx * ww - wx * yw;
	T coef15 = yx * zw - zx * yw;

	T coef16 = zx * wz - wx * zz;
	T coef18 = yx * wz - wx * yz;
	T coef19 = yx * zz - zx * yz;

	T coef20 = zx * wy - wx * zy;
	T coef22 = yx * wy - wx * yy;
	T coef23 = yx * zy - zx * yy;

	Vec4 fac0(coef00, coef00, coef02, coef03);
	Vec4 fac1(coef04, coef04, coef06, coef07);
	Vec4 fac2(coef08, coef08, coef10, coef11);
	Vec4 fac3(coef12, coef12, coef14, coef15);
	Vec4 fac4(coef16, coef16, coef18, coef19);
	Vec4 fac5(coef20, coef20, coef22, coef23);

	Vec4 v0(yx, xx, xx, xx);
	Vec4 v1(yy, xy, xy, xy);
	Vec4 v2(yz, xz, xz, xz);
	Vec4 v3(yw, xw, xw, xw);

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
		+ xx * (yy * zz - zy * yz)
		- yx * (xy * zz - zy * xz)
		+ zx * (xy * yz - yy * xz));

	return This(
		(yy * zz - zy * yz) *  oneOverDeterminant, // xx
		(xy * zz - zy * xz) * -oneOverDeterminant, // xy
		(xy * yz - yy * xz) *  oneOverDeterminant, // xz
		0,
		(yx * zz - zx * yz) * -oneOverDeterminant, // yx
		(xx * zz - zx * xz) *  oneOverDeterminant, // yy
		(xx * yz - yx * xz) * -oneOverDeterminant, // yz
		0,
		(yx * zy - zx * yy) *  oneOverDeterminant, // zx
		(xx * zy - zx * xy) * -oneOverDeterminant, // zy
		(xx * yy - yx * xy) *  oneOverDeterminant, // zz
		0,
		0,0,0,1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::inverse3x3Transpose() const -> This {
	T oneOverDeterminant = T(1) / (
		+ xx * (yy * zz - zy * yz)
		- yx * (xy * zz - zy * xz)
		+ zx * (xy * yz - yy * xz));

	return This(
		(yy * zz - zy * yz) *  oneOverDeterminant, // xx
		(yx * zz - zx * yz) * -oneOverDeterminant, // yx
		(yx * zy - zx * yy) *  oneOverDeterminant, // zx
		0,
		(xy * zz - zy * xz) * -oneOverDeterminant, // xy
		(xx * zz - zx * xz) *  oneOverDeterminant, // yy
		(xx * zy - zx * xy) * -oneOverDeterminant, // zy
		0,
		(xy * yz - yy * xz) *  oneOverDeterminant, // xz
		(xx * yz - yx * xz) * -oneOverDeterminant, // yz
		(xx * yy - yx * xy) *  oneOverDeterminant, // zz
		0,
		0,0,0,1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::determinant() const -> T {
	return (xx * yy * zz * ww) - (xx * yy * zw * wz) + (xx * yz * zw * wy) - (xx * yz * zy * ww)
		 + (xx * yw * zy * wz) - (xx * yw * zz * wy) - (xy * yz * zw * wx) + (xy * yz * zx * ww)
		 - (xy * yw * zx * wz) + (xy * yw * zz * wx) - (xy * yx * zz * ww) + (xy * yx * zw * wz)
		 + (xz * yw * zx * wy) - (xz * yw * zy * wx) + (xz * yx * zy * ww) - (xz * yx * zw * wy)
		 + (xz * yy * zw * wx) - (xz * yy * zx * ww) - (xw * yx * zy * wz) + (xw * yx * zz * wy)
		 - (xw * yy * zz * wx) + (xw * yy * zx * wz) - (xw * yz * zx * wy) + (xw * yz * zy * wx); 
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_rotateRad(const Vec3& rad) -> This {
	if (Math::almostZero(rad)) return s_identity();

	Vec3 s, c;
	sincos(rad.x, s.x, c.x);
	sincos(rad.y, s.y, c.y);
	sincos(rad.z, s.z, c.z);

	return This(
		(c.y * c.z), (s.x * s.y * c.z - c.x * s.z), (s.x * s.z + c.x * s.y * c.z), 0,
		(c.y * s.z), (c.x * c.z + s.x * s.y * s.z), (c.x * s.y * s.z - s.x * c.z), 0,
		(-s.y),      (s.x * c.y),                   (c.x * c.y),                   0,
		0,           0,                             0,                             1
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
auto Mat_<4,4,T,SIMD>::s_translateAndScale(const Vec3 & translate, const Vec3 & scale) -> This {
	return This( scale.x, 0, 0, 0,
				 0, scale.y, 0, 0,
				 0, 0, scale.z, 0,
				 translate.x, translate.y, translate.z, 1);
}


template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::This Mat_<4, 4, T, SIMD>::s_TRS(const TRS& v) {
	return s_TRS(v.pos, v.rot, v.scale);
}

template<class T, VecSimd SIMD>
constexpr typename Mat_<4, 4, T, SIMD>::This Mat_<4, 4, T, SIMD>::s_TRS(const Vec3& translate, const Quat4& rotate, const Vec3& scale) {
	T x = rotate.x;
	T y = rotate.y;
	T z = rotate.z;
	T w = rotate.w;
	
	Mat4 mat;
	mat.cx.x = scale.x * (T(1) - T(2) * (y * y + z * z));
	mat.cx.y = scale.x * (T(2) * (x * y - z * w));
	mat.cx.z = scale.x * (T(2) * (x * z + y * w));
	mat.cx.w = 0;

	mat.cy.x = scale.y * (T(2) * (x * y + z * w));
	mat.cy.y = scale.y * (T(1) - T(2) * (x * x + z * z));
	mat.cy.z = scale.y * (T(2) * (y * z - x * w));
	mat.cy.w = 0;

	mat.cz.x = scale.z * (T(2) * (x * z - y * w));
	mat.cz.y = scale.z * (T(2) * (y * z + x * w));
	mat.cz.z = scale.z * (T(1) - T(2) * (x * x + y * y));
	mat.cz.w = 0;

	mat.cw = translate.xyz1();
	return mat;	
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::s_TRS_rad(const Vec3 & translate, const Vec3 & rotate, const Vec3 & scale) -> This {
	Vec3 s, c;
	Math::sincos(rotate.x, s.x, c.x);
	Math::sincos(rotate.y, s.y, c.y);
	Math::sincos(rotate.z, s.z, c.z);

	Mat4 mat;
	mat.cx.x = scale.x * (c.y * c.z);
	mat.cx.y = scale.x * (c.y * s.z);
	mat.cx.z = scale.x * (-s.y);
	mat.cx.w = 0;

	mat.cy.x = scale.y * (s.x * s.y * c.z - c.x * s.z);
	mat.cy.y = scale.y * (c.x * c.z + s.x * s.y * s.z);
	mat.cy.z = scale.y * (s.x * c.y);
	mat.cy.w = 0;

	mat.cz.x = scale.z * (s.x * s.z + c.x * s.y * c.z);
	mat.cz.y = scale.z * (c.x * s.y * s.z - s.x * c.z);
	mat.cz.z = scale.z * (c.x * c.y);
	mat.cz.w = 0;

	mat.cw = translate.xyz1();
	return mat;
}


template<class T, VecSimd SIMD> constexpr 
auto Mat_<4,4,T,SIMD>::s_quat(const Quat4& quat) -> This  {
	T x = quat.x;
	T y = quat.y;
	T z = quat.z;
	T w = quat.w;
	
	Mat4 mat;
	mat.cx.x = (T(1) - T(2) * (y * y + z * z));
	mat.cx.y = (T(2) * (x * y - z * w));
	mat.cx.z = (T(2) * (x * z + y * w));
	mat.cx.w = 0;

	mat.cy.x = (T(2) * (x * y + z * w));
	mat.cy.y = (T(1) - T(2) * (x * x + z * z));
	mat.cy.z = (T(2) * (y * z - x * w));
	mat.cy.w = 0;

	mat.cz.x = (T(2) * (x * z - y * w));
	mat.cz.y = (T(2) * (y * z + x * w));
	mat.cz.z = (T(1) - T(2) * (x * x + y * y));
	mat.cz.w = 0;

	mat.cw = Vec4(0,0,0,1);
	return mat;	
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
auto Mat_<4, 4, T, SIMD>::s_perspective(T verticalFieldOfViewInRadians,
                                        T width, T height, T nearClip, T farClip, 
                                        const ProjectionDesc& desc) -> This 
{
	T zNear, zFar;
	if (desc.isReverseZ) {
		zFar  = nearClip;
		zNear = farClip;
	} else {
		zNear = nearClip;
		zFar  = farClip;
	}
	
	T dz = zFar - zNear;
	T sinFov, cosFov;
	Math::sincos(verticalFieldOfViewInRadians * T(0.5), sinFov, cosFov);
	T invTanFov = cosFov / sinFov;
	T aspect = height / width;
	
	T forwardSign = desc.isRightHanded ? T(-1) : T(1);
	T zRange      = zFar;
	T zScale      = T(1);
	switch (desc.range) {
		case ProjectionDesc::DepthRange::ZeroToOne: {
			zRange = zFar;
			zScale = T(1);
		} break;
		case ProjectionDesc::DepthRange::NegOneToOne: {
			zRange = zFar + zNear;
			zScale = T(2);
		} break;
		default: AX_ASSERT(false);
	}

	return This(aspect * invTanFov, 0        , 0                          , 0,
				0                 , invTanFov, 0                          , 0,
				0                 , 0        ,  forwardSign  * zRange / dz, forwardSign,
				0                 , 0        , -zFar * zNear * zScale / dz, 0);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_ortho(T left,   T right, T bottom, T top, T nearClip, T farClip,
                                  const ProjectionDesc& desc) -> This 
{
	T zNear, zFar;
	if (desc.isReverseZ) {
		zFar  = nearClip;
		zNear = farClip;
	} else {
		zNear = nearClip;
		zFar  = farClip;
	}
	
	T w  = right - left;
	T h  = top   - bottom;
	T dz = zFar  - zNear;

	T forwardSign = desc.isRightHanded ? T(-1) : T(1);
	T zRange      = zFar;
	T zScale      = T(1);
	switch (desc.range) {
		case ProjectionDesc::DepthRange::ZeroToOne: {
			zRange = zNear;
			zScale = T(1);
		} break;
		case ProjectionDesc::DepthRange::NegOneToOne: {
			zRange  = zFar + zNear;
			zScale = T(2);
		} break;
		default: AX_ASSERT(false);
	}

	return This(2/w              , 0                , 0,                         0,
				0                , 2/h              , 0,                         0,
				0                , 0                , forwardSign * zScale / dz, 0,
				-(right+left) / w, -(top+bottom) / h,              -zRange / dz, 1);
}

template<class T, VecSimd SIMD> constexpr
auto Mat_<4, 4, T, SIMD>::s_lookAt(const Vec3& eye, const Vec3& aim, const Vec3& up,
                                   const ProjectionDesc& desc) -> This 
{
	auto f = (aim - eye).normalize(); // forward
	if (!desc.isRightHanded) f = -f;
	
	auto s = f.cross(up).normalize(); // side
	auto u = s.cross(f);              // up

	return This(
		s.x, u.x, -f.x, 0,
		s.y, u.y, -f.y, 0,
		s.z, u.z, -f.z, 0,
		-s.dot(eye), 
		-u.dot(eye), 
		 f.dot(eye), 1
	);
}


template<class T, VecSimd SIMD> constexpr
auto Mat_<4,4,T,SIMD>::unprojectPointFromInverseMatrix(const Vec3& screenPos, const Rect2& viewport) const -> Vec3 {
	auto  tmp = Vec4(screenPos, 1);
	tmp.y = viewport.size.y - tmp.y; // y is down

	tmp.x = (tmp.x - viewport.x) / viewport.size.x * 2 - 1;
	tmp.y = (tmp.y - viewport.y) / viewport.size.y * 2 - 1;

	return mulPoint(tmp).xyz_div_w();
}

} // namespace
