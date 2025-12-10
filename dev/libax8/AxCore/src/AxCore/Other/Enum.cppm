module;
#include "AxCore-pch.h"

export module AxCore.Enum;
export import AxCore.StrView;

export namespace ax {

template<class T> requires Type_IsEnum<T>
class EnumFn_ {
public:
	using IntType = Type_EnumInt<T>;

	AX_INLINE constexpr static T s_fromInt(IntType const& v) { return static_cast<T>(v); }
	
	AX_INLINE constexpr	EnumFn_(T& v) : value(v) {}

	AX_INLINE constexpr operator		T&()		{ return value; }
	AX_INLINE constexpr operator const	T&() const	{ return value; }
	
	AX_INLINE constexpr void	setToZero	()			{ value = static_cast<T>(0); }
	AX_INLINE constexpr bool	isZero		() const	{ return ax_enum_int(value) == 0; }

	AX_INLINE constexpr void 	setFlags	(const T& f) { value = static_cast<T>(ax_enum_int(value) |  ax_enum_int(f)); }
	AX_INLINE constexpr void 	unsetFlags	(const T& f) { value = static_cast<T>(ax_enum_int(value) & ~ax_enum_int(f)); }
	AX_INLINE constexpr void 	toggleFlags	(const T& f) { value = static_cast<T>(ax_enum_int(value) ^  ax_enum_int(f)); }

	AX_INLINE constexpr bool	hasFlags	(const T& f) { return ax_enum_int(f) == (ax_enum_int(value) & ax_enum_int(f)); }
	AX_INLINE constexpr bool	hasAnyFlags	(const T& f) { return IntType(0) != (ax_enum_int(value) & ax_enum_int(f)); }

	AX_INLINE constexpr			IntType& toInt()		{ return static_cast<IntType>(value); }

	AX_INLINE constexpr void	setInt(const IntType & r) { value = static_cast<T>(r); }
	
	AX_INLINE constexpr void	operator=	(AxTag::Zero_) { setToZero(); }

	AX_INLINE constexpr StrView	str() const { return _ax_macro_enum_str(value); }
	AX_INLINE constexpr bool	tryParse(StrView view) { return _ax_macro_enum_try_parse(view, value); };

	AX_INLINE constexpr bool operator==(const T& r) const { return r == value; }

	template<class SE> void onJsonIO_Value(SE& se) { IntType tmp; se.io(tmp); setInt(tmp); }

	T& value;
};

template<class T> constexpr EnumFn_<T> EnumFn(T& v) { static_assert(std::is_enum_v<T>); return EnumFn_<T>(v); }

} // namespace