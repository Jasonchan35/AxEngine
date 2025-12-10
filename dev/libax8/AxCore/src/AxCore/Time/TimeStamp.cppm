module;


export module AxCore.TimeStamp;
export import AxCore.TimeDuration;
export import AxCore.Formatter;

export namespace ax {


template<class CLOCK>
class Timestamp_ {
	using This = Timestamp_;
public:
	using Clock = CLOCK;

	static_assert(std::is_same_v<typename CLOCK::duration::rep, i64>);
	static_assert(std::is_same_v<typename CLOCK::duration, std::chrono::nanoseconds>);

	Timestamp_() = default;
	constexpr Timestamp_(Seconds      v) : _value(v) {}
	constexpr Timestamp_(Milliseconds v) : _value(v) {}
	constexpr Timestamp_(Microseconds v) : _value(v) {}
	constexpr Timestamp_(Nanoseconds  v) : _value(v) {}

			  static Timestamp_	s_now()  noexcept { return CLOCK::s_now(); }
	constexpr static Timestamp_	s_zero() noexcept { return Nanoseconds(0); }

	static const Int kMinToSec	= 60;
	static const Int kHourToSec	= 60 * kMinToSec;
	static const Int kDayToSec	= 24 * kHourToSec;
	static const Int kWeekToSec	= 7  * kDayToSec;

	constexpr static Timestamp_	s_date_1970_to_2000() noexcept { return Nanoseconds((365*30+7) * kDayToSec * 1000000000LL); } // 1970 year to 2000 year

	static Timestamp_ s_timezone() noexcept;

	constexpr Nanoseconds	value() const { return _value; }
	constexpr operator Nanoseconds() const { return _value; }

			  void			setToNow		() noexcept		{ *this = s_now(); }
	constexpr void			set(Nanoseconds	v) noexcept		{ _value = v; }
	constexpr Nanoseconds	nanoseconds() const	noexcept	{ return _value; }
	constexpr f32			seconds_f32() const	noexcept	{ return _value.seconds_f32(); }
	constexpr f64			seconds_f64() const	noexcept	{ return _value.seconds_f64(); }

	constexpr bool operator==(const This& r) const noexcept { return _value == r._value; }
	constexpr bool operator!=(const This& r) const noexcept { return _value != r._value; }
	constexpr bool operator< (const This& r) const noexcept { return _value <  r._value; }
	constexpr bool operator<=(const This& r) const noexcept { return _value <= r._value; }
	constexpr bool operator> (const This& r) const noexcept { return _value >  r._value; }
	constexpr bool operator>=(const This& r) const noexcept { return _value >= r._value; }

	constexpr This operator+ (const This& r) const noexcept { return _value + r._value; }
	constexpr This operator- (const This& r) const noexcept { return _value - r._value; }

	void operator+=(const This& r) noexcept { *this = *this + r; }
	void operator-=(const This& r) noexcept { *this = *this - r; }

	template<class CH>
	void onFormat(Format_<CH>& ctx) const { ctx << Fmt("{}s", seconds_f64()); }

private:
	Nanoseconds	_value;
};

template<class CLOCK>
Timestamp_<CLOCK> Timestamp_<CLOCK>::s_timezone() noexcept {
#if AX_OS_WINDOWS
	FILETIME utc = {}, local = {};
	FileTimeToLocalFileTime (&utc, &local);
	i64 u = ( static_cast<i64>(local.dwHighDateTime) << 32) | local.dwLowDateTime; // u is 100-nanosecond
	return Nanoseconds(u * 100);
#else
	struct timeval	tv = {};
	struct timezone	z  = {};
	gettimeofday( &tv, &z );
	return Nanoseconds(z.tz_minuteswest * 60 * 1000000000LL);
#endif
}

struct LocalClock {
	LocalClock() = delete;

	using duration = std::chrono::nanoseconds;
	using time_point = std::chrono::time_point<LocalClock>;

	static Nanoseconds s_now() {
		auto t = std::chrono::system_clock::now().time_since_epoch() + s_timezone();
		return Nanoseconds(std::chrono::duration_cast<duration>(t).count());
	}

