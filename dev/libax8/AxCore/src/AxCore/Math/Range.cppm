module;


export module AxCore.Range;
import AxCore.BasicType;
import AxCore.Math;

export namespace ax {

template<class T>
class Range_Iterator {
	using This = Range_Iterator; 
public:
	Range_Iterator(const T& current)
		: _current(current) {}

	T operator*() const { return _current; }

	Range_Iterator& operator++() {
		++_current;
		return *this;
	}

	bool operator!=(const This& other) const {
		return _current != other._current;
	}

private:
	T _current;
};


template<class T>
class Range_ {
	using This = Range_;
	T	_start = T(0);
	T	_stop  = T(0);

public:
	AX_INLINE constexpr Range_() = default;
	AX_INLINE constexpr Range_(const T& stop_) noexcept { set(T(0), stop_); }
	AX_INLINE constexpr Range_(const T& start_, const T& stop_) noexcept : _start(start_), _stop(stop_) {}

	static AX_INLINE constexpr This s_startAndSize(const T& start_, const T& size_) noexcept { return Range_(start_, start_ + size_); }

	operator Range_<const T>() const { return Range_<const T>(_start, _stop); }
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const noexcept { return _start < _stop; }

	AX_INLINE constexpr void set(const T& start_, const T& stop_ ) noexcept { _start = start_; _stop = stop_; }
	AX_INLINE constexpr void setStartAndSize(const T& start_, const T& size_) noexcept { _start = start_; _stop = start_ + size_; }
	
	AX_INLINE constexpr void reset() noexcept { *this = Range_(); }

	AX_INLINE constexpr void setStart(const T& start_) noexcept	{ _start = start_; }
	AX_INLINE constexpr void setStop (const T& stop_ ) noexcept	{ _stop   = stop_;   }
	AX_INLINE constexpr void setSize (const T& size_ ) noexcept	{ _stop   = _start + size_; }

	AX_NODISCARD AX_INLINE constexpr		T&	start()       noexcept{ return _start; }
	AX_NODISCARD AX_INLINE constexpr const	T&	start() const noexcept{ return _start; }
	AX_NODISCARD AX_INLINE constexpr		T&	stop ()       noexcept{ return _stop; }
	AX_NODISCARD AX_INLINE constexpr const	T&	stop () const noexcept{ return _stop; }
	AX_NODISCARD AX_INLINE constexpr		T	size () const noexcept{ return _stop - _start; }

	AX_NODISCARD AX_INLINE constexpr bool contains(const T& v) const noexcept		{ return v >= _start && v < _stop; }
	AX_NODISCARD AX_INLINE constexpr bool contains(const Range_& r) const noexcept	{ return r._start >= _start && r._stop <= _stop; }

	AX_NODISCARD AX_INLINE constexpr This operator|(const This& r) const noexcept	{ return _union(r); }
	AX_NODISCARD AX_INLINE constexpr This operator^(const This& r) const noexcept	{ return _intersect(r); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const T& m) const noexcept { return Range_(_start + m, _stop + m); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& m) const noexcept { return Range_(_start - m, _stop - m); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& m) const noexcept { return Range_(_start * m, _stop * m); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& m) const noexcept { return Range_(_start / m, _stop / m); }

	AX_INLINE constexpr void operator|=(const This& r) noexcept	{ *this = *this | r; }
	AX_INLINE constexpr void operator^=(const This& r) noexcept	{ *this = *this ^ r; }

	AX_INLINE constexpr void operator+=(const T& r) noexcept	{ *this = *this + r; }
	AX_INLINE constexpr void operator-=(const T& r) noexcept	{ *this = *this - r; }
	AX_INLINE constexpr void operator*=(const T& r) noexcept	{ *this = *this * r; }
	AX_INLINE constexpr void operator/=(const T& r) noexcept	{ *this = *this / r; }
	
	AX_NODISCARD AX_INLINE constexpr This alignTo(const T& alignment) const {
		return This(Math::alignDown(_start, alignment),
					Math::alignUp(    _stop, alignment));
	}

	using MIter = Range_Iterator<T>;
	using CIter = Range_Iterator<const T>;
	
	constexpr MIter begin()			{ return _start; }
	constexpr CIter begin() const	{ return _start; }
	constexpr MIter end()			{ return _stop; }
	constexpr CIter end() const		{ return _stop; }

private:
	AX_INLINE
	This _union(const This& r) const noexcept {
		auto newStart = Math::min(_start, r._start);
		auto newEnd   = Math::max(_stop,   r._stop);
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}

	AX_INLINE
	This _intersect(const This& r) const noexcept {
		auto newStart = Math::max(_start, r._start);
		auto newEnd   = Math::min(_stop,   r._stop);
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}
};

using IntRange = Range_<Int>;

template<class T>
AX_NODISCARD AX_INLINE constexpr Range_<T> Range_StartAndSize(const T& begin, const T& size) noexcept {
	return Range_<T>::s_startAndSize(begin, size);
}
AX_NODISCARD AX_INLINE constexpr IntRange IntRange_StartAndSize(const Int& begin, const Int& end) noexcept {
	return IntRange::s_startAndSize(begin, end);
}

} // namespace
