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

template<Int N, class OUT_CLS, class STORAGE> class NumBase_;
template<       class OUT_CLS, class STORAGE> using Num1_  = NumBase_<1, OUT_CLS, STORAGE>;
template<       class OUT_CLS, class STORAGE> using Num2_  = NumBase_<2, OUT_CLS, STORAGE>;
template<       class OUT_CLS, class STORAGE> using Num3_  = NumBase_<3, OUT_CLS, STORAGE>;
template<       class OUT_CLS, class STORAGE> using Num4_  = NumBase_<4, OUT_CLS, STORAGE>;

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

template<class OUT_CLS, class STORAGE>
class NumBase_<4, OUT_CLS, STORAGE> : public STORAGE {
	using This = NumBase_;
	using OutClass = OUT_CLS;
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

	AX_NODISCARD AX_INLINE constexpr NumBase_() = default;
	AX_NODISCARD AX_INLINE constexpr NumBase_(const Storage & storage_) : Storage(storage_) {}
	AX_NODISCARD AX_INLINE constexpr NumBase_(Tag::All_, const T& v_) : Storage(v_, v_, v_, v_) {}

	AX_NODISCARD AX_INLINE constexpr NumBase_(const T& e0, const T& e1, const T& e2, const T& e3) : Storage(e0, e1, e2, e3) {}

	AX_INLINE constexpr void setAll(const T& v_);
	AX_NODISCARD AX_INLINE constexpr OutClass operator+(const OutClass& v) const {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32) {
				return Storage(_mm_add_ps(this->_m, v._m));
			}
		}
		return OutClass(_e0 + v._e0, _e1 + v._e1, _e2 + v._e2, _e3 + v._e3);
	}
	AX_NODISCARD AX_INLINE constexpr OutClass operator-(const OutClass& v) const {
		if (!std::is_constant_evaluated()) {
			if constexpr (_use_SSE_f32) {
				return Storage(_mm_sub_ps(this->_m, v._m));
			}
		}
		return OutClass(_e0 - v._e0, _e1 - v._e1, _e2 - v._e2, _e3 - v._e3);
	}
	
	AX_NODISCARD AX_INLINE constexpr bool operator<=(const OutClass& v) const {
		return _e0 <= v._e0 && _e1 <= v._e1 && _e2 <= v._e2 && _e3 <= v._e3;
	}

	AX_NODISCARD AX_INLINE constexpr OutClass abs() const {
		return OutClass(Math::abs(_e0), Math::abs(_e1), Math::abs(_e2), Math::abs(_e3));
	}

	template<class FMT_CH>
	void onFormat(Format_<FMT_CH> & fmt) const {
		fmt << Fmt("({},{},{},{})", _e0, _e1, _e2, _e3);
	}
};

//----------------


} // namespace
