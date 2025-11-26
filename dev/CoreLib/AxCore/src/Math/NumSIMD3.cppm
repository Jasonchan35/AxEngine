module;
#include "AxCore-pch.h"
export module AxCore.NumSIMD3;
export import AxCore.NumSIMD2;

export namespace ax {

template<class VEC, class STORAGE>
class NumSIMD_<3, VEC, STORAGE> : public STORAGE {
	using This = NumSIMD_;
	using T = typename STORAGE::Element;
public:
	using _NumLimit = NumSIMD_NumLimit<This, T>;
	using Element = T;
	using Storage = STORAGE;
	static constexpr Int elementCount = STORAGE::elementCount;
	static constexpr CpuSIMD cpuSIMD  = STORAGE::cpuSIMD;

	using Storage::_data;
	using Storage::_e0;
	using Storage::_e1;
	using Storage::_e2;
	
	static constexpr bool _use_SSE     = cpuSIMD == CpuSIMD::SSE;
	static constexpr bool _use_SSE_f32 = _use_SSE && Type_IsSame<T, f32>;
	static constexpr bool _use_SSE_f64 = _use_SSE && Type_IsSame<T, f64>;

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_NODISCARD AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_data); }
	AX_NODISCARD AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_data); }

	AX_NODISCARD AX_INLINE constexpr NumSIMD_() = default;
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(const Storage & storage_) : Storage(storage_) {}
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(const T& e0, const T& e1, const T& e2) : Storage(e0, e1, e2) {}
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(Tag::All_, const T& v) : Storage(s_all(v)) {}

	AX_INLINE constexpr       T& unsafe_at(Int i)       { return _data[i]; }
	AX_INLINE constexpr const T& unsafe_at(Int i) const { return _data[i]; }

	AX_NODISCARD static AX_INLINE constexpr VEC s_all (const T& v);
	AX_NODISCARD static AX_INLINE constexpr VEC s_zero(const T& v) { return s_all(0); }
	AX_NODISCARD static AX_INLINE constexpr VEC s_one (const T& v) { return s_all(1); }
	
	AX_INLINE constexpr void setAll(const T& v) { *this = s_all(v); }

	AX_NODISCARD constexpr VEC operator+(const VEC& v) const;
	AX_NODISCARD constexpr VEC operator-(const VEC& v) const;
	AX_NODISCARD constexpr VEC operator*(const VEC& v) const;
	AX_NODISCARD constexpr VEC operator/(const VEC& v) const;

	AX_NODISCARD AX_INLINE constexpr VEC operator+(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator-(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator*(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator/(const T& v) const { return *this + VEC::s_all(v);  }
	
	AX_INLINE constexpr void operator+=(const VEC& v) { *this = *this + v; }
	AX_INLINE constexpr void operator-=(const VEC& v) { *this = *this - v; }
	AX_INLINE constexpr void operator*=(const VEC& v) { *this = *this * v; }
	AX_INLINE constexpr void operator/=(const VEC& v) { *this = *this / v; }

	AX_NODISCARD constexpr bool operator< (const VEC& v) const;
	AX_NODISCARD constexpr bool operator> (const VEC& v) const;
	AX_NODISCARD constexpr bool operator<=(const VEC& v) const;
	AX_NODISCARD constexpr bool operator>=(const VEC& v) const;

	template<class R, class R_STORAGE>
	AX_NODISCARD constexpr bool almostEqual(const NumSIMD_<3, R, R_STORAGE>& v) const;

	AX_NODISCARD constexpr VEC abs() const;

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const {
		fmt << Fmt("({},{},{})", _e0, _e1, _e2);
	}
};

template <class VEC, class STORAGE> AX_NODISCARD AX_INLINE
constexpr VEC NumSIMD_<3, VEC, STORAGE>::s_all(const T& v) {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return Storage(   _mm_set1_ps(v)); }
		if constexpr (_use_SSE_f64) { return Storage(_mm256_set1_pd(v)); }
	}
	return Storage(v,v,v);
}

template <class VEC, class STORAGE> AX_INLINE
constexpr VEC NumSIMD_<3, VEC, STORAGE>::operator+(const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return Storage(   _mm_add_ps(this->_m, v._m)); }
		if constexpr (_use_SSE_f64) { return Storage(_mm256_add_pd(this->_m, v._m)); } 
	}
	return VEC(	_e0 + v._e0,
				_e1 + v._e1,
				_e2 + v._e2);
}