	static duration s_timezone() {
	#if AX_OS_WINDOWS
		FILETIME utc = {}, local = {};
		FileTimeToLocalFileTime (&utc, &local);
		i64 u = (static_cast<i64>(local.dwHighDateTime) << 32) | local.dwLowDateTime; // u is 100-nanosecond
		return duration(u * 100);
	#else
		struct timeval	tv = {};
		struct timezone	z  = {};
		gettimeofday( &tv, &z );
		return duration(z.tz_minuteswest * 60 * 1000000000LL);
	#endif
	}
};

struct UtcClock {
	UtcClock() = delete;

	using duration = std::chrono::nanoseconds;
	using time_point = std::chrono::time_point<UtcClock>;

	static Nanoseconds s_now() {
		auto t = std::chrono::system_clock::now().time_since_epoch();
		return Nanoseconds(std::chrono::duration_cast<duration>(t).count());
	}
};

struct HiResClock {
	HiResClock() = delete;

	using duration = std::chrono::nanoseconds;
	using time_point = std::chrono::time_point<HiResClock>;

	static Nanoseconds s_now() {
		auto t = std::chrono::high_resolution_clock::now().time_since_epoch();
		return Nanoseconds(std::chrono::duration_cast<duration>(t).count());
	}
};

using UtcTime   = Timestamp_<UtcClock>;
using LocalTime = Timestamp_<LocalClock>;
using HiResTime = Timestamp_<HiResClock>;

inline LocalTime UtcToLocalTime(const UtcTime& utc) {
	LocalTime o = utc.nanoseconds();
	o += LocalTime::s_timezone();
	return o;
}

inline UtcTime LocalToUtcTime(const LocalTime& ts) {
	UtcTime o = ts.nanoseconds();
	o -= UtcTime::s_timezone();
	return o;
}

#if AX_OS_WINDOWS

inline UtcTime UtcTime_make(const FILETIME &ft) {
	i64 u = (static_cast<i64>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime; // based on 1601-01-01
	u -= 116444736000000000; // based on 1970-01-01
	return Nanoseconds(u * 100);
}

inline FILETIME FILETIME_make(const UtcTime& t) {
	i64 u = t.nanoseconds().value / 100;
	u += 116444736000000000; // based on 1970-01-01
	
	FILETIME	ft;
	ft.dwHighDateTime = static_cast<DWORD>(u >> 32);
	ft.dwLowDateTime  = static_cast<DWORD>(u);
	return ft;
}

#endif // AX_OS_WINDOWS

inline UtcTime UtcTime_make(const time_t& t) {
	return Seconds(t);
}

inline UtcTime UtcTime_make(const timespec & t) {
	i64 v = static_cast<i64>(t.tv_sec) * 1000000000 + t.tv_nsec;
	return Nanoseconds(v);
}

inline timespec timespec_make(const UtcTime& t) {
	i64 i = t.nanoseconds().value;
	timespec o;
	o.tv_sec  = static_cast<time_t>(i / 1000000000LL);
	o.tv_nsec = static_cast<long  >(i % 1000000000LL);
	return o;
}

inline UtcTime UtcTime_make(const timeval & t) {
	i64 v = static_cast<i64>(t.tv_sec)  * 1000000000LL + static_cast<i64>(t.tv_usec) * 1000LL;
	return Nanoseconds(v);
}

inline timeval timeval_make(const UtcTime& t) {
	i64 i = t.nanoseconds().value;
	timeval	o;
	o.tv_sec  = static_cast<decltype(o.tv_sec)>(  i / 1000000000LL);
	o.tv_usec = static_cast<decltype(o.tv_usec)>((i % 1000000000LL) / 1000LL);
	return o;
}

#if AX_OS_MACOSX || AX_OS_IOS

inline UtcTime UtcTime_make(NSDate* ns) {
	UtcTime o;
	o.setSecondsInFloat64([ns timeIntervalSince1970]);
	return o;
}

#endif


} // namespace