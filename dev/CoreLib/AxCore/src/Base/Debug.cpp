module AxCore.Debug;

namespace ax::Debug {

bool isDebuggerPresent() {
#if AX_OS_WINDOWS
	return ::IsDebuggerPresent();
#else
	return false;
#endif
}

} // namespace
