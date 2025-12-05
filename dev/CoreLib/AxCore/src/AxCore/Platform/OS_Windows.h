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


#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <WS2tcpip.h> // struct sockaddr_in6
#include <windows.h>
#include <rpc.h> // uuid
#include <shellapi.h>
#include <conio.h>
#include <comdef.h> //Com error
#include <ShlObj_core.h> // SHGetFolderPath

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

#endif // AX_OS_WINDOWS
