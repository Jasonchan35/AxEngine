#pragma once

#if AX_OS_WINDOWS

#if AX_USE_PRECOMPILE_HEADER

#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <WS2tcpip.h> // struct sockaddr_in6
#include <windows.h>
#include <rpc.h> // uuid
#pragma comment( lib, "Rpcrt4.lib" )


#ifdef small
#undef small // char
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef RGB
#undef RGB
#endif

#endif // else AX_USE_PRECOMPILE_HEADER

#endif // AX_OS_WINDOWS
