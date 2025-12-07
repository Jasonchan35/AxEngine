module;
#include "AxCore-pch.h"

export module AxCore.Enum;
export import AxCore.StrView;

export namespace ax {

template<class T> requires Type_IsEnum<T>
class EnumFn {
public:
	using IntType = Type_EnumInt<T>;

	AX_INLINE constexpr static T s_fromInt(IntType const& v) { return static_cast<T>(v); }
	
	AX_INLINE constexpr	EnumFn(T& v) : value(v) {}

	AX_INLINE constexpr operator		T&()		{ return value; }
	AX_INLINE constexpr operator const	T&() const	{ return value; }
	
	AX_INLINE constexpr void	setToZero	()			{ value = static_cast<T>(0); }
	AX_INLINE constexpr bool	isZero		() const	{ return ax_enum_int(value) == 0; }

	AX_INLINE constexpr void 	setFlags	(const T& f) { value = static_cast<T>(ax_enum_int(value) |  ax_enum_int(f)); }
	AX_INLINE constexpr void 	unsetFlags	(const T& f) { value = static_cast<T>(ax_enum_int(value) & ~ax_enum_int(f)); }
	AX_INLINE constexpr void 	toggleFlags	(const T& f) { value = static_cast<T>(ax_enum_int(value) ^  ax_enum_int(f)); }

	AX_INLINE constexpr bool	hasAllFlags	(const T& f) { return ax_enum_int(f) == (ax_enum_int(value) & ax_enum_int(f)); }
	AX_INLINE constexpr bool	hasAnyFlags	(const T& f) { return IntType(0) != (ax_enum_int(value) & ax_enum_int(f)); }

	AX_INLINE constexpr			IntType& asInt()		{ return static_cast<      IntType&>(value); }
	AX_INLINE constexpr const	IntType& asInt() const	{ return static_cast<const IntType&>(value); }
	
	AX_INLINE constexpr void	operator=	(const IntType& r) { setInt(r); }
	AX_INLINE constexpr void	operator=	(AX_ZERO_) { setToZero(); }

	AX_INLINE constexpr StrView	str() const { return enumStr(value); }

	AX_INLINE constexpr bool operator==(const T& r) const { return r == value; }

	template<class SE> void onJsonIO_Value(SE& se) { se.io(asInt()); }

	T& value;
};

} // namespace