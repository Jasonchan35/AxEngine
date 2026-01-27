module;

export module AxCore.NormInt; // Normalized Integer
export import AxCore.Float16;

export namespace ax {

template<class T> class NormInt_;
using UNorm8  = NormInt_<u8>;
using UNorm16 = NormInt_<u16>;
using UNorm32 = NormInt_<u32>;

using SNorm8  = NormInt_<i8>;
using SNorm16 = NormInt_<i16>;
using SNorm32 = NormInt_<i32>;

template<class T> struct NormIntTraits;
template<> struct NormIntTraits<u8> {
	using IntType                    = u8;
	using FloatType                  = f32;
	using BigIntType                 = u64;
	static constexpr IntType kIntMin = 0;
	static constexpr IntType kIntMax = UINT8_MAX;
};

template<> struct NormIntTraits<u16> {
	using IntType                    = u16;
	using FloatType                  = f32;
	using BigIntType                 = u64;
	static constexpr IntType kIntMin = 0;
	static constexpr IntType kIntMax = UINT16_MAX;
};

template<> struct NormIntTraits<u32> {
	using IntType                    = u32;
	using FloatType                  = f32;
	using BigIntType                 = u64;
	static constexpr IntType kIntMin = 0;
	static constexpr IntType kIntMax = UINT32_MAX;
};

template<> struct NormIntTraits<i8> {
	using IntType                    = i8;
	using FloatType                  = f32;
	using BigIntType                 = i64;
	static constexpr IntType kIntMin = 0;
	static constexpr IntType kIntMax = INT8_MAX;
};

template<> struct NormIntTraits<i16> {
	using IntType                    = i16;
	using FloatType                  = f32;
	using BigIntType                 = i64;
	static constexpr IntType kIntMin = 0;
	static constexpr IntType kIntMax = INT16_MAX;
};

template<> struct NormIntTraits<i32> {
	using IntType                    = i32;
	using FloatType                  = f32;
	using BigIntType                 = i64;
	static constexpr IntType kIntMin = 0;
	static constexpr IntType kIntMax = INT32_MAX;
};

// Normalized value
// unsigned int to float  0..1
// signed   int to float -1..1
template<class T>
class NormInt_ {
	using This = NormInt_;
public:
	struct _NumLimit;
	
	using Traits = NormIntTraits<T>;

	using FloatType = typename Traits::FloatType;
	using IntType   = typename Traits::IntType;
	using BigIntType = typename Traits::BigIntType;

	static constexpr IntType kIntMin = Traits::kIntMin;
	static constexpr IntType kIntMax = Traits::kIntMax;

	static constexpr BigIntType kBigIntMin = static_cast<BigIntType>(Traits::kIntMin);
	static constexpr BigIntType kBigIntMax = static_cast<BigIntType>(Traits::kIntMax);

	static constexpr FloatType kFloatMin = std::is_signed_v<IntType> ? FloatType(-1) : FloatType(0);
	static constexpr FloatType kFloatMax = FloatType(1);

	AX_INLINE constexpr NormInt_() = default;
	AX_INLINE constexpr NormInt_(IntType v) : v_int(v) {}

	AX_INLINE static constexpr This kZero() { return This(0);           }
	AX_INLINE static constexpr This kMin () { return This(kIntMin);     }
	AX_INLINE static constexpr This kMax () { return This(kIntMax);     }
	AX_INLINE static constexpr This kHalf() { return This(kIntMax / 2); }

	AX_INLINE constexpr BigIntType toBigInt() const { return static_cast<BigIntType>(v_int); }
	AX_INLINE constexpr FloatType  toFloat() const  { return toFloat_<FloatType>(); }

	AX_INLINE constexpr f16  to_f16() const { return f16(to_f32()); }
	AX_INLINE constexpr f32  to_f32() const { return static_cast<f32>(v_int) / static_cast<f32>(kIntMax); }
	AX_INLINE constexpr f64  to_f64() const { return static_cast<f64>(v_int) / static_cast<f64>(kIntMax); }

	template<class R> constexpr R toFloat_() const;
	template<> AX_INLINE constexpr f16 toFloat_<f16>() const { return to_f16(); }
	template<> AX_INLINE constexpr f32 toFloat_<f32>() const { return to_f32(); }
	template<> AX_INLINE constexpr f64 toFloat_<f64>() const { return to_f64(); }

	AX_INLINE constexpr void setFloat(FloatType f) { *this = s_fromFloat(f); }

	AX_INLINE static constexpr This s_fromFloat(f16 f) { return _s_fromFloat(f.toFloat_<FloatType>()); }
	AX_INLINE static constexpr This s_fromFloat(f32 f) { return _s_fromFloat(static_cast<FloatType>(f)); }
	AX_INLINE static constexpr This s_fromFloat(f64 f) { return _s_fromFloat(static_cast<FloatType>(f)); }

	AX_INLINE constexpr bool operator==(const This &r) const { return v_int == r.v_int; }
	AX_INLINE constexpr bool operator!=(const This &r) const { return v_int != r.v_int; }
	AX_INLINE constexpr bool operator> (const This &r) const { return v_int >  r.v_int; }
	AX_INLINE constexpr bool operator>=(const This &r) const { return v_int >= r.v_int; }
	AX_INLINE constexpr bool operator< (const This &r) const { return v_int <  r.v_int; }
	AX_INLINE constexpr bool operator<=(const This &r) const { return v_int <= r.v_int; }

	AX_INLINE constexpr This operator-() const { return -v_int; }

	AX_INLINE constexpr This operator+(const This& r) const { return _s_fromBigInt(toBigInt() + r.toBigInt()); }
	AX_INLINE constexpr This operator-(const This& r) const { return _s_fromBigInt(toBigInt() - r.toBigInt()); }
	AX_INLINE constexpr This operator* (const This& r) const { return _s_fromFloat(toFloat() * r.toFloat()); }
	AX_INLINE constexpr This operator/ (const This& r) const { return _s_fromFloat(toFloat() / r.toFloat()); }

	AX_INLINE constexpr void operator+=(const This& r) { *this = *this + r; }
	AX_INLINE constexpr void operator-=(const This& r) { *this = *this + r; }
	AX_INLINE constexpr void operator*=(const This& r) { *this = *this * r; }
	AX_INLINE constexpr void operator/=(const This& r) { *this = *this / r; }

	template<class SE> constexpr void onJsonIO_Value(SE& se) { se.io(v_int); }
	template<class CH> void onFmt(Format_<CH>& ctx) const { ctx << v_int; }

	IntType	v_int;

private:
	AX_INLINE static constexpr This _s_fromBigInt(BigIntType v) {
		return This(static_cast<IntType>(Math::clamp(v, kBigIntMin, kBigIntMax)));
	}

	AX_INLINE static constexpr This _s_fromFloat(FloatType f) {
		auto v = Math::round(Math::clamp(f, kFloatMin, kFloatMax) * static_cast<FloatType>(kIntMax));
		return This(static_cast<IntType>(v));
	}
};

template<class T> struct Type_FloatTypeFrom_T< NormInt_<T> >  { using Type = typename NormInt_<T>::FloatType; };

template<class T> struct IsNormInt_Helper : std::false_type {};
template<class T> struct IsNormInt_Helper< NormInt_<T> > : std::true_type {};

template<class T> struct NormInt_<T>::_NumLimit : public NumLimit<T> {};

} // namespace ax


