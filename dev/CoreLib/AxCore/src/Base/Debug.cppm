module;
#include "AxCore-pch.h"

export module AxCore.Debug;
import AxCore.BasicType;

export namespace ax::Debug {

bool isDebuggerPresent();

// for internal use, i.e. unit test cannot have high level logger functions
inline void _internal_log(const char* msg) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "libax", msg);
#else
	std::wcout << msg << std::endl;
#endif
}

// for internal use, i.e. unit test cannot have high level logger functions
inline void _internal_logError(const char* msg) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_ERROR, "libax", msg);
#else
	std::wcerr << msg;
#endif
}


void _internal_forceCrash() {
	std::cout << "ax_force_crash\n";
	*reinterpret_cast<int*>(1) = 0;
}

void _internal_assert(const char* title, const char* expr, const SrcLoc& loc, const char* msg) {
		const int bufLen = 32 * 1024;
	char buf[bufLen + 1];
	snprintf(buf, bufLen,
		"\n%s\n"
		"  Expr: %s\n"
		"  Func: %s\n"
		"Source: %s:%lld\n"
		"-------------\n"
		"%s\n",
		title,
			expr,
			loc.function().c_str(),
			loc.file().c_str(), loc.line(),
			msg);
	buf[bufLen] = 0; //snprintf might not end with zero if exists bufLen limit

#if AX_OS_WINDOWS & _DEBUG
	if (1 == _CrtDbgReport(_CRT_ASSERT, loc.file().c_str(), static_cast<int>(loc.line()), "bax", "%s", buf)) {
		_CrtDbgBreak();
	}
#else
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_ERROR, "libax", buf);
#else
	std::wcerr << msg;
#endif
	AX_ASSERT(false);
#endif
}

} // namespace
