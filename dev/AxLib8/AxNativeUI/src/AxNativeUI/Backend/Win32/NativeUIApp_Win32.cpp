module;

#if AX_NATIVE_UI_WIN32

module AxNativeUI;
import :NativeUIApp_Win32;

namespace ax {



int NativeUIApp_Win32::onRun() {
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	
	_createHiddenWindow();

	_cursorUnknown			= LoadCursor(nullptr, IDC_ARROW);
//-----
	_cursorArrow			= LoadCursor(nullptr, IDC_ARROW);
	_cursorClosedHand		= LoadCursor(nullptr, IDC_CROSS);
	_cursorContextualMenu	= nullptr;
	_cursorCrossHair		= nullptr;
	_cursorDisappearingItem = nullptr;
	_cursorDragCopy			= nullptr;
	_cursorDragLink			= nullptr;
	_cursorHelp				= LoadCursor(nullptr, IDC_HELP);
	_cursorIBeam			= LoadCursor(nullptr, IDC_IBEAM);
	_cursorIBeamVertical	= nullptr;
	_cursorNotAllowed		= LoadCursor(nullptr, IDC_NO);
	_cursorOpenHand			= LoadCursor(nullptr, IDC_SIZEALL); //! need custom icon
	_cursorPointingHand		= LoadCursor(nullptr, IDC_HAND);
	_cursorResizeAll		= LoadCursor(nullptr, IDC_SIZEALL);
	_cursorResizeNESW		= LoadCursor(nullptr, IDC_SIZENESW);
	_cursorResizeNS			= LoadCursor(nullptr, IDC_SIZENS);
	_cursorResizeNWSE		= LoadCursor(nullptr, IDC_SIZENWSE);
	_cursorResizeWE			= LoadCursor(nullptr, IDC_SIZEWE);
	_cursorShortWait		= LoadCursor(nullptr, IDC_APPSTARTING);
	_cursorWait				= LoadCursor(nullptr, IDC_WAIT);

	// IDC_UPARROW

	create();

	for (;;) {
		if (_desc.peekMessage) {
			if (!::PeekMessage(&_win32_msg, nullptr, 0, 0, PM_REMOVE)) {
				onPeekMessage();
				continue;
			}
		} else {
			if (!::GetMessage(&_win32_msg, nullptr, 0, 0)) break;
		}

		if (_win32_msg.message == WM_QUIT) break;

		::TranslateMessage(&_win32_msg);
		::DispatchMessage(&_win32_msg);
		handleCustomAppEvent(false);
	}

	willQuit();
	return _returnCode;
}

void NativeUIApp_Win32::quit(int returnCode) {
	_returnCode = returnCode;
	::PostQuitMessage(returnCode);
}

Rect2f NativeUIApp_Win32::s_getVirtualDesktopRect() {
	return Rect2f(0, 0,
					static_cast<float>(GetSystemMetrics(SM_CXVIRTUALSCREEN)), 
					static_cast<float>(GetSystemMetrics(SM_CYVIRTUALSCREEN)));
}

bool NativeUIApp_Win32::s_getAsyncKeyState(NativeUIKeyCode key) {
	int w = NativeUI_Win32::to_Win32KeyCode(key);
	return ::GetAsyncKeyState(w) != 0;
}

bool NativeUIApp_Win32::onSetNativeCursor(NativeUICursorType type) {
	HCURSOR o = nullptr;
	switch (type) {
		#define AX_MACRO_OP(E, VALUE) case NativeUICursorType::E: o = _cursor##E; break;
			AX_NativeUICursorType_ENUM_LIST(AX_MACRO_OP)
		#undef AX_MACRO_OP
	}

	if (!o) {
		AX_ASSERT(false);
		o = _cursorArrow;
	}
	::SetCursor(o);

	return true;
}

bool NativeUIApp_Win32::onSetNativeCursorPos(const Vec2f& worldPos) {
	auto ret = ::SetCursorPos(static_cast<int>(worldPos.x), static_cast<int>(worldPos.y));
	return ret != 0;
}

bool NativeUIApp_Win32::onRequestNativeCustomAppEvent() {
	if (!_hiddenWindow)
		return false;

	if (!::PostMessage(_hiddenWindow, AX_WM_USER_HasCustomAppEvent, 0, 0))
		return false;
	return true;
}

LRESULT WINAPI NativeUIApp_Win32::s_hiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case AX_WM_USER_HasCustomAppEvent: {
			NativeUIApp_Win32::s_instance()->handleCustomAppEvent(false);
		}break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void NativeUIApp_Win32::_createHiddenWindow() {
	const wchar_t* clsName = L"NativeUIApp_HiddenWindow";

	auto hInstance = ::GetModuleHandle(nullptr);
	WNDCLASSEX wc = {};
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= &s_hiddenWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= nullptr;
	wc.hCursor			= nullptr;
	wc.hbrBackground	= nullptr;
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	DWORD dwStyle = 0;
	DWORD dwExStyle = 0;

	if (!::RegisterClassEx(&wc)) {
		throw Error_Undefined();
	}

	_hiddenWindow = ::CreateWindowExW(dwExStyle, clsName, clsName, dwStyle, 
								0, 0, 0, 0, 
								nullptr, nullptr, hInstance, nullptr);
	if (!_hiddenWindow) {
		throw Error_Undefined();
	}	
}


}

#endif