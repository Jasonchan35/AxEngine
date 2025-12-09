#pragma once

#if AX_OS_WINDOWS
	#include "AxCore-OS_Windows.h"

	#define AX_APP_MAIN(T) \
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPWSTR lpCmdLine, int nCmdShow) { return ::ax::App_run<T>(0, nullptr); } \
	int main(int argc, const char* argv[]) { return ::ax::App_run<T>(argc, argv); } \
	// //--------

#else
	#define AX_APP_MAIN(T) \
	int main(int argc, const char* argv[]) { return ::ax::App_run<T>(argc, argv); } \
	//--------
	
#endif
