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

template<class T> AX_NODISCARD AX_INLINE constexpr bool ax_bit_has		(const T& value, const T& bits) { return (value & bits) != T(0); }
template<class T> AX_NODISCARD AX_INLINE constexpr bool ax_bit_has_all	(const T& value, const T& bits) { return (value & bits) == bits; }
template<class T> AX_NODISCARD AX_INLINE constexpr T    ax_bit_set		(const T& value, const T& bits) { return value |  bits; }
template<class T> AX_NODISCARD AX_INLINE constexpr T    ax_bit_unset	(const T& value, const T& bits) { return value & ~bits; }
template<class T> AX_NODISCARD AX_INLINE constexpr T    ax_bit_toggle	(const T& value, const T& bits) { return value ^  bits; }
template<class T> AX_NODISCARD AX_INLINE constexpr T    ax_bit_set		(const T& value, const T& bits, bool b) {
	return b ? ax_bit_set(value, bits) : ax_bit_unset(value, bits);
}

template<class T> AX_NODISCARD AX_INLINE constexpr Int	ax_bit_count1	(const T& value) {
	Int result = 0;
	Int n = AX_SIZEOF(T) * 8;
	T mask = T(1);
	for (Int i = 0; i < n; i++) {
		if (value & mask) result++;
		mask <<= 1;
	}
	return result;
}

template<class T> AX_NODISCARD AX_INLINE constexpr Int	ax_bit_count0	(const T& value) { return ax_bit_count1(~value); }


template<class T> AX_NODISCARD AX_INLINE constexpr Int	ax_bit_highest	(const T& value) {
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
	T mask = T(1);
	for (Int i = 0; i < n; ++i) {
		if (value & mask) result = i;
		mask <<= 1;
	}
	return result;
}

template<class T> AX_NODISCARD AX_INLINE constexpr Int	ax_bit_lowest	(const T& value) {
	Int result = -1;
	Int n = AX_SIZEOF(T) * 8;
	T mask = T(1 << (n-1));
	for (Int i = n-1; i > 0; --i) {
		if (value  & mask) result = i;
		mask >>= 1;
	}
	return result;
}

} // namespace