template <class VEC, class STORAGE> AX_INLINE
constexpr VEC NumSIMD_<3, VEC, STORAGE>::operator-(const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return Storage(   _mm_sub_ps(this->_m, v._m)); }
		if constexpr (_use_SSE_f64) { return Storage(_mm256_sub_pd(this->_m, v._m)); }
	}
	return VEC(	_e0 - v._e0,
				_e1 - v._e1,
				_e2 - v._e2);
}

template <class VEC, class STORAGE> AX_INLINE
constexpr VEC NumSIMD_<3, VEC, STORAGE>::operator*(const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return Storage(   _mm_mul_ps(this->_m, v._m)); }
		if constexpr (_use_SSE_f64) { return Storage(_mm256_mul_pd(this->_m, v._m)); }
	}
	return VEC(	_e0 * v._e0,
				_e1 * v._e1,
				_e2 * v._e2);
}

template <class VEC, class STORAGE> AX_INLINE
constexpr VEC NumSIMD_<3, VEC, STORAGE>::operator/(const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return Storage(   _mm_div_ps(this->_m, v._m)); }
		if constexpr (_use_SSE_f64) { return Storage(_mm256_div_pd(this->_m, v._m)); }
	}
	return VEC(	_e0 / v._e0,
				_e1 / v._e1,
				_e2 / v._e2);
}

template <class VEC, class STORAGE> AX_INLINE
constexpr bool NumSIMD_<3, VEC, STORAGE>::operator< (const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return 0b111 ==    _mm_movemask_ps(    _mm_cmp_ps(this->_m, v._m, _CMP_LT_OQ)); } 
		if constexpr (_use_SSE_f64) { return 0b111 == _mm256_movemask_pd( _mm256_cmp_pd(this->_m, v._m, _CMP_LT_OQ)); } 
	}
	return _e0 < v._e0
		&& _e1 < v._e1
		&& _e2 < v._e2;
}

template <class VEC, class STORAGE> AX_INLINE
constexpr bool NumSIMD_<3, VEC, STORAGE>::operator> (const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return 0b111 ==    _mm_movemask_ps(    _mm_cmp_ps(this->_m, v._m, _CMP_GT_OQ)); } 
		if constexpr (_use_SSE_f64) { return 0b111 == _mm256_movemask_pd( _mm256_cmp_pd(this->_m, v._m, _CMP_GT_OQ)); } 
	}
	return _e0 > v._e0
		&& _e1 > v._e1
		&& _e2 > v._e2;
}

template <class VEC, class STORAGE> AX_INLINE
constexpr bool NumSIMD_<3, VEC, STORAGE>::operator<=(const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return 0b111 ==    _mm_movemask_ps(    _mm_cmp_ps(this->_m, v._m, _CMP_LE_OQ)); } 
		if constexpr (_use_SSE_f64) { return 0b111 == _mm256_movemask_pd( _mm256_cmp_pd(this->_m, v._m, _CMP_LE_OQ)); } 
	}
	return _e0 <= v._e0
		&& _e1 <= v._e1
		&& _e2 <= v._e2;
}

template <class VEC, class STORAGE> AX_INLINE
constexpr bool NumSIMD_<3, VEC, STORAGE>::operator>=(const VEC& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (_use_SSE_f32) { return 0b111 ==    _mm_movemask_ps(    _mm_cmp_ps(this->_m, v._m, _CMP_GE_OQ)); } 
		if constexpr (_use_SSE_f64) { return 0b111 == _mm256_movemask_pd( _mm256_cmp_pd(this->_m, v._m, _CMP_GE_OQ)); } 
	}
	return _e0 >= v._e0
		&& _e1 >= v._e1
		&& _e2 >= v._e2;
}

template <class VEC, class STORAGE>
template <class R_VEC, class R_STORAGE>
constexpr bool NumSIMD_<3, VEC, STORAGE>::almostEqual(const NumSIMD_<3, R_VEC, R_STORAGE>& v) const {
	if (!std::is_constant_evaluated()) {
		if constexpr (Type_IsSame<VEC, R_VEC> && Type_IsSame<STORAGE, R_STORAGE>) {
			auto diff = Math::abs(*this - v);
			return diff <= Math::epsilon<VEC>;
		}
	}
	
// for UnitTest validate between different SIMD
	return Math::almostEqual(_e0, v._e0)
		&& Math::almostEqual(_e1, v._e1)
		&& Math::almostEqual(_e2, v._e2);
}

template <class VEC, class STORAGE> AX_INLINE
constexpr VEC NumSIMD_<3, VEC, STORAGE>::abs() const {
	return VEC(	Math::abs(_e0),
				Math::abs(_e1),
				Math::abs(_e2));
}

} // namespace