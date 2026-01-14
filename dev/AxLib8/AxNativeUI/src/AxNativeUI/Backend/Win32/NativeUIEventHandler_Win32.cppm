module;

#if AX_NATIVE_UI_WIN32

export module AxNativeUI:NativeUIEventHandler_Win32;
export import :NativeUIWindow_Base;
export import :NativeUI_Win32;

export namespace ax {

class NativeUIEventHandler_Win32 {
public:
	virtual ~NativeUIEventHandler_Win32() = default;

	virtual void onNativeUIMouseEvent(NativeUIMouseEvent& ev) {}
	virtual void onNativeUIKeyEvent(NativeUIKeyEvent& ev) {}

	bool handleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	NativeUIMouseEventButton _mousePressedButtons	  = NativeUIMouseEventButton::None;

	bool _handleMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleKeyEvent(  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


AX_INLINE Vec2f axWin32_LPARAM_to_Vec2f(LPARAM lp) {
	auto x = static_cast<i16>(LOWORD(lp));
	auto y = static_cast<i16>(HIWORD(lp));
	return Vec2f(x, y);
}


}

#endif