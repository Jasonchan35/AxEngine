module;

#if AX_NATIVE_UI_WIN32

export module AxNativeUI:NativeUIApp_Win32;
export import :NativeUIWindow_Win32;
export import :NativeUIApp_Base;

export namespace ax {

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

} //namespace

#endif