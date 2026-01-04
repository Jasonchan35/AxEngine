module;

export module AxCore.VecSimd;
export import AxCore.Num;

export namespace ax {

#define AX_NumSIMD_ENUM_LIST(E) \
	E(Default,) \
	E(Basic,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_NumSIMD_ENUM_LIST, VecSimd, u8)

inline constexpr VecSimd VecSimd_Default = VecSimd::SSE;

class VecSimd_NullReg {};
template<Int N, class T, VecSimd SIMD>
struct VecSimdInfo_ { using Register = VecSimd_NullReg; };

template<> struct VecSimdInfo_<4, f32, VecSimd::SSE>	{ using Register = __m128;   }; 
template<> struct VecSimdInfo_<4, f64, VecSimd::SSE>	{ using Register = __m256d;  };

template<class VEC, class T>
struct VecSimd_NumLimit {
	using T_NumLimit = NumLimit<T>;

	static constexpr bool isExact       =  T_NumLimit::isExact;
	static constexpr bool hasInfinity   =  T_NumLimit::hasInfinity;
	static constexpr VEC  infinity      () { return  VEC::s_all(T_NumLimit::infinity());    }
	static constexpr VEC  negInfinity   () { return  VEC::s_all(T_NumLimit::negInfinity()); }
	static constexpr VEC  lowest        () { return  VEC::s_all(T_NumLimit::lowest());      }
	static constexpr VEC  min           () { return  VEC::s_all(T_NumLimit::min());         }
	static constexpr VEC  max           () { return  VEC::s_all(T_NumLimit::max());         }
	static constexpr VEC  epsilon       () { return  VEC::s_all(T_NumLimit::epsilon());     }
	static constexpr VEC  NaN           () { return  VEC::s_all(T_NumLimit::NaN());         }
};

template<Int N, class T, VecSimd SIMD>
class VecSimd_Data_ {
	using Info = VecSimdInfo_<N,T,SIMD>;
	using Register = typename Info::Register;
public:
	using Vec = VecSimd_Data_;
	union {
		Register	mm;	
		T			e[N];
	};
	
	using VecData1 = VecSimd_Data_<1,T,SIMD>;
	using VecData2 = VecSimd_Data_<2,T,SIMD>;
	using VecData3 = VecSimd_Data_<3,T,SIMD>;
	using VecData4 = VecSimd_Data_<4,T,SIMD>;
	
