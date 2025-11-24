module;
#include "AxPlatform-pch.h"

module AxPlatform.ThreadUtil;

namespace ax::Thread {

bool ThreadId::isCurrentThread() {
	return *this == s_current();
}

#if AX_OS_WINDOWS

ThreadId ThreadId::s_current() {
	ThreadId o;
	auto h =  ::GetCurrentThread();
	o._v = ThreadId_Native(h, GetThreadId(h));
	return o;
}

#elif AX_OS_MACOSX || AX_OS_IOS

ThreadId ThreadId::s_current() {
	ThreadId o;
	o._v = pthread_self();
	return o;
}

#else

ThreadId ThreadId::s_current() {
	ThreadId o;
	o._v = pthread_self();
	return o;
}

#endif

} // namespace