module;

export module AxNativeUI;

#if AX_NATIVE_UI_WIN32
	export import AxNativeUI.Backend_Win32;
	export namespace ax {
		using NativeUIApp_Backend		= NativeUIApp_Win32;
		using NativeUIWindow_Backend	= NativeUIWindow_Win32;
	} // namespace
#else

	export import AxNativeUI.Backend_Null;
	export namespace ax {
		using NativeUIWindow_Impl = NativeUIWindow_Null;
	} // namespace
#endif

export namespace ax {
	class NativeUIApp : public NativeUIApp_Backend {
		AX_RTTI_INFO(NativeUIApp, NativeUIApp_Backend)
	public:
		AX_DOWNCAST_GET_INSTANCE()

		NativeUIApp(const CreateDesc& desc = CreateDesc()) : Base(desc) {}
	};

	
	class NativeUIWindow : public NativeUIWindow_Backend {
		AX_RTTI_INFO(NativeUIWindow, NativeUIWindow_Backend)
	public:
		NativeUIWindow(CreateDesc& desc) : Base(desc) {}
	};
	
} // namespace ax
