export module AxCore.Debug;

import <iostream>;

export namespace ax::Debug {

bool isDebuggerPresent();

// for internal use, i.e. unit test cannot have high level logger functions
inline void _internal_log(const char* msg) {
#if AX_OS_ANDROID
	__android_log_write(ANDROID_LOG_INFO, "libax", msg);
#else
	std::wcout << msg;
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

} // namespace
