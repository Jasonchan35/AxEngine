module;
#include "AxCore-pch.h"
export module AxCore.VecSimd;
export import AxCore.NormInt;
export import AxCore.Float16;

export import AxCore.NumSIMD4;

export namespace ax {

class VecSimd_Register_None {};
template<Int N, class T, CpuSIMD SIMD>	struct VecSimdInfo_							{ using Register = VecSimd_Register_None; static constexpr Int padding = 0; };
template<> 								struct VecSimdInfo_<3, f32, CpuSIMD::SSE>	{ using Register = __m128;                static constexpr Int padding = 1; }; 
template<> 								struct VecSimdInfo_<3, f64, CpuSIMD::SSE>	{ using Register = __m256d;               static constexpr Int padding = 1; };
template<> 								struct VecSimdInfo_<4, f32, CpuSIMD::SSE>	{ using Register = __m128;                static constexpr Int padding = 0; }; 
template<> 								struct VecSimdInfo_<4, f64, CpuSIMD::SSE>	{ using Register = __m256d;               static constexpr Int padding = 0; };

template<class VEC, class T>
struct VecSIMD_NumLimit {
	using T_NumLimit = NumLimit<T>;

	static constexpr bool isExactType   =  T_NumLimit::isExactType;
	static constexpr bool hasInfinity   =  T_NumLimit::hasInfinity;
	static constexpr VEC  infinity      =  VEC(Tag::All, T_NumLimit::infinity);
	static constexpr VEC  negInfinity   =  VEC(Tag::All, T_NumLimit::negInfinity);
	static constexpr VEC  lowest        =  VEC(Tag::All, T_NumLimit::lowest);
	static constexpr VEC  min           =  VEC(Tag::All, T_NumLimit::min);
	static constexpr VEC  max           =  VEC(Tag::All, T_NumLimit::max);
	static constexpr VEC  epsilon       =  VEC(Tag::All, T_NumLimit::epsilon);
	static constexpr VEC  NaN           =  VEC(Tag::All, T_NumLimit::NaN);
};

template<Int N, class T, CpuSIMD SIMD>
class VecSimd_ {
	using Info = VecSimdInfo_<N,T,SIMD>;
	using Register = typename Info::Register;
	static constexpr Int padding = Info::padding; 
public:
	using Vec = VecSimd_;
	union {
		Register	mm;	
		T			e[N + padding];
	};
	
	static constexpr bool _use_SSE     = SIMD == CpuSIMD::SSE;
	static constexpr bool _use_SSE_f32x4 = N == 4 && _use_SSE && Type_IsSame<T, f32>;
	static constexpr bool _use_SSE_f64x4 = N == 4 && _use_SSE && Type_IsSame<T, f64>;

	AX_NODISCARD AX_INLINE constexpr VecSimd_() = default;
	AX_NODISCARD AX_INLINE constexpr VecSimd_(const Register& mm_) : mm(mm_) {}
	AX_NODISCARD AX_INLINE constexpr VecSimd_(T t0, T t1, T t2) : e{t0, t1, t2} {
		static_assert(N == 3);
//		e[0] = t0; e[1] = t1; e[2] = t2;;
	}
	
	AX_NODISCARD AX_INLINE constexpr VecSimd_(T t0, T t1, T t2, T t3) : e{t0, t1, t2, t3}{
		static_assert(N == 4);
	}

	template <class R, CpuSIMD R_SIMD>
	AX_NODISCARD AX_INLINE constexpr static Vec s_cast(const VecSimd_<N, R, R_SIMD>& vec) {
		Vec ret;
		if constexpr (N > 0) ret.e[0] = static_cast<R>(vec.e[0]);
		if constexpr (N > 1) ret.e[1] = static_cast<R>(vec.e[1]);
		if constexpr (N > 2) ret.e[2] = static_cast<R>(vec.e[2]);
		if constexpr (N > 3) ret.e[3] = static_cast<R>(vec.e[3]);
		return ret;
	}
	
