module;

export module AxCore.BitFn;
export import AxCore.BasicType;

export namespace ax {

// this cast can over come function pointer to void*
template<class DST, class SRC> AX_INLINE
constexpr DST ax_bit_cast(const SRC& src) {
	// std::bit_cast needs c++20, so use union work around
	union Wrap {
		constexpr Wrap(const SRC& src_) : src(src_) {}
		DST dst;
		SRC src;
	};
	static_assert(sizeof(DST) == sizeof(SRC));
	return Wrap(src).dst;
}

template<class T>
struct BitFn {
	T& value;
	BitFn(T& v) : value(v) {}
	
	AX_INLINE constexpr	bool	hasAny	(const T& bits) const	{ return (value & bits) != T(0); }
	AX_INLINE constexpr	bool	hasAll	(const T& bits) const	{ return (value & bits) == bits; }
	AX_INLINE constexpr void	set		(const T& bits, bool b)	{ b ? set(bits) : unset(bits); }
	AX_INLINE constexpr void	set		(const T& bits)			{ value |=  bits; }
	AX_INLINE constexpr void	unset	(const T& bits)			{ value &= ~bits; }
	AX_INLINE constexpr void	toggle	(const T& bits)			{ value ^=  bits; }

	AX_INLINE constexpr Int		count0	() const { return _count1(~value); }
	AX_INLINE constexpr Int		count1	() const { return _count1(value);  }
	AX_INLINE constexpr Int		highest	() const { return _highest(); }
	AX_INLINE constexpr Int		lowest	() const { return _lowest();  }

private:
	AX_INLINE static constexpr Int	_count1	(const T& v) {
		Int result = 0;
		Int n = AX_SIZEOF(T) * 8;
		T m = T(1);
		for (Int i = 0; i < n; i++) {
			if (m & v) result++;
			m <<= 1;
		}
		return result;
	}

	AX_INLINE constexpr Int	_highest() const {
#if AX_COMPILER_VC
		if constexpr (std::is_same_v<T, i32>() || std::is_same_v<T, u32>()) {
			unsigned int index = 0;
			if (!_BitScanForward(&index, value))
				return -1;
			return static_cast<Int>(index);
		}
		if constexpr (std::is_same_v<T, i64>() || std::is_same_v<T, u64>()) {
			unsigned int index = 0;
			if (!_BitScanForward64(&index, value))
				return -1;
			return static_cast<Int>(index);
		}
#endif

#if AX_COMPILER_GCC
		if constexpr (std::is_same_v<T, i32>() || std::is_same_v<T, u32>()) {
			int r = __builtin_ffs(value);
			if (r == 0) return -1;
			return static_cast<Int>(r - 1);
		}
		if constexpr (std::is_same_v<T, i64>() || std::is_same_v<T, u64>()) {
			int r = __builtin_ffsl(value);
			if (r == 0) return -1;
			return static_cast<Int>(r - 1);
		}
#endif
		Int result = -1;
		Int n = AX_SIZEOF(T) * 8;
		T m = T(1);
		for (Int i = 0; i < n; ++i) {
			if (hasAny(m)) result = i;
			m <<= 1;
		}
		return result;
	}

	AX_INLINE constexpr Int	_lowest() const {
		Int result = -1;
		Int n = AX_SIZEOF(T) * 8;
		T m = T(1 << (n-1));
		for (Int i = n-1; i > 0; --i) {
			if (hasAny(m)) result = i;
			m >>= 1;
		}
		return result;
	}	
};

} // namespace
