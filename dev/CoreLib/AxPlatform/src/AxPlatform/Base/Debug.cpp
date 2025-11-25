module;

#include "AxPlatform-pch.h"
#include "AxPlatform/Base/AX_WINDOWS.h"

module AxPlatform.Debug;

namespace ax::Debug {

bool isDebuggerPresent() {
#if AX_OS_WINDOWS
	return ::IsDebuggerPresent();
#else
	return false;
#endif
}

} // namespace
