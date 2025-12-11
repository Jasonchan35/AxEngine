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

	// please use s_beginEnd() or s_beginSize()
	AX_INLINE constexpr Range_(const T& begin_, const T& end_) noexcept : _begin(begin_), _end(end_) {}
public:

	operator Range_<const T>() const { return Range_<const T>(_begin, _end); }

	static AX_INLINE constexpr This s_beginEnd (const T& begin_, const T& end_ ) noexcept { return Range_(begin_, end_); }
	static AX_INLINE constexpr This s_beginSize(const T& begin_, const T& size_) noexcept { return Range_(begin_, begin_ + size_); }
	
	AX_INLINE constexpr Range_() = default;
	AX_INLINE constexpr Range_(const T& end_) noexcept { setBeginEnd(0, end_); }
	
//	AX_INLINE constexpr Range_(const T& begin_, const T& size_) { set(begin_, size_); }
	
	AX_INLINE constexpr void setBeginEnd (const T& begin_, const T& end_ ) noexcept { _begin = begin_; _end = end_; }
	AX_INLINE constexpr void setBeginSize(const T& begin_, const T& size_) noexcept { _begin = begin_; _end = begin_ + size_; }
	
	AX_INLINE constexpr void reset() noexcept { *this = Range_(); }

	AX_NODISCARD AX_INLINE constexpr void	setBegin(const T& newBegin) noexcept	{ _begin = newBegin; }
	AX_NODISCARD AX_INLINE constexpr void	setEnd	(const T& newEnd  ) noexcept	{ _end   = newEnd;   }
	AX_NODISCARD AX_INLINE constexpr void	setSize	(const T& newSize ) noexcept	{ _end   = _begin + newSize; }

	AX_NODISCARD AX_INLINE constexpr		T&	begin()       noexcept{ return _begin; }
	AX_NODISCARD AX_INLINE constexpr const	T&	begin() const noexcept{ return _begin; }
	AX_NODISCARD AX_INLINE constexpr		T&	end  ()       noexcept{ return _end; }
	AX_NODISCARD AX_INLINE constexpr const	T&	end  () const noexcept{ return _end; }
	AX_NODISCARD AX_INLINE constexpr		T	size () const noexcept{ return _end - _begin; }
	
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const noexcept { return _begin < _end; }

	AX_NODISCARD AX_INLINE constexpr bool contains(const T& v) const noexcept		{ return v >= _begin && v < _end; }
	AX_NODISCARD AX_INLINE constexpr bool contains(const Range_& r) const noexcept	{ return r._begin >= _begin && r._end <= _end; }

	AX_NODISCARD AX_INLINE constexpr This operator|(const This& r) const noexcept	{ return _union(r); }
	AX_NODISCARD AX_INLINE constexpr This operator&(const This& r) const noexcept	{ return _intersect(r); }

				 AX_INLINE constexpr void operator|=(const This& r) noexcept	{ *this = _union(r); }
				 AX_INLINE constexpr void operator&=(const This& r) noexcept	{ *this = _intersect(r); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const T& m) const noexcept { return Range_(_begin + m, _end + m); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& m) const noexcept { return Range_(_begin - m, _end - m); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& m) const noexcept { return Range_(_begin * m, _end * m); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& m) const noexcept { return Range_(_begin / m, _end / m); }

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

template<class T>
class RangeEx_  {
	using This = RangeEx_;
	T	_begin	= 0;
	T	_end	= 0;
	T	_step	= 0;

	// please use s_beginAndEnd() or s_beginAndSize()
	AX_INLINE constexpr RangeEx_(const T& begin_, const T& end_) noexcept;
public:
	
};

using IntRange = Range_<Int>;

} // namespace
