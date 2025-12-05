module;
#include "AxCore-pch.h"

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

	operator Range_<const T>() const { return Range_<const T>(_begin, _end); }

	AX_INLINE constexpr Range_() = default;
	AX_INLINE constexpr Range_(const T& begin_, const T& end_) { set(begin_, end_); }
	
	AX_INLINE constexpr void set(const T& begin_, const T& end_) { _begin = begin_; _end = end_; }
	AX_INLINE constexpr void reset() { set(0,0); }

	AX_NODISCARD AX_INLINE constexpr T	size() const	{ return _end - _begin; }
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const { return _begin < _end; }

	AX_NODISCARD AX_INLINE constexpr bool contains(const T& v) const		{ return v >= _begin && v < _end; }
	AX_NODISCARD AX_INLINE constexpr bool contains(const Range_& r) const	{ return r._begin >= _begin && r._end <= _end; }

	AX_NODISCARD AX_INLINE constexpr This operator|(const This& r) const	{ return _union(r); }
	AX_NODISCARD AX_INLINE constexpr This operator&(const This& r) const	{ return _intersect(r); }

	AX_NODISCARD AX_INLINE constexpr void operator|=(const This& r)	{ *this = _union(r); }
	AX_NODISCARD AX_INLINE constexpr void operator&=(const This& r)	{ *this = _intersect(r); }

	AX_NODISCARD AX_INLINE constexpr This operator+(const T& m) const { return Range_(_begin + m, _end + m); }
	AX_NODISCARD AX_INLINE constexpr This operator-(const T& m) const { return Range_(_begin - m, _end - m); }
	AX_NODISCARD AX_INLINE constexpr This operator*(const T& m) const { return Range_(_begin * m, _end * m); }
	AX_NODISCARD AX_INLINE constexpr This operator/(const T& m) const { return Range_(_begin / m, _end / m); }

	AX_NODISCARD AX_INLINE constexpr		T& begin() 			{ return _begin; }
	AX_NODISCARD AX_INLINE constexpr const	T& begin() const	{ return _begin; }

	AX_NODISCARD AX_INLINE constexpr		T& end() 			{ return _end; }
	AX_NODISCARD AX_INLINE constexpr const	T& end() const		{ return _end; }
	
private:
	AX_INLINE
	This _union(const This& r) const {
		auto newStart = Math::min(_begin, r._begin);
		auto newEnd   = Math::max(_end,   r._end);
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}

	AX_INLINE
	This _intersect(const This& r) const {
		auto newStart = Math::max(_begin, r._begin);
		auto newEnd   = Math::min(_end,   r._end);
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}
};

using IntRange = Range_<Int>;


} // namespace
