#pragma once

#if AX_OS_WINDOWS

#ifndef NOMINMAX
	#define NOMINMAX 1 // no min and max macro in windows.h
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1 // Exclude rarely-used stuff from Windows headers
#endif

#ifndef NOMINMAX
	#define NOMINMAX 1 // no min and max macro in windows.h
#endif

AX_VC_WARNING_PUSH_AND_DISABLE(5039)
// warning C5039: pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. 
// Undefined behavior may occur if this function throws an exception.

#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <WS2tcpip.h> // struct sockaddr_in6
#include <windows.h>
#include <rpc.h> // uuid
#include <shellapi.h>
#include <conio.h>
#include <ShlObj_core.h> // SHGetFolderPath

AX_VC_WARNING_PUSH_AND_DISABLE(5204)
// comdef.h(489,1): warning C5204: 'Font': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from 
#include <comdef.h> //Com error
AX_VC_WARNING_POP()


#pragma comment( lib, "Rpcrt4.lib" )

AX_VC_WARNING_POP()

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

#endif // AX_OS_WINDOWS
