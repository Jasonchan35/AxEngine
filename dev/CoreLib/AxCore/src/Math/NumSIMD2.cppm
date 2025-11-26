module;
#include "AxCore-pch.h"
export module AxCore.NumSIMD2;
export import AxCore.NumSIMD1;

export namespace ax {

template<class VEC, class STORAGE>
class NumSIMD_<2, VEC, STORAGE> : public STORAGE {
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
	
	static constexpr bool _use_SSE     = cpuSIMD == CpuSIMD::SSE;
	static constexpr bool _use_SSE_f32 = _use_SSE && Type_IsSame<T, f32>;
	static constexpr bool _use_SSE_f64 = _use_SSE && Type_IsSame<T, f64>;

	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_NODISCARD AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_data); }
	AX_NODISCARD AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_data); }

	AX_NODISCARD AX_INLINE constexpr NumSIMD_() = default;
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(const Storage & storage_) : Storage(storage_) {}
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(const T& e0, const T& e1) : Storage(e0, e1) {}
	AX_NODISCARD AX_INLINE constexpr NumSIMD_(Tag::All_, const T& v) : Storage(s_all(v)) {}

	      Storage&	asStorage() { return *this; }
	const Storage&	asStorage() const { return *this; }
	
	AX_INLINE constexpr       T& unsafe_at(Int i)       { return _data[i]; }
	AX_INLINE constexpr const T& unsafe_at(Int i) const { return _data[i]; }

	AX_NODISCARD static AX_INLINE constexpr VEC s_all (const T& v) { return Storage(v,v); }
	AX_NODISCARD static AX_INLINE constexpr VEC s_zero(const T& v) { return s_all(0); }
	AX_NODISCARD static AX_INLINE constexpr VEC s_one (const T& v) { return s_all(1); }

	AX_INLINE constexpr void setAll(const T& v) { *this = s_all(v); }
	
	AX_NODISCARD AX_INLINE constexpr VEC operator+(const VEC& v) const { return VEC(_e0 + v._e0, _e1 + v._e1); }
	AX_NODISCARD AX_INLINE constexpr VEC operator-(const VEC& v) const { return VEC(_e0 - v._e0, _e1 - v._e1); }
	AX_NODISCARD AX_INLINE constexpr VEC operator*(const VEC& v) const { return VEC(_e0 * v._e0, _e1 * v._e1); }
	AX_NODISCARD AX_INLINE constexpr VEC operator/(const VEC& v) const { return VEC(_e0 / v._e0, _e1 / v._e1); }

	AX_NODISCARD AX_INLINE constexpr VEC operator+(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator-(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator*(const T& v) const { return *this + VEC::s_all(v);  }
	AX_NODISCARD AX_INLINE constexpr VEC operator/(const T& v) const { return *this + VEC::s_all(v);  }
	
	AX_INLINE constexpr void operator+=(const VEC& v) { *this = *this + v; }
	AX_INLINE constexpr void operator-=(const VEC& v) { *this = *this - v; }
	AX_INLINE constexpr void operator*=(const VEC& v) { *this = *this * v; }
	AX_INLINE constexpr void operator/=(const VEC& v) { *this = *this / v; }

	AX_NODISCARD AX_INLINE constexpr bool operator< (const VEC& v) const { return _e0 <  v._e0 && _e1 <  v._e1; }
	AX_NODISCARD AX_INLINE constexpr bool operator> (const VEC& v) const { return _e0 >  v._e0 && _e1 >  v._e1; }
	AX_NODISCARD AX_INLINE constexpr bool operator<=(const VEC& v) const { return _e0 <= v._e0 && _e1 <= v._e1; }
	AX_NODISCARD AX_INLINE constexpr bool operator>=(const VEC& v) const { return _e0 >= v._e0 && _e1 >= v._e1; }

	template<class R, class R_STORAGE>
	AX_NODISCARD constexpr bool almostEqual(const NumSIMD_<2, R, R_STORAGE>& v) const {
		return Math::almostEqual(_e0, v._e0) && Math::almostEqual(_e1, v._e1);
	}

	AX_NODISCARD constexpr VEC abs() const { return VEC(Math::abs(_e0), Math::abs(_e1)); }

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const {
		fmt << Fmt("({},{})", _e0, _e1);
	}
};


} // namespace