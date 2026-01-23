module;

#include "half-2.2.1/include/half.hpp"

export module AxCore.Float16;
export import AxCore.BasicMath;
export import AxCore.Formatter;

export namespace ax {

struct Float16;
using f16 = Float16;

struct Float16 {
	using This = Float16;
	using half = half_float::half;
	
	static constexpr auto binTag = half_float::detail::binary;
	static constexpr unsigned int kBin_Zero = 0;
	static constexpr unsigned int kBin_One  = 0x3c00;
	static constexpr unsigned int kBin_Half = 0x3800;
	static constexpr unsigned int kBin_Two  = 0x4000;

public:
	struct _NumLimit;
	
	Float16() = default;
	explicit Float16(float  f) : _v(f) {}

	AX_INLINE static constexpr This s_epsilon()		{ return This(std::numeric_limits<half>::epsilon()); }
	AX_INLINE static constexpr This s_zero()		{ return half(binTag, kBin_Zero); }
	AX_INLINE static constexpr This s_half()		{ return half(binTag, kBin_Half); }
	AX_INLINE static constexpr This s_one ()		{ return half(binTag, kBin_One); }
	AX_INLINE static constexpr This s_two()			{ return half(binTag, kBin_Two); }

	AX_INLINE constexpr bool operator==(const This & r) const { return _v == r._v; }
	AX_INLINE constexpr bool operator!=(const This & r) const { return _v != r._v; }

	AX_INLINE constexpr bool operator>=(const This & r) const { return _v >= r._v; }
	AX_INLINE constexpr bool operator> (const This & r) const { return _v >  r._v; }
	AX_INLINE constexpr bool operator<=(const This & r) const { return _v <= r._v; }
	AX_INLINE constexpr bool operator< (const This & r) const { return _v <  r._v; }

	AX_INLINE constexpr This operator-() const { return -_v; }

	AX_INLINE constexpr This operator+(const This& r) const { return _v + r._v; }
	AX_INLINE constexpr This operator-(const This& r) const { return _v - r._v; }
	AX_INLINE constexpr This operator*(const This& r) const { return _v * r._v; }
	AX_INLINE constexpr This operator/(const This& r) const { return _v / r._v; }

	AX_INLINE constexpr void  operator+=(const This& r) { *this = *this + r; }
	AX_INLINE constexpr void  operator-=(const This& r) { *this = *this - r; }
	AX_INLINE constexpr void  operator*=(const This& r) { *this = *this * r; }
	AX_INLINE constexpr void  operator/=(const This& r) { *this = *this / r; }

	AX_INLINE f32 to_f32() const { return _v; }
	AX_INLINE f64 to_f64() const { return static_cast<f64>(to_f32()); }

	template<class R>
	AX_INLINE    R toFloat_() const { return static_cast<R>(to_f32()); }
	AX_INLINE  f32 toFloat() const  { return to_f32(); }

	AX_INLINE static This s_fromFloat(f16 v) { return This(v); }
	AX_INLINE static This s_fromFloat(f32 v) { return This(v); }
	AX_INLINE static This s_fromFloat(f64 v) { return This(static_cast<f32>(v)); }

	template<class SE> constexpr void onJsonIO_Value(SE& se) {
		if constexpr (se.isReader()) {
			float tmp = 0; se.io(tmp); *this = Float16(tmp);
		} else {
			float tmp = toFloat(); se.io(tmp);
		}
	}

	
	AX_INLINE constexpr This abs() const { return *this >= s_zero() ? *this : -*this; }
	AX_INLINE constexpr bool almostEqual(const This& r, const This& epsilon = s_epsilon()) const {
		return Math::abs(*this - r) <= epsilon;
	}
	
	template<class F > constexpr void onFormat(F & f) const { f << Fmt("{}", to_f32()); }
	template<class CH> constexpr bool onStrParse(StrView_<CH> str) {
		f32 tmp;
		if (!str.tryParse(tmp)) return false;
		*this = Float16(tmp);
		return true;
	}

private:
	half _v;
	constexpr Float16(half v) : _v(v) {}
};

struct Float16::_NumLimit {
	static constexpr bool isExact = false;
	
	static constexpr f16    lowest         =  std::numeric_limits<f16>::lowest();
	static constexpr f16    min            =  std::numeric_limits<f16>::min();
	static constexpr f16    max            =  std::numeric_limits<f16>::max();
	static constexpr f16    epsilon() { return f16::s_epsilon(); }
	
	static constexpr f16    NaN            =  std::numeric_limits<f16>::quiet_NaN();
	static constexpr bool   hasInfinity    =  std::numeric_limits<f16>::has_infinity;
	static constexpr f16    infinity       =  std::numeric_limits<f16>::infinity();
	static constexpr f16    negInfinity    = -std::numeric_limits<f16>::infinity();
	
//	static constexpr f16    minExponent    =  std::numeric_limits<f16>::min_exponent;
//	static constexpr f16    maxExponent    =  std::numeric_limits<f16>::max_exponent;
//	static constexpr f16    minExponent10  =  std::numeric_limits<f16>::min_exponent10;
//	static constexpr f16    maxExponent10  =  std::numeric_limits<f16>::max_exponent10;
};

} // namespace ax
