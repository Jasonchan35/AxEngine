module;

#include "AxCore-pch.h"
export module AxCore._PCH;

#if !AX_USE_PRECOMPILE_HEADER

export import <algorithm>;
export import <bit>;
export import <cctype>;
export import <cfloat>;
export import <cstdint>;
export import <cstdlib>;
export import <exception>;
export import <format>;
export import <iostream>;
export import <memory>;
export import <mutex>;
export import <optional>;
export import <random>;
export import <source_location>;
export import <stdexcept>;
export import <string_view>;
export import <thread>;
export import <tuple>;
export import <type_traits>;
export import <utility>;

#if AX_OS_WINDOWS
export import <WinSock2.h>; // WinSock2.h must include before windows.h to avoid winsock1 define
export import <WS2tcpip.h>; // struct sockaddr_in6
export import <windows.h>;
export import <conio.h>;

#else
export import <uuid/uuid.h>;
#endif

#endif // AX_USE_PRECOMPILE_HEADER


