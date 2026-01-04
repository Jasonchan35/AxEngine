module;

#if AX_NATIVE_UI_WIN32

export module AxNativeUI:NativeUIWindow_Win32;
export import :NativeUIWindow_Base;
export import :NativeUIEventHandler_Win32;

export namespace ax {

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

		virtual void onNativeUIMouseEvent(NativeUIMouseEvent& ev) override { _owner->_onNativeUIMouseEvent(ev); }
		virtual void onNativeUIKeyEvent  (NativeUIKeyEvent&   ev) override { _owner->_onNativeUIKeyEvent(ev); }
	};
	
	UIEventHandler _uiEventHandler;

	HWND	_hwnd = nullptr;
};




} //namespace

#endif