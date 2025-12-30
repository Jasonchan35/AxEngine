module;


export module AxCore.TimeDuration;

export import AxCore.BasicType;

export namespace ax {

template<class T, i64 RATIO> class TimeDuration_;
using Seconds		= TimeDuration_<i64, 1>;
using Milliseconds	= TimeDuration_<i64, 1000>;
using Microseconds	= TimeDuration_<i64, 1000000>;
using Nanoseconds	= TimeDuration_<i64, 1000000000>;

using Seconds_f32	= TimeDuration_<f32, 1>;
using Seconds_f64	= TimeDuration_<f64, 1>;

template<class T, i64 RATIO>
class TimeDuration_ {
	using This = TimeDuration_;
public:
	static constexpr i64 kRatio = RATIO;

	T	value = 0;

	// explicit to avoid implicit int to value
	constexpr explicit TimeDuration_(T v = T(0)) noexcept : value(v) {}

	constexpr TimeDuration_(const TimeDuration_&) noexcept = default;

	template<i64 R>
	constexpr TimeDuration_(const TimeDuration_<T, R>& v) noexcept { convert(v); }

	AX_INLINE static constexpr This kZero() { return This(0); }
	AX_INLINE static constexpr This kMax()  { return This(NumLimit<T>::max()); }

	static constexpr This s_f32(f32 s) noexcept { return This(static_cast<T>(s * static_cast<f32>(RATIO))); }
	static constexpr This s_f64(f64 s) noexcept { return This(static_cast<T>(s * static_cast<f64>(RATIO))); }

	constexpr f32 seconds_f32() const noexcept { return static_cast<f32>(value) / static_cast<f32>(RATIO); }
	constexpr f64 seconds_f64() const noexcept { return static_cast<f64>(value) / static_cast<f64>(RATIO); }

	constexpr This operator+ (const This& r) const noexcept { return This(value + r.value); }
	constexpr This operator- (const This& r) const noexcept { return This(value - r.value); }
	constexpr This operator* (const This& r) const noexcept { return This(value * r.value); }
	constexpr This operator/ (const This& r) const noexcept { return This(value / r.value); }
	constexpr This operator% (const This& r) const noexcept { return This(value % r.value); }

	constexpr This operator+ (const T& r) const noexcept { return This(value + r); }
	constexpr This operator- (const T& r) const noexcept { return This(value - r); }
	constexpr This operator* (const T& r) const noexcept { return This(value * r); }
	constexpr This operator/ (const T& r) const noexcept { return This(value / r); }
	constexpr This operator% (const T& r) const noexcept { return This(value % r); }

	constexpr void operator= (const This& r) noexcept { value = r.value; }

	constexpr bool operator==(const This& r) const { return value == r.value; }
	constexpr bool operator!=(const This& r) const { return value != r.value; }
	constexpr bool operator> (const This& r) const { return value >  r.value; }
	constexpr bool operator>=(const This& r) const { return value >= r.value; }
	constexpr bool operator< (const This& r) const { return value <  r.value; }
	constexpr bool operator<=(const This& r) const { return value <= r.value; }

	template<i64 R>
	constexpr void convert(const TimeDuration_<T, R>& v) noexcept {
		if constexpr (RATIO == R) {
			value = v.value;

		} else if constexpr (RATIO > R) {
			value = v.value * (RATIO / R);

		} else if constexpr (RATIO < R) {
			value = v.value / (R / RATIO);

		} else {
			static_assert(false);
		}
	}

};

} // namespace