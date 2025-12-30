module;

#if AX_NATIVE_UI_WIN32

export module AxNativeUI:NativeUI_Win32;
export import :NativeUIWindow_Base;

export namespace ax {

constexpr UINT AX_WM_USER_HasCustomAppEvent = WM_USER + 1;

class NativeUI_Win32 {
public:

	static NativeUIKeyCode	 s_toNativeKey(int key);
	static int				 s_toWin32Key(NativeUIKeyCode key);
	
	static NativeUIEventModifier s_eventModifier();

	static Vec2f  s_worldToLocalPos(HWND hwnd, const Vec2f& pt);
	static Vec2f  s_localToWorldPos(HWND hwnd, const Vec2f& pt);

	static Rect2f s_getWorldRect(HWND hwnd);
	static Rect2f s_getLocalRect(HWND hwnd);
};


} // namespace ax

#endif // #if AX_NATIVE_UI_WIN32