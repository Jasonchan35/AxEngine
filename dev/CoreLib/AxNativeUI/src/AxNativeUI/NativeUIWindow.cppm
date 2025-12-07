module;

export module AxNativeUI.Window;

#if AX_OS_WINDOWS
	export import AxNativeUI.Backend_Win32;
	export namespace ax { using NativeUIWindow_Impl = NativeUIWindow_Win32; }
#else

#include "Null/NativeUIWindow_Null.h"
namespace ax {
	using NativeUIWindow_Impl = NativeUIWindow_Null;
	}
#endif

export namespace ax {
	class NativeUIWindow : public NativeUIWindow_Impl {
		AX_RTTI_INFO(NativeUIWindow, NativeUIWindow_Impl)
	public:
		NativeUIWindow(CreateDesc& desc) : Base(desc) {}
	};
	
} // namespace ax
