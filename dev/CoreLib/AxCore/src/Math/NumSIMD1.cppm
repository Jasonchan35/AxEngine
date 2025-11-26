module;
#include "AxCore-pch.h"
export module AxCore.NumSIMD1;
export import AxCore._PCH;
export import AxCore.NormInt;
export import AxCore.Float16;

export namespace ax {

#define AX_NumSIMD_ENUM_LIST(E) \
	E(None,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_NumSIMD_ENUM_LIST, CpuSIMD, u8)

class SIMD_MM_Void {};
template<Int N, class T, CpuSIMD SIMD>	struct CpuSIMD_REG_							{ using Type = SIMD_MM_Void; static constexpr Int padding = 0; };
template<> 								struct CpuSIMD_REG_<3, f32, CpuSIMD::SSE>	{ using Type = __m128;       static constexpr Int padding = 1; }; 
template<> 								struct CpuSIMD_REG_<3, f64, CpuSIMD::SSE>	{ using Type = __m256d;      static constexpr Int padding = 1; };
template<> 								struct CpuSIMD_REG_<4, f32, CpuSIMD::SSE>	{ using Type = __m128;       static constexpr Int padding = 0; }; 
template<> 								struct CpuSIMD_REG_<4, f64, CpuSIMD::SSE>	{ using Type = __m256d;      static constexpr Int padding = 0; };

template<Int N, class VEC, class STORAGE> class NumSIMD_;
template<       class VEC, class STORAGE> using NumSIMD1_  = NumSIMD_<1, VEC, STORAGE>;
template<       class VEC, class STORAGE> using NumSIMD2_  = NumSIMD_<2, VEC, STORAGE>;
template<       class VEC, class STORAGE> using NumSIMD3_  = NumSIMD_<3, VEC, STORAGE>;
template<       class VEC, class STORAGE> using NumSIMD4_  = NumSIMD_<4, VEC, STORAGE>;

template <Int N, class VEC, class STORAGE> AX_NODISCARD AX_INLINE
constexpr VEC operator+(const typename VEC::Element& s, const NumSIMD_<N, VEC, STORAGE>& vec) {
	return VEC::s_all(s) + vec;
}

template <Int N, class VEC, class STORAGE> AX_NODISCARD AX_INLINE
constexpr VEC operator-(const typename VEC::Element& s, const NumSIMD_<N, VEC, STORAGE>& vec) {
	return VEC::s_all(s) - vec;
}

template <Int N, class VEC, class STORAGE> AX_NODISCARD AX_INLINE
constexpr VEC operator*(const typename VEC::Element& s, const NumSIMD_<N, VEC, STORAGE>& vec) {
	return VEC::s_all(s) * vec;
}

template <Int N, class VEC, class STORAGE> AX_NODISCARD AX_INLINE
constexpr VEC operator/(const typename VEC::Element& s, const NumSIMD_<N, VEC, STORAGE>& vec) {
	return VEC::s_all(s) / vec;
}

template<class VEC, class T>
struct NumSIMD_NumLimit {
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

template<class T>
constexpr bool Type_Is_NumSIMD = false;

//----------------

template<class VEC, class STORAGE>
class NumSIMD_<1, VEC, STORAGE> : public STORAGE {
	using This = NumSIMD_;
	using T = typename STORAGE::Element;
public:
	using _NumLimit = NumSIMD_NumLimit<This, T>;
	using Element = T;
	using Storage = STORAGE;
	static constexpr Int elementCount = STORAGE::elementCount;
	static constexpr CpuSIMD cpuSIMD  = STORAGE::cpuSIMD;

	static constexpr bool _use_SSE     = cpuSIMD == CpuSIMD::SSE;
	static constexpr bool _use_SSE_f32 = _use_SSE && Type_IsSame<T, f32>;
	static constexpr bool _use_SSE_f64 = _use_SSE && Type_IsSame<T, f64>;

	using Storage::_data;
	using Storage::_e0;

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_NODISCARD AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_data); }
	AX_NODISCARD AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_data); }

	AX_NODISCARD AX_INLINE constexpr NumSIMD_() = default;
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(const Storage & storage_) : Storage(storage_) {}
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(const T& e0) : Storage(e0) {}
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(Tag::All_, const T& v) : Storage(s_all(v)) {}

	AX_INLINE constexpr       T& unsafe_at(Int i)       { return _data[i]; }
	AX_INLINE constexpr const T& unsafe_at(Int i) const { return _data[i]; }

	AX_NODISCARD AX_INLINE static constexpr VEC s_all (const T& v) { return Storage(v); }
	AX_NODISCARD AX_INLINE static constexpr VEC s_zero(const T& v) { return s_all(0); }
	AX_NODISCARD AX_INLINE static constexpr VEC s_one (const T& v) { return s_all(1); }
	
	 AX_INLINE constexpr void setAll(const T& v) { *this = s_all(v); }

	AX_NODISCARD constexpr VEC operator+(const VEC& v) const { 	return VEC(	_e0 + v._e0); }
	AX_NODISCARD constexpr VEC operator-(const VEC& v) const { 	return VEC(	_e0 - v._e0); }
	AX_NODISCARD constexpr VEC operator*(const VEC& v) const { 	return VEC(	_e0 * v._e0); }
	AX_NODISCARD constexpr VEC operator/(const VEC& v) const { 	return VEC(	_e0 / v._e0); }

	AX_NODISCARD AX_INLINE constexpr VEC operator+(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator-(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator*(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator/(const T& v) const { return *this + VEC::s_all(v);  }
	
	AX_INLINE constexpr void operator+=(const VEC& v) { *this = *this + v; }
	AX_INLINE constexpr void operator-=(const VEC& v) { *this = *this - v; }
	AX_INLINE constexpr void operator*=(const VEC& v) { *this = *this * v; }
	AX_INLINE constexpr void operator/=(const VEC& v) { *this = *this / v; }

	AX_NODISCARD AX_INLINE constexpr bool operator< (const VEC& v) const { return _e0 <  v._e0; }
	AX_NODISCARD AX_INLINE constexpr bool operator> (const VEC& v) const { return _e0 >  v._e0; }
	AX_NODISCARD AX_INLINE constexpr bool operator<=(const VEC& v) const { return _e0 <= v._e0; }
	AX_NODISCARD AX_INLINE constexpr bool operator>=(const VEC& v) const { return _e0 >= v._e0; }

	template<class R, class R_STORAGE>
	AX_NODISCARD constexpr bool almostEqual(const NumSIMD_<1, R, R_STORAGE>& v) const { return Math::almostEqual(_e0, v._e0); }

	AX_NODISCARD constexpr VEC abs() const { return VEC(Math::abs(_e0)); }

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const {
		fmt << Fmt("({})", _e0);
	}
};

} // namespace
