module;


export module AxCore.Range;
import AxCore.BasicType;
import AxCore.Math;

export namespace ax {

template<class T>
class Range_ {
	using This = Range_;
	T	_begin = 0;
	T	_end   = 0;

public:
	AX_INLINE constexpr Range_() = default;
	AX_INLINE constexpr Range_(const T& end_) noexcept { set(0, end_); }
	AX_INLINE constexpr Range_(const T& begin_, const T& end_) noexcept : _begin(begin_), _end(end_) {}

	static AX_INLINE constexpr This s_beginSize(const T& begin_, const T& size_) noexcept { return Range_(begin_, begin_ + size_); }

	operator Range_<const T>() const { return Range_<const T>(_begin, _end); }
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const noexcept { return _begin < _end; }

	AX_INLINE constexpr void set(const T& begin_, const T& end_ ) noexcept { _begin = begin_; _end = end_; }
	AX_INLINE constexpr void setBeginSize(const T& begin_, const T& size_) noexcept { _begin = begin_; _end = begin_ + size_; }
	
	AX_INLINE constexpr void reset() noexcept { *this = Range_(); }

	AX_INLINE constexpr void setBegin(const T& begin_) noexcept	{ _begin = begin_; }
	AX_INLINE constexpr void setEnd	 (const T& end_  ) noexcept	{ _end   = end_;   }
	AX_INLINE constexpr void setSize (const T& size_ ) noexcept	{ _end   = _begin + size_; }

	AX_NODISCARD AX_INLINE constexpr		T&	begin()       noexcept{ return _begin; }
	AX_NODISCARD AX_INLINE constexpr const	T&	begin() const noexcept{ return _begin; }
	AX_NODISCARD AX_INLINE constexpr		T&	end  ()       noexcept{ return _end; }
	AX_NODISCARD AX_INLINE constexpr const	T&	end  () const noexcept{ return _end; }
	AX_NODISCARD AX_INLINE constexpr		T	size () const noexcept{ return _end - _begin; }

	AX_NODISCARD AX_INLINE constexpr bool contains(const T& v) const noexcept		{ return v >= _begin && v < _end; }
	AX_NODISCARD AX_INLINE constexpr bool contains(const Range_& r) const noexcept	{ return r._begin >= _begin && r._end <= _end; }

	AX_NODISCARD AX_INLINE constexpr This operator|(const This& r) const noexcept	{ return _union(r); }
	AX_NODISCARD AX_INLINE constexpr This operator^(const This& r) const noexcept	{ return _intersect(r); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const T& m) const noexcept { return Range_(_begin + m, _end + m); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& m) const noexcept { return Range_(_begin - m, _end - m); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& m) const noexcept { return Range_(_begin * m, _end * m); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& m) const noexcept { return Range_(_begin / m, _end / m); }

	AX_INLINE constexpr void operator|=(const This& r) noexcept	{ *this = *this | r; }
	AX_INLINE constexpr void operator^=(const This& r) noexcept	{ *this = *this ^ r; }

	AX_INLINE constexpr void operator+=(const T& r) noexcept	{ *this = *this + r; }
	AX_INLINE constexpr void operator-=(const T& r) noexcept	{ *this = *this - r; }
	AX_INLINE constexpr void operator*=(const T& r) noexcept	{ *this = *this * r; }
	AX_INLINE constexpr void operator/=(const T& r) noexcept	{ *this = *this / r; }
	
	
	AX_NODISCARD AX_INLINE constexpr This alignTo(const T& alignment) const {
		return This(Math::alignDown(_begin, alignment),
					Math::alignUp(    _end, alignment));
	}

private:
	AX_INLINE
	This _union(const This& r) const noexcept {
		auto newStart = Math::min(_begin, r._begin);
		auto newEnd   = Math::max(_end,   r._end);
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}

	AX_INLINE
	This _intersect(const This& r) const noexcept {
		auto newStart = Math::max(_begin, r._begin);
		auto newEnd   = Math::min(_end,   r._end);
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}
};

using IntRange = Range_<Int>;

template<class T>
AX_NODISCARD AX_INLINE constexpr Range_<T> Range_BeginSize(const T& begin, const T& size) noexcept {
	return Range_<T>::s_beginSize(begin, size);
}
AX_NODISCARD AX_INLINE constexpr IntRange IntRange_BeginSize(const Int& begin, const Int& end) noexcept {
	return IntRange::s_beginSize(begin, end);
}

} // namespace