	static constexpr bool _use_SSE			= SIMD == VecSimd::SSE;
	static constexpr bool _is_f32			= Type_IsSame<T, f32>;
	static constexpr bool _is_f64			= Type_IsSame<T, f64>;
	static constexpr bool _use_SSE_m128_ps	= N == 4 && _use_SSE && _is_f32;
	static constexpr bool _use_SSE_m256_pd	= N == 4 && _use_SSE && _is_f64;

	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_() = default;
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(const VecSimd_Data_&) = default;
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(const Register& mm_) : mm(mm_) {}
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(TagZero_T) {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_m128_ps) { mm =    _mm_setzero_ps(); return; }
			if constexpr (_use_SSE_m256_pd) { mm = _mm256_setzero_pd(); return; }
		}
		if constexpr (N > 0) e[0] = 0;
		if constexpr (N > 1) e[1] = 0;
		if constexpr (N > 2) e[2] = 0;
		if constexpr (N > 3) e[3] = 0;
	}

	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(const Num1_<T>& v) : VecSimd_Data_(v.at_noBoundCheck(0)) {}
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(const Num2_<T>& v) : VecSimd_Data_(v.at_noBoundCheck(0), v.at_noBoundCheck(1)) {}
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(const Num3_<T>& v) : VecSimd_Data_(v.at_noBoundCheck(0), v.at_noBoundCheck(1), v.at_noBoundCheck(2)) {}
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(const Num4_<T>& v) : VecSimd_Data_(v.at_noBoundCheck(0), v.at_noBoundCheck(1), v.at_noBoundCheck(2), v.at_noBoundCheck(3)) {}
	
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(T t0) : e{t0} {
		static_assert(N == 1);
	}
	
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(T t0, T t1) : e{t0, t1} {
		static_assert(N == 2);
	}
	
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(T t0, T t1, T t2) : e{t0, t1, t2} {
		static_assert(N == 3);
	}
	
	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(T t0, T t1, T t2, T t3) : e{t0, t1, t2, t3}{
		static_assert(N == 4);
	}

	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(VecSimd_Data_<3,T,SIMD> v, T t3) : e{v.e[0], v.e[1], v.e[2], t3}{
		static_assert(N == 4);
	}

	AX_NODISCARD AX_INLINE constexpr VecSimd_Data_(T t0, VecSimd_Data_<3,T,SIMD> v) : e{t0, v.e[0], v.e[1], v.e[2]}{
		static_assert(N == 4);
	}
	
	      T* data()			{ return e; }
	const T* data() const	{ return e; }
	
	template <class R, VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static Vec s_cast(const VecSimd_Data_<N, R, R_SIMD>& vec) {
		Vec ret;
		AX_VC_WARNING_PUSH_AND_DISABLE(4244) // Warning C4244 : '=': conversion from 'R' to 'T', possible loss of data
		if constexpr (N > 0) ret.e[0] = static_cast<R>(vec.e[0]);
		if constexpr (N > 1) ret.e[1] = static_cast<R>(vec.e[1]);
		if constexpr (N > 2) ret.e[2] = static_cast<R>(vec.e[2]);
		if constexpr (N > 3) ret.e[3] = static_cast<R>(vec.e[3]);
		AX_VC_WARNING_POP()
		return ret;
	}
	
	AX_NODISCARD AX_INLINE constexpr static Vec s_all (T t) {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_m128_ps) return    _mm_set1_ps(t);
			if constexpr (_use_SSE_m256_pd) return _mm256_set1_pd(t);
		}
		return s_unroll(t, [](T t){ return t; });
	}
	AX_NODISCARD AX_INLINE constexpr static Vec s_zero() { return VecSimd_Data_(TagZero); }
	AX_NODISCARD AX_INLINE constexpr static Vec s_one () { return s_all(1); }

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE bool almostEqual(VecSimd_Data_<N, T, R_SIMD> vec) const {
		return unroll_and(vec, [](T a, T b){ return Math::almostEqual(a, b); });
	}

	AX_NODISCARD AX_INLINE bool almostZero() const {
		return unroll_and_0([](T a){ return Math::almostZero(a); });
	}

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE bool exactlyEqual(VecSimd_Data_<N, T, R_SIMD> vec) const {
		return unroll_and(vec, [](T a, T b){ return Math::exactlyEqual(a, b); });
	}

	AX_NODISCARD AX_INLINE bool operator==(Vec vec) const { return exactlyEqual(vec); }

	AX_NODISCARD AX_INLINE Vec min(Vec vec) const { return unroll(vec, [](T a, T b){ return Math::min(a, b); }); }
	AX_NODISCARD AX_INLINE Vec max(Vec vec) const { return unroll(vec, [](T a, T b){ return Math::max(a, b); }); }
	
	AX_NODISCARD AX_INLINE constexpr auto operator+(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_m128_ps) return    _mm_add_ps(mm, vec.mm);
			if constexpr (_use_SSE_m256_pd) return _mm256_add_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a + b; });
	}
	
	AX_NODISCARD AX_INLINE constexpr auto operator-(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_m128_ps) return    _mm_sub_ps(mm, vec.mm);
			if constexpr (_use_SSE_m256_pd) return _mm256_sub_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a - b; });
	}
	
	AX_NODISCARD AX_INLINE constexpr auto operator*(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_m128_ps) return    _mm_mul_ps(mm, vec.mm);
			if constexpr (_use_SSE_m256_pd) return _mm256_mul_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a * b; });
	}

	AX_NODISCARD AX_INLINE constexpr auto operator/(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_m128_ps) return    _mm_div_ps(mm, vec.mm);
			if constexpr (_use_SSE_m256_pd) return _mm256_div_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a / b; });
	}

	AX_NODISCARD AX_INLINE constexpr auto operator+(T t) const -> Vec { return *this + s_all(t); }
	AX_NODISCARD AX_INLINE constexpr auto operator-(T t) const -> Vec { return *this - s_all(t); }
	AX_NODISCARD AX_INLINE constexpr auto operator*(T t) const -> Vec { return *this * s_all(t); }
	AX_NODISCARD AX_INLINE constexpr auto operator/(T t) const -> Vec { return *this / s_all(t); }
	
	AX_INLINE constexpr auto operator+=(Vec vec) -> void { *this = *this + vec; }
	AX_INLINE constexpr auto operator-=(Vec vec) -> void { *this = *this - vec; }
	AX_INLINE constexpr auto operator*=(Vec vec) -> void { *this = *this * vec; }
	AX_INLINE constexpr auto operator/=(Vec vec) -> void { *this = *this / vec; }
	AX_INLINE constexpr auto operator+=(T t) -> void { *this = *this + t; }
	AX_INLINE constexpr auto operator-=(T t) -> void { *this = *this - t; }
	AX_INLINE constexpr auto operator*=(T t) -> void { *this = *this * t; }
	AX_INLINE constexpr auto operator/=(T t) -> void { *this = *this / t; }

	AX_NODISCARD AX_INLINE constexpr T horizontalAdd() const {
		T sum = e[0];
		if constexpr (N > 1) sum += e[1];
		if constexpr (N > 2) sum += e[2];
		if constexpr (N > 3) sum += e[3];
		return sum;
	}
	
	AX_NODISCARD AX_INLINE constexpr T dot(Vec vec) const {
#if 0		
		if constexpr (N == 3 && _use_SSE && _is_f32) {
			__m128 v1 = _mm_set_ps(0.0f,     e[2],     e[1],     e[0]); 
			__m128 v2 = _mm_set_ps(0.0f, vec.e[2], vec.e[1], vec.e[0]);
			// Mask 0x71:
			// 0x7 (High nibble): 1110 -> Multiply elements 0, 1, and 2 (ignore 3)
			// 0x1 (Low nibble):  0001 -> Store the result in element 0 of the output
			__m128 res = _mm_dp_ps(v1, v2, 0x71);
			return _mm_cvtss_f32(res);
		}
#endif		
		auto tmp = *this * vec;
		return tmp.horizontalAdd();
	}

