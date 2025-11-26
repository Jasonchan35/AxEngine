module;
#include "AxCore-pch.h"
export module AxCore.Num;
export import AxCore._PCH;

export namespace ax {

#define AX_NumSIMD_ENUM_LIST(E) \
	E(NoSIMD,) \
	E(SSE,)  \
//---
AX_ENUM_CLASS(AX_NumSIMD_ENUM_LIST, CpuSIMD, u8)

template<Int N, class VEC, class STORAGE> class Num_;
template<       class VEC, class STORAGE> using Num1_  = Num_<1, VEC, STORAGE>;
template<       class VEC, class STORAGE> using Num2_  = Num_<2, VEC, STORAGE>;
template<       class VEC, class STORAGE> using Num3_  = Num_<3, VEC, STORAGE>;
template<       class VEC, class STORAGE> using Num4_  = Num_<4, VEC, STORAGE>;

template<Int N, class T, CpuSIMD SIMD> class Num_Storage;

template<class OUT_CLS, class T>
struct NumBase_NumLimit {
	using T_NumLimit = NumLimit<T>;

	static constexpr bool isExactType       =  T_NumLimit::isExactType;
	static constexpr bool hasInfinity       =  T_NumLimit::hasInfinity;
	static constexpr OUT_CLS  infinity      =  OUT_CLS(Tag::All, T_NumLimit::infinity);
	static constexpr OUT_CLS  negInfinity   =  OUT_CLS(Tag::All, T_NumLimit::negInfinity);
	static constexpr OUT_CLS  lowest        =  OUT_CLS(Tag::All, T_NumLimit::lowest);
	static constexpr OUT_CLS  min           =  OUT_CLS(Tag::All, T_NumLimit::min);
	static constexpr OUT_CLS  max           =  OUT_CLS(Tag::All, T_NumLimit::max);
	static constexpr OUT_CLS  epsilon       =  OUT_CLS(Tag::All, T_NumLimit::epsilon);
	static constexpr OUT_CLS  NaN           =  OUT_CLS(Tag::All, T_NumLimit::NaN);
};


template<class VEC, class STORAGE>
class Num_<4, VEC, STORAGE> : public STORAGE {
	using This = Num_;
	using T = typename STORAGE::Element;
public:
	using _NumLimit = typename NumBase_NumLimit<This, T>;
	using Element = T;
	using Storage = STORAGE;
	static constexpr Int elementCount = STORAGE::elementCount;
	static constexpr CpuSIMD cpuSIMD  = STORAGE::cpuSIMD;
protected:
	using Storage::_data;
	using Storage::_e0;
	using Storage::_e1;
	using Storage::_e2;
	using Storage::_e3;
	
	static constexpr bool _use_f32     = Type_IsSame<T, f32>;
	static constexpr bool _use_f64     = Type_IsSame<T, f64>;
	static constexpr bool _use_SSE     = cpuSIMD == CpuSIMD::SSE;
	static constexpr bool _use_SSE_f32 = _use_SSE && _use_f32;
	static constexpr bool _use_SSE_f64 = _use_SSE && _use_f64;
public:
	using CFixedSpan =    FixedSpan<T, elementCount>;
	using MFixedSpan = MutFixedSpan<T, elementCount>;
	AX_NODISCARD AX_INLINE constexpr CFixedSpan fixedSpan() const { return CFixedSpan(_data); }
	AX_NODISCARD AX_INLINE constexpr MFixedSpan fixedSpan()       { return MFixedSpan(_data); }

	AX_NODISCARD AX_INLINE constexpr Num_() = default;
	AX_NODISCARD AX_INLINE constexpr Num_(const Storage & storage_) : Storage(storage_) {}
	AX_NODISCARD AX_INLINE constexpr Num_(const T& e0, const T& e1, const T& e2, const T& e3) : Storage(e0, e1, e2, e3) {}
	AX_NODISCARD AX_INLINE constexpr Num_(Tag::All_, const T& v_) : Storage(v_, v_, v_, v_) {}

	const T& unsafe_at(Int i) const { return _data[i]; }
	
	AX_INLINE constexpr void setAll(const T& v_) {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32) {
				this->_m = _mm_set1_ps(v_); return;
			} else if constexpr (_use_f64) {
				this->_m = _mm256_set1_pd(v_); return;
			}
		}
		_e0 = v_; _e1 = v_; _e2 = v_; _e3 = v_;
	}
	
	AX_NODISCARD AX_INLINE constexpr VEC operator+(const VEC& v) const {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32) return Storage(   _mm_add_ps(this->_m, v._m));
			if constexpr (_use_SSE_f64) return Storage(_mm256_add_pd(this->_m, v._m)); 
		}
		return VEC(_e0 + v._e0, _e1 + v._e1, _e2 + v._e2, _e3 + v._e3);
	}
	AX_NODISCARD AX_INLINE constexpr VEC operator-(const VEC& v) const {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32) return Storage(   _mm_sub_ps(this->_m, v._m));
			if constexpr (_use_SSE_f64) return Storage(_mm256_sub_pd(this->_m, v._m));
		}
		return VEC(_e0 - v._e0, _e1 - v._e1, _e2 - v._e2, _e3 - v._e3);
	}
	
	AX_NODISCARD AX_INLINE constexpr bool operator<=(const VEC& v) const {
		return _e0 <= v._e0 && _e1 <= v._e1 && _e2 <= v._e2 && _e3 <= v._e3;
	}

	AX_NODISCARD AX_INLINE constexpr VEC abs() const {
		return VEC(Math::abs(_e0), Math::abs(_e1), Math::abs(_e2), Math::abs(_e3));
	}

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const {
		fmt << Fmt("({},{},{},{})", _e0, _e1, _e2, _e3);
	}
};

//----------------


} // namespace
