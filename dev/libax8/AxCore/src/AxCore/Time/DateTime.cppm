module;
#include "AxCore-pch.h"

export module AxCore.DateTime;
export import AxCore.TimeStamp;

export namespace ax {


#define AX_Weekday_ENUM_LIST(E) \
	E(Sunday,) \
	E(Monday,) \
	E(Tuesday,) \
	E(Wednesday,) \
	E(Thursday,) \
	E(Friday,) \
	E(Saturday,) \
//----
AX_ENUM_CLASS(AX_Weekday_ENUM_LIST, Weekday, i8)

class DateTime {
public:
	static const Int kMinToSec	= 60;
	static const Int kHourToSec	= 60 * kMinToSec;
	static const Int kDayToSec	= 24 * kHourToSec;
	static const Int kWeekToSec	= 7  * kDayToSec;

	void reset() { _t = {}; }

	DateTime() = default;
	explicit DateTime(const LocalTime& t) { set(t); }
	explicit DateTime(const UtcTime&   t) { set(t); }

	void set(const LocalTime& t);
	void set(const UtcTime& t);

	void setToNow()					{ set(LocalTime::s_now()); }

	static	DateTime	s_now()		{ return DateTime(LocalTime::s_now()); }

	LocalTime	toLocalTime() const;
	UtcTime		toUtcTime() const;

	Int		year() const			{ return _t.tm_year + 1900; }
	void	setYear(Int v)			{ _t.tm_year = static_cast<int>(v) - 1900; }
	
	Int		month() const			{ return _t.tm_mon + 1; }
	void	setMonth(Int v)			{ _t.tm_mon = static_cast<int>(v) - 1; }
	
	Int		day() const				{ return _t.tm_mday; }
	void	setDay(Int v)			{ _t.tm_mday = static_cast<int>(v); }
	
	Int		hour() const			{ return _t.tm_hour; }
	void	setHour(Int v)			{ _t.tm_hour = static_cast<int>(v); }
	
	Int		minute() const			{ return _t.tm_min; }
	void	setMinute(Int v)		{ _t.tm_min = static_cast<int>(v); }
	
	Int		second() const			{ return _t.tm_sec; }
	void	setSecond(Int v)		{ _t.tm_sec = static_cast<int>(v); }
	
	bool 	isDaylightSavingTime	() const { return _t.tm_isdst != 0; }
	void	setIsDaylightSavingTime	(bool b) { _t.tm_isdst = b ? 1 : 0; }

	//!days since Sunday - [0,6]
	Weekday	weekday() const { return static_cast<Weekday>(_t.tm_wday); }

	void setWeekday(Weekday w) { _t.tm_wday = ax_enum_int(w); }

	void setDate(Int y, Int m, Int d) { setYear(y); setMonth(m);  setDay(d);    }
	void setTime(Int h, Int m, Int s) { setHour(h); setMinute(m); setSecond(s); }

	Int	yearInDays() const { return _t.tm_yday; }

	void addSeconds	(Int sec ) { set(toUtcTime() + Nanoseconds(sec)); }
	void addMinutes	(Int min ) { addSeconds(min  * kMinToSec); }
	void addHours	(Int hour) { addSeconds(hour * kHourToSec); }
	void addDays	(Int day ) { addSeconds(day  * kDayToSec); }
	void addWeeks	(Int week) { addSeconds(week * kWeekToSec); }

	bool tryParse(StrView str);
	void parse(StrView str) { if (!tryParse(str)) throw Error_ParseString(); }

	template<class CH>
	void onFormat(Format_<CH>& ctx) const {
		auto tmp = Fmt("{:04}-{:02}-{:02} {:02}:{:02}:{:02}"
						, year(), month(),  day()
						, hour(), minute(), second());
		ctx << tmp;
	}

private:
	struct tm _t = {};
	Nanoseconds _nanoSec{0};
};


void DateTime::set(const LocalTime& ts) {
	time_t tmp = Seconds(ts.nanoseconds()).value;
#if AX_OS_WINDOWS
	_nanoSec = ts.nanoseconds() % 1000000000LL;

	if (::gmtime_s(&_t, &tmp) != 0) {
		throw Error_Time(Fmt("errno={}", errno));
	}

#elif AX_COMPILER_GCC | AX_COMPILER_CLANG

	if (::gmtime_r(&tmp, &_t) == nullptr)
		throw Error_Undefined();

#else
#error "Unsupported compiler"
#endif
}

void DateTime::set(const UtcTime& ts) {
	time_t tmp = Seconds(ts.nanoseconds()).value;
#if AX_OS_WINDOWS
	_nanoSec = ts.nanoseconds() % 1000000000LL;

	if (::gmtime_s(&_t, &tmp) != 0) {
		throw Error_Time();
	}

#elif AX_COMPILER_GCC | AX_COMPILER_CLANG

	if (::gmtime_r(&tmp, &_t) == nullptr)
		throw Error_Undefined();

#else
#error "Unsupported compiler"
#endif
}

LocalTime DateTime::toLocalTime() const {
	//	auto t = ::mktime(ax_const_cast(&_t)); // seconds since 1970
#if AX_OS_WINDOWS
	auto t = ::_mkgmtime(ax_const_cast(&_t)); // seconds since 1970
#else
	auto t = ::timegm(ax_const_cast(&_t));
#endif

	auto ts = LocalTime(Nanoseconds(t * 1000000000LL)) + Nanoseconds(_nanoSec);
	return ts;
}

UtcTime DateTime::toUtcTime() const {
#if AX_OS_WINDOWS
	auto t = ::_mkgmtime(ax_const_cast(&_t)); // seconds since 1970
#else
	auto t = ::timegm(ax_const_cast(&_t));
#endif
	auto ts = UtcTime(Nanoseconds(t * 1000000000LL)) + Nanoseconds(_nanoSec);
	return ts;
}

bool DateTime::tryParse(StrView str) {
	int Y,M,D;
	int h,m;
	double s;

#if AX_OS_WINDOWS
	int ret = _snscanf_s(str.data(), SafeCast(str.size()), "%d-%d-%d %d:%d:%lf", &Y, &M, &D, &h, &m, &s);
#else
	TempString tmpStr(str);
	int ret = sscanf(tmpStr.c_str(), "%d-%d-%d %d:%d:%lf", &Y, &M, &D, &h, &m, &s);
#endif

	if( ret != 6 )
		return false;

	setYear(Y);
	setMonth(M);
	setDay(D);

	setHour(h);
	setMinute(m);
	setSecond(static_cast<Int>(s));

	// to get weekday
	auto tmp = toLocalTime();
	set(tmp);

	return true;
}


} // namespace 