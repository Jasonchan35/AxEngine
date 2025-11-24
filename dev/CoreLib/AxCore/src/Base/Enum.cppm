module;
#include "AxBase.h"

export module AxCore.Enum;

export namespace ax {

template<class T> using ax_enum_int_t = std::underlying_type_t<T>;
template<class T> AX_NODISCARD constexpr auto ax_enum_int(const T & v) { return static_cast<ax_enum_int_t<T>>(v); }

} // namespace 