//------------
	AX_NODISCARD AX_INLINE static constexpr Vec s_unroll(T t, T (*func)(T a)) {
		Vec ret;
		if constexpr (N > 0) ret.e[0] = func(t);
		if constexpr (N > 1) ret.e[1] = func(t);
		if constexpr (N > 2) ret.e[2] = func(t);
		if constexpr (N > 3) ret.e[3] = func(t);
		return ret;
	}		

	AX_NODISCARD AX_INLINE constexpr bool unroll_and_0(bool (*func)(T a)) const {
		bool ret = true;
		if constexpr (N > 0) ret = ret && func(e[0]);
		if constexpr (N > 1) ret = ret && func(e[1]);
		if constexpr (N > 2) ret = ret && func(e[2]);
		if constexpr (N > 3) ret = ret && func(e[3]);
		return ret;
	}

	AX_NODISCARD AX_INLINE constexpr bool unroll_or(bool (*func)(T a)) const {
		bool ret = false;
		if constexpr (N > 0) ret = ret || func(e[0]);
		if constexpr (N > 1) ret = ret || func(e[1]);
		if constexpr (N > 2) ret = ret || func(e[2]);
		if constexpr (N > 3) ret = ret || func(e[3]);
		return ret;
	}

	template<VecSimd R_SIMD>
	AX_NODISCARD AX_INLINE constexpr bool unroll_and(VecSimd_Data_<N, T, R_SIMD> vec, bool (*func)(T a, T b)) const {
		bool ret = true;
		if constexpr (N > 0) ret = ret && func(e[0], vec.e[0]);
		if constexpr (N > 1) ret = ret && func(e[1], vec.e[1]);
		if constexpr (N > 2) ret = ret && func(e[2], vec.e[2]);
		if constexpr (N > 3) ret = ret && func(e[3], vec.e[3]);
		return ret;
	}

	AX_NODISCARD AX_INLINE constexpr bool unroll_or(Vec vec, bool (*func)(T a, T b)) const {
		bool ret = false;
		if constexpr (N > 0) ret = ret || func(e[0], vec.e[0]);
		if constexpr (N > 1) ret = ret || func(e[1], vec.e[1]);
		if constexpr (N > 2) ret = ret || func(e[2], vec.e[2]);
		if constexpr (N > 3) ret = ret || func(e[3], vec.e[3]);
		return ret;
	}
	
	AX_NODISCARD AX_INLINE constexpr Vec unroll(Vec vec, T (*func)(T a, T b)) const {
		Vec ret;
		if constexpr (N > 0) ret.e[0] = func(e[0], vec.e[0]);
		if constexpr (N > 1) ret.e[1] = func(e[1], vec.e[1]);
		if constexpr (N > 2) ret.e[2] = func(e[2], vec.e[2]);
		if constexpr (N > 3) ret.e[3] = func(e[3], vec.e[3]);
		return ret;
	}

	AX_NODISCARD AX_INLINE constexpr Vec unroll(T t, T (*func)(T a, T b)) const {
		Vec ret;
		if constexpr (N > 0) ret.e[0] = func(t);
		if constexpr (N > 1) ret.e[1] = func(t);
		if constexpr (N > 2) ret.e[2] = func(t);
		if constexpr (N > 3) ret.e[3] = func(t);
		return ret;
	}		
	
	template<class CH>
	constexpr void onFormat(Format_<CH> & fmt) const {
		if constexpr (false) {}
		else if constexpr (N == 1) fmt << Fmt("({})"         , e[0]);
		else if constexpr (N == 2) fmt << Fmt("({},{})"      , e[0], e[1]);
		else if constexpr (N == 3) fmt << Fmt("({},{},{})"   , e[0], e[1], e[2]);
		else if constexpr (N == 4) fmt << Fmt("({},{},{},{})", e[0], e[1], e[2], e[3]);
		else { static_assert(false); }
	}
	
};

} // namespace
