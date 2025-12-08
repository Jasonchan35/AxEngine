module;

export module AxNativeUI;
export import :NativeUIEvent;

#if AX_NATIVE_UI_WIN32

export import :NativeUIApp_Win32;
export namespace ax {
	using NativeUIApp_Impl		= NativeUIApp_Win32;
	using NativeUIWindow_Impl	= NativeUIWindow_Win32;
} // namespace

#else

export import :NativeUI_Null;
export namespace ax {
	using NativeUIWindow_Impl = NativeUIWindow_Null;
} // namespace

#endif

export namespace ax {
	class NativeUIApp : public NativeUIApp_Impl {
		AX_RTTI_INFO(NativeUIApp, NativeUIApp_Impl)
	public:
		AX_DOWNCAST_GET_INSTANCE()

		NativeUIApp(const CreateDesc& desc = CreateDesc()) : Base(desc) {}
	};

	
	class NativeUIWindow : public NativeUIWindow_Impl {
		AX_RTTI_INFO(NativeUIWindow, NativeUIWindow_Impl)
	public:
		NativeUIWindow(CreateDesc& desc) : Base(desc) {}
	};
	
} // namespace ax
