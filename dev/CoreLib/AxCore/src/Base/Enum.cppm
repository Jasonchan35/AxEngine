export module AxCore.Enum;

import <type_traits>;

import "AxBase.h";

export namespace ax {

template<class T> using ax_enum_int_t = std::underlying_type_t<T>;
template<class T> AX_NODISCARD constexpr auto ax_enum_int(const T & v) { return static_cast<ax_enum_int_t<T>>(v); }

} // namespace 