module;
#include "AxCore-pch.h"

export module AxCore.Float16;
export import AxCore.BasicMath;
export import AxCore.Formatter;

export namespace ax {

struct Float16;
using f16 = Float16;

struct Float16 {
	using This = Float16;
public:
	struct _NumLimit;
	
	enum class hdata : u16 {
		// set(float) require c++20 std::bit_cast() for constexpr, therefore using pre-computed int
		Zero = 0,
		One  = 0x3c00,
		Half = 0x3800,
		Epsilon = 0x14, // 0.0009765625f
	};

	Float16() = default;
	constexpr Float16(float  f) : _v(hdata::Zero) { set(f); }
	constexpr Float16(hdata  v) : _v(v) {}

	constexpr void operator=(float  f) { set(f); }

	constexpr void set(float  v);

	AX_INLINE static constexpr This s_epsilon()		{ return This(hdata::Epsilon); }
	AX_INLINE static constexpr This s_zero()		{ return This(hdata::Zero   ); }
	AX_INLINE static constexpr This s_one ()		{ return This(hdata::One    ); }
	AX_INLINE static constexpr This s_half()		{ return This(hdata::Half   ); }

	AX_INLINE constexpr bool operator==(const This & r) const { return _v == r._v; }
	AX_INLINE constexpr bool operator!=(const This & r) const { return _v != r._v; }

	AX_INLINE constexpr bool operator>=(const This & r) const { return to_f32() >= to_f32(); }
	AX_INLINE constexpr bool operator> (const This & r) const { return to_f32() >  to_f32(); }
	AX_INLINE constexpr bool operator<=(const This & r) const { return to_f32() <= to_f32(); }
	AX_INLINE constexpr bool operator< (const This & r) const { return to_f32() <  to_f32(); }

	AX_INLINE constexpr This operator-() const { return s_make(-to_f32()); }

	AX_INLINE constexpr This operator+(const This& r) const { return s_make(to_f32() + r.to_f32()); }
	AX_INLINE constexpr This operator-(const This& r) const { return s_make(to_f32() - r.to_f32()); }
	AX_INLINE constexpr This operator*(const This& r) const { return s_make(to_f32() * r.to_f32()); }
	AX_INLINE constexpr This operator/(const This& r) const { return s_make(to_f32() / r.to_f32()); }

	AX_INLINE constexpr void  operator+=(const This& r) { *this = *this + r; }
	AX_INLINE constexpr void  operator-=(const This& r) { *this = *this - r; }
	AX_INLINE constexpr void  operator*=(const This& r) { *this = *this * r; }
	AX_INLINE constexpr void  operator/=(const This& r) { *this = *this / r; }

	AX_INLINE constexpr f32 to_f32() const;
	AX_INLINE constexpr f64 to_f64() const { return static_cast<f64>(to_f32()); }

	template<class R>
	AX_INLINE constexpr   R toFloat_() const { return static_cast<R>(to_f32()); }
	AX_INLINE constexpr f32 toFloat() const  { return to_f32(); }

	AX_INLINE static constexpr This s_fromFloat(f16 v) { return This(v); }
	AX_INLINE static constexpr This s_fromFloat(f32 v) { return This(v); }
	AX_INLINE static constexpr This s_fromFloat(f64 v) { return This(static_cast<f32>(v)); }

	AX_INLINE static constexpr This s_make(f32 v) { return This(v); }
	
	template<class SE> constexpr void onJsonIO_Value(SE& se) {
		if constexpr (se.isReader()) {
			float tmp = 0; se.io(tmp); set(tmp);
		} else {
			float tmp = toFloat(); se.io(tmp);
		}
	}

	AX_INLINE constexpr bool almostEqual(const This& r) const { return Math::almostEqual(to_f32(), r.to_f32()); }
	template<class F > constexpr void onFormat(F & f) const { f << Fmt("{}", to_f32()); }
	template<class CH> constexpr bool onStrParse(StrView_<CH> str) {
		f32 tmp;
		if (!str.tryParse(tmp)) return false;
		set(tmp);
		return true;
	}
	
	constexpr hdata value() const { return _v; }
private:
	hdata _v; // = 0; <-- want to keep "trivial default constructor"

	constexpr u16 v_int() const { return static_cast<u16>(_v); }

	static constexpr i32 _s_bitcast_i32(float v) { return ax_bit_cast<i32>(v); }
	static constexpr f32 _s_bitcast_f32(i32   v) { return ax_bit_cast<f32>(v); }

//	explicit Float16(hdata v) : _v(v) {};
	static constexpr float s_overflow();
};

constexpr Float16 Float16_make(float  v) { return Float16::s_make(v); }
constexpr Float16 f16_make(float  v) { return Float16::s_make(v); }

constexpr void Float16::set(float f) {
	i32 i = _s_bitcast_i32(f);

	//
	// Our floating point number, f, is represented by the bit
	// pattern in integer i.  Disassemble that bit pattern into
	// the sign, s, the exponent, e, and the significand, m.
	// Shift s into the position where it will go in in the
	// resulting half number.
	// Adjust e, accounting for the different exponent bias
	// of float and half (127 versus 15).
	//

	int s =  (i >> 16) & 0x00008000;
	int e = ((i >> 23) & 0x000000ff) - (127 - 15);
	int m =   i        & 0x007fffff;

	//
	// Now reassemble s, e and m into a half:
	//

	if(e <= 0) {
		if(e < -10) {
			//
			// E is less than -10.  The absolute value of f is
			// less than half_MIN (f may be a small normalized
			// float, a denormalized float or a zero).
			//
			// We convert f to a half zero.
			//
			_v = hdata(s);
			return;
		}
		//
		// E is between -10 and 0.  F is a normalized float,
		// whose magnitude is less than __half_NRM_MIN.
		//
		// We convert f to a denormalized half.
		// 

		m = (m | 0x00800000) >> (1 - e);

		// Round to nearest, round "0.5" up.
		//
		// Rounding may cause the significand to overflow and make
		// our number normalized.  Because of the way a half's bits
		// are laid out, we don't have to treat this case separately;
		// the code below will handle it correctly.

		if(m & 0x00001000) 
			m += 0x00002000;

		// Assemble the half from s, e (zero) and m.
		_v = hdata(s | (m >> 13));
		return;

	} else if(e == 0xff - (127 - 15)) {
		if(m == 0) {
			// F is an infinity; convert f to a half
			// infinity with the same sign as f.
			_v = hdata(s | 0x7c00);
			return;
		} else {
			// F is a NAN; we produce a half NAN that preserves
			// the sign bit and the 10 leftmost bits of the
			// significand of f, with one exception: If the 10
			// leftmost bits are all zero, the NAN would turn 
			// into an infinity, so we have to set at least one
			// bit in the significand.
			m >>= 13;
			_v = hdata(s | 0x7c00 | m | (m == 0));
			return;
		}
	}
	else
	{
		// E is greater than zero.  F is a normalized float.
		// We try to convert f to a normalized half.
		//
		// Round to nearest, round "0.5" up
		if(m &  0x00001000) {
			m += 0x00002000;
			if(m & 0x00800000) {
				m =  0;     // overflow in significand,
				e += 1;     // adjust exponent
			}
		}

		// Handle exponent overflow
		if (e > 30) {
			s_overflow();        // Cause a hardware floating point overflow;

			_v = hdata(s | 0x7c00);
			return;
			// if this returns, the half becomes an
		}   // infinity with the same sign as f.

		//
		// Assemble the half from s, e and m.
		//

		_v = hdata(s | (e << 10) | (m >> 13));
		return;
	}
}

constexpr float Float16::to_f32() const {
	auto v = v_int();
	int s = (v >> 15) & 0x00000001;
	int e = (v >> 10) & 0x0000001f;
	int m =  v        & 0x000003ff;

	if(e == 0) {
		if (m == 0) {
			// Plus or minus zero
			return _s_bitcast_f32(s << 31);
		} else {
			// Denormalized number -- renormalize it
			while(!(m & 0x00000400)) {
				m <<= 1;
				e -=  1;
			}
			e += 1;
			m &= ~0x00000400;
		}
	} else if(e == 31) {
		if (m == 0) {
			// Positive or negative infinity
			return _s_bitcast_f32((s << 31) | 0x7f800000);
		} else {
			// Nan -- preserve sign and significand bits
			return _s_bitcast_f32((s << 31) | 0x7f800000 | (m << 13));
		}
	}

	// Normalized number
	e = e + (127 - 15);
	m = m << 13;

	// Assemble s, e and m.
	return _s_bitcast_f32((s << 31) | (e << 23) | m);
}

#pragma warning(push)
#pragma warning(disable : 4756) // warning C4756: overflow in constant arithmetic

constexpr float Float16::s_overflow() {
	float f = 1e10;
	for (int i = 0; i < 10; ++i)
		f *= f; // this will overflow before the for loop terminates
	return f;
}

#pragma warning(pop)

struct Float16::_NumLimit {
	static constexpr bool isExact = false;
	
	// static constexpr f16    lowest         =  std::numeric_limits<f16>::lowest();
	// static constexpr f16    min            =  std::numeric_limits<f16>::min();
	// static constexpr f16    max            =  std::numeric_limits<f16>::max();
		static constexpr f16    epsilon       () { return f16::s_epsilon(); }
	// TODO
	// static constexpr f16    NaN            =  std::numeric_limits<f16>::quiet_NaN();
	// static constexpr bool   hasInfinity    =  std::numeric_limits<f16>::has_infinity;
	// static constexpr f16    infinity       =  std::numeric_limits<f16>::infinity();
	// static constexpr f16    negInfinity    = -std::numeric_limits<f16>::infinity();
	//
	// static constexpr f16    minExponent    =  std::numeric_limits<f16>::min_exponent;
	// static constexpr f16    maxExponent    =  std::numeric_limits<f16>::max_exponent;
	// static constexpr f16    minExponent10  =  std::numeric_limits<f16>::min_exponent10;
	// static constexpr f16    maxExponent10  =  std::numeric_limits<f16>::max_exponent10;
};

} // namespace ax