	AX_NODISCARD AX_INLINE constexpr static Vec s_all (T t) {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32x4) return    _mm_set1_ps(t);
			if constexpr (_use_SSE_f64x4) return _mm256_set1_pd(t);
		}
		return s_unroll(t, [](T t){ return t; });
	}
	AX_NODISCARD AX_INLINE constexpr static Vec s_zero(T v) { return s_all(0); }
	AX_NODISCARD AX_INLINE constexpr static Vec s_one (T v) { return s_all(1); }

	AX_NODISCARD AX_INLINE bool almostEqual(Vec vec) const {
		return unroll_and(vec, [](T a, T b){ return Math::almostEqual(a, b); });
	}

	AX_NODISCARD AX_INLINE bool almostZero(Vec vec) const {
		return unroll_and(vec, [](T a, T b){ return Math::almostZero(a, b); });
	}
	
	AX_NODISCARD AX_INLINE constexpr auto operator+(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32x4) return    _mm_add_ps(mm, vec.mm);
			if constexpr (_use_SSE_f64x4) return _mm256_add_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a + b; });
	}
	
	AX_NODISCARD AX_INLINE constexpr auto operator-(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32x4) return    _mm_sub_ps(mm, vec.mm);
			if constexpr (_use_SSE_f64x4) return _mm256_sub_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a - b; });
	}
	
	AX_NODISCARD AX_INLINE constexpr auto operator*(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32x4) return    _mm_mul_ps(mm, vec.mm);
			if constexpr (_use_SSE_f64x4) return _mm256_mul_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a * b; });
	}

	AX_NODISCARD AX_INLINE constexpr auto operator/(Vec vec) const -> Vec {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32x4) return    _mm_div_ps(mm, vec.mm);
			if constexpr (_use_SSE_f64x4) return _mm256_div_pd(mm, vec.mm);
		}
		return unroll(vec, [](T a, T b) -> T { return a / b; });
	}

	AX_NODISCARD AX_INLINE constexpr auto operator+=(Vec vec) -> void { *this = *this + vec; }
	AX_NODISCARD AX_INLINE constexpr auto operator-=(Vec vec) -> void { *this = *this - vec; }
	AX_NODISCARD AX_INLINE constexpr auto operator*=(Vec vec) -> void { *this = *this * vec; }
	AX_NODISCARD AX_INLINE constexpr auto operator/=(Vec vec) -> void { *this = *this / vec; }
	
//------------
	AX_NODISCARD AX_INLINE static constexpr Vec s_unroll(T t, T (*func)(T a)) {
		Vec ret;
		if constexpr (N > 0) ret.e[0] = func(t);
		if constexpr (N > 1) ret.e[1] = func(t);
		if constexpr (N > 2) ret.e[2] = func(t);
		if constexpr (N > 3) ret.e[3] = func(t);
		return ret;
	}		

	AX_NODISCARD AX_INLINE constexpr bool unroll_and(Vec vec, bool (*func)(T a, T b)) const {
		bool ret = func(e[0], vec.e[0]);
		if constexpr (N > 1) ret = ret && func(e[1], vec.e[1]);
		if constexpr (N > 2) ret = ret && func(e[2], vec.e[2]);
		if constexpr (N > 3) ret = ret && func(e[3], vec.e[3]);
		return ret;
	}

	AX_NODISCARD AX_INLINE constexpr bool unroll_or(Vec vec, bool (*func)(T a, T b)) const {
		bool ret = func(e[0], vec.e[0]);
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
		     if constexpr (N == 3) fmt << Fmt("({},{},{})"   , e[0], e[1], e[2]);
		else if constexpr (N == 4) fmt << Fmt("({},{},{},{})", e[0], e[1], e[2], e[3]);
		else { static_assert(false); }
	}
	
};

} // namespace
