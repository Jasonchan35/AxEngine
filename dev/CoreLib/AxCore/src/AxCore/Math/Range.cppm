module;
#include "AxCore-pch.h"

export module AxCore.Range;
import AxCore.BasicType;
import AxCore.Math;

export namespace ax {

template<class T>
class Range_ {
	using This = Range_;
public:
	T	start = 0;
	T	size  = 0;

	AX_INLINE constexpr Range_() = default;
	AX_INLINE constexpr Range_(const T& start_, const T& size_) { set(start_, size_); }

	AX_INLINE constexpr void set(const T& newStart, const T& newSize) {
		start = newStart;
		size  = newSize;
	}
	
	AX_INLINE constexpr void reset() { set(0,0); }

	AX_INLINE constexpr explicit operator bool() const { return size > 0; }

	AX_INLINE constexpr void setMin(const T& newMin) { auto e = end(); start = newMin; setEnd(e); }
	AX_INLINE constexpr void setEnd(const T& newEnd) { size = newEnd - start; }

	AX_INLINE constexpr void trimStart(const T& n) { T t = Math::max_0(size - n); start -= t; size += t; }

	AX_INLINE constexpr bool contains(const Range_& r) const { return r.start >= start && r.end() <= end(); }

	AX_INLINE constexpr This operator|(const This& r) const	{ return _union(r); }
	AX_INLINE constexpr This operator&(const This& r) const	{ return _intersect(r); }

	AX_INLINE constexpr void operator|=(const This& r)	{ *this = _union(r); }
	AX_INLINE constexpr void operator&=(const This& r)	{ *this = _intersect(r); }

	AX_NODISCARD constexpr This mul(const T& m) { return Range_(start * m, size * m); }
	AX_NODISCARD constexpr This div(const T& m) { return Range_(start / m, size / m); }

	T begin() const	{ return start; }
	T end() const	{ return start + size; }

private:
	AX_INLINE
	This _union(const This& r) const {
		auto newStart = Math::min(start, r.start);
		auto newEnd   = Math::max(end(), r.end());
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}

	AX_INLINE
	This _intersect(const This& r) const {
		auto newStart = Math::max(start, r.start);
		auto newEnd   = Math::min(end(),  r.end());
		if (newEnd <= newStart)	return This();
		return This(newStart, newEnd - newStart);
	}
};

using IntRange = Range_<Int>;

} // namespace
