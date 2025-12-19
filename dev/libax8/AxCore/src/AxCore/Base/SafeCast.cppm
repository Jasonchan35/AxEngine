module;

export module AxCore.SafeCast;
export import AxCore.Error;

export namespace ax {

template<class DST, class SRC> AX_INLINE
constexpr Opt<DST> ax_try_safe_cast_(const SRC& src) noexcept {
	if constexpr (Type_IsSame<std::remove_cv_t<DST>, std::remove_cv_t<SRC>>) {
		return src;
		
	} else if constexpr (std::is_floating_point_v<SRC> && std::is_floating_point_v<DST>) {
		// float -> float
		return static_cast<DST>(src);
		
	} else if constexpr (std::is_integral_v<SRC> && std::is_floating_point_v<DST>) {
		// int -> float
		return static_cast<DST>(src);
		
	} else if constexpr (std::is_floating_point_v<SRC> && std::is_integral_v<DST>) {
		// float -> int
		using DST_LIMIT = std::numeric_limits<DST>;
		using FLOAT_TYPE = SRC;
		if (src < static_cast<FLOAT_TYPE>(DST_LIMIT::lowest())) { AX_ASSERT_MSG(false, "safe_cast"); return std::nullopt; }
		if (src > static_cast<FLOAT_TYPE>(DST_LIMIT::max()   )) { AX_ASSERT_MSG(false, "safe_cast"); return std::nullopt; }
		return static_cast<DST>(src);
		
	} else {
		// enum/int -> enum/int
		using DST_INT = Type_IntOrEnumInt<DST>;
		using SRC_INT = Type_IntOrEnumInt<SRC>;
		
		if (!std::in_range<DST_INT>(static_cast<SRC_INT>(src))) { AX_ASSERT_MSG(false, "safe_cast"); return std::nullopt; }
		return static_cast<DST>(src);
	}
}

template<class DST, class SRC> constexpr
DST ax_safe_cast_to(const SRC& src) {
	if constexpr (Type_IsSame<std::remove_cv_t<DST>, std::remove_cv_t<SRC>>) {
		return src;

	} else {
		if (auto result = ax_try_safe_cast_<DST>(src)) {
			return result.value();
		}
		throw Error_SafeCast();
	}
}

template<class SRC>
struct ax_safe_cast_from {
	const SRC& src;
	constexpr ax_safe_cast_from(const SRC& src_) : src(src_) {}
	template <typename DST>	constexpr operator DST() const { return ax_safe_cast_to<DST>(src); }
};

template<class SRC>
struct ax_static_cast_from {
	SRC& src;
	constexpr ax_static_cast_from(SRC& src_) : src(src_) {}
	template <typename DST>	constexpr operator DST() const { return static_cast<DST>(src); }
};

template<class SRC>
struct ax_reinterpret_cast_from {
	SRC& src;
	constexpr ax_reinterpret_cast_from(SRC& src_) : src(src_) {}
	template <typename DST>	constexpr operator DST() const { return reinterpret_cast<DST>(src); }
};





} // namespace