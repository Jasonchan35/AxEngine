module;

#if AX_NATIVE_UI_WIN32

export module AxNativeUI.Backend_Win32;
export import AxNativeUI.Backend_Base;

export namespace ax {

class NativeUI_Win32 {
public:
	enum { AX_WM_USER_HasCustomAppEvent = WM_USER + 1 };

	static NativeUIKeyCode		 s_convertKey(WPARAM vkey);
	static NativeUIEventModifier s_eventModifier();

	static Vec2f  s_worldToLocalPos(HWND hwnd, const Vec2f& pt);
	static Vec2f  s_localToWorldPos(HWND hwnd, const Vec2f& pt);
	static Rect2f s_getWorldRect(HWND hwnd);
};

class NativeUIEventHandler_Win32 {
public:
	enum { AX_WM_USER_HasCustomAppEvent = WM_USER + 1 };

	virtual ~NativeUIEventHandler_Win32() = default;

	virtual void onUIMouseEvent(NativeUIMouseEvent& ev) {}
	virtual void onUIKeyEvent(NativeUIKeyEvent& ev) {}

	bool handleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	NativeUIMouseEventButton _mousePressedButtons	  = NativeUIMouseEventButton::None;
	NativeUIMouseEventButton _lastMousePressedButtons = NativeUIMouseEventButton::None;

	bool _handleMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleKeyEvent(  HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

class NativeUIWindow_Win32 : public NativeUIWindow_Base {
	AX_RTTI_INFO(NativeUIWindow_Win32, NativeUIWindow_Base)
public:
	virtual ~NativeUIWindow_Win32() override;

	NativeUIWindow_Win32(CreateDesc& desc);

	HWND	hwnd() { return _hwnd; }
	NativeUIScreen	screen();

protected:
	virtual void onNativeSetVisible	(bool b) override;
	virtual void onNativeSetActive	(bool b) override;
	virtual void onNativeSetWorldPos(const Vec2f& v) override;
	virtual void onNativeSetSize	(const Vec2f& v) override;
	virtual void onNativeSetWindowTitle(StrView title) override;
	virtual void doCloseWindow	() override;
	virtual void doDestroyWindow() override;

private:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AX_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	class UIEventHandler : public NativeUIEventHandler_Win32 {
		NativeUIWindow_Win32* _owner = nullptr;
	public:
		UIEventHandler(NativeUIWindow_Win32* owner) : _owner(owner) {}

		virtual void onUIMouseEvent(NativeUIMouseEvent& ev) override { _owner->onNativeUIMouseEvent(ev); }
		virtual void onUIKeyEvent  (NativeUIKeyEvent&   ev) override { _owner->onNativeUIKeyEvent(ev); }
	};
	
	UIEventHandler _uiEventHandler;

	HWND	_hwnd = nullptr;
};


class NativeUIApp_Win32 : public NativeUIApp_Base {
	AX_RTTI_INFO(NativeUIApp_Win32, NativeUIApp_Base)
public:
	AX_DOWNCAST_GET_INSTANCE()

	NativeUIApp_Win32(const CreateDesc& desc) : Base(desc) {}

	virtual int		onRun		() override;
	virtual	void	quit		(int returnCode) override;

	static Rect2f s_getVirtualDesktopRect();

	MSG _win32_msg;

protected:
	virtual bool	onSetNativeCursor(NativeUICursorType type) override;
	virtual bool	onSetNativeCursorPos(const Vec2f& worldPos) override;
	virtual bool	onRequestNativeCustomAppEvent() override;

private:
	void _createHiddenWindow();
	static LRESULT WINAPI s_hiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define AX_MACRO_OP(E, VALUE) HCURSOR _cursor##E = nullptr;
	AX_NativeUICursorType_ENUM_LIST(AX_MACRO_OP)
#undef AX_MACRO_OP

HWND _hiddenWindow = nullptr;
};

AX_INLINE Vec2f axWin32_LPARAM_to_Vec2f(LPARAM lp) {
	auto x = static_cast<i16>(LOWORD(lp));
	auto y = static_cast<i16>(HIWORD(lp));
	return Vec2f(x, y);
}


} // namespace ax

#endif // #if AX_NATIVE_UI_WIN32