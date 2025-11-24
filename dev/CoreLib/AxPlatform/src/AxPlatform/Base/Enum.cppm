module;
#include "AxPlatform-pch.h"

export module AxPlatform.Enum;
export import AxPlatform.BasicType; 

export namespace ax {

template<class T> using ax_enum_int_t = std::underlying_type_t<T>;
template<class T> AX_NODISCARD constexpr auto ax_enum_int(const T & v) { return static_cast<ax_enum_int_t<T>>(v); }

} // namespace 