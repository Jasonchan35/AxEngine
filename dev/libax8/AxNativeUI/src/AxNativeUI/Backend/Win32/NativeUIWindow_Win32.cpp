module;

#if AX_NATIVE_UI_WIN32

module AxNativeUI;
import :NativeUIWindow_Win32;
import :NativeUI_Win32;

namespace ax {

NativeUIWindow_Win32::~NativeUIWindow_Win32() {
	if (_hwnd) {
		::SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
		::DestroyWindow(_hwnd);
	}
}

LRESULT WINAPI NativeUIWindow_Win32::s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thisObj));
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd)) {
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
				thisObj->_hwnd = nullptr;
				ax_delete(thisObj);
			}
		}break;

		case WM_CLOSE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				thisObj->onCloseButton();
				return 0;
			}
		}break;

		case WM_ACTIVATE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				u16 a = LOWORD(wParam);
				switch (a) {
					case WA_ACTIVE:		thisObj->onActiveChanged(true);  break;
					case WA_CLICKACTIVE:thisObj->onActiveChanged(true);  break;
					case WA_INACTIVE:	thisObj->onActiveChanged(false); break;
				}
			}
		}break;

		case WM_MOVE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				RECT rc;
				::GetWindowRect(hwnd, &rc);
				thisObj->onWorldPosChanged(NativeUI_Win32::s_getWorldRect(hwnd).pos);
			}
		}break;

//		case WM_SIZING: {
//			if (auto* thisObj = s_getThis(hwnd)) {
//				thisObj->onSizeChanged(s_win32_getWorldRect(hwnd).size);
//			}
//		}break;

		case WM_SIZE: {
			if (auto* thisObj = s_getThis(hwnd)) {
				RECT rc;
				::GetWindowRect(hwnd, &rc);
				thisObj->onSizeChanged(Rect2f::s_from(rc).size);
			}
		}break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				if (thisObj->_uiEventHandler.handleEvent(hwnd, msg, wParam, lParam))
					return 0;
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

NativeUIScreen NativeUIWindow_Win32::screen() {
	if (_hwnd) {
		return MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
	}
	return NativeUIScreen();
}

NativeUIWindow_Win32::NativeUIWindow_Win32(CreateDesc& desc) 
: Base(desc) 
, _uiEventHandler(this)
{
	const wchar_t* clsName = L"NativeUIWindow";

	auto hInstance = ::GetModuleHandle(nullptr);
	WNDCLASSEX wc = {};
	wc.cbSize			= sizeof(wc);
	wc.style			= CS_HREDRAW | CS_VREDRAW; // | CS_DROPSHADOW;
	wc.lpfnWndProc		= &s_wndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= nullptr; //(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= clsName;
	wc.hIconSm			= LoadIcon(hInstance, IDI_APPLICATION);

	if (!desc.closeButton) {
		wc.style |= CS_NOCLOSE;
	}

	DWORD dwStyle = 0;
	DWORD dwExStyle = WS_EX_ACCEPTFILES;

	switch (desc.type) {
		case CreateDesc::Type::ToolWindow:
		case CreateDesc::Type::NormalWindow: {
			dwStyle   |= WS_OVERLAPPED | WS_SYSMENU;

			if (desc.closeButton) dwStyle |= WS_SYSMENU;
			if (desc.resizable  ) dwStyle |= WS_THICKFRAME;
			if (desc.titleBar   ) dwStyle |= WS_CAPTION;
			if (desc.minButton  ) dwStyle |= WS_MINIMIZEBOX;
			if (desc.maxButton  ) dwStyle |= WS_MAXIMIZEBOX;
		}break;

		case CreateDesc::Type::PopupWindow: {
			dwStyle   |= WS_POPUP | WS_BORDER;
		}break;
		default: AX_ASSERT(false); break;
	}

	if (desc.type == CreateDesc::Type::ToolWindow) {
		dwExStyle |= WS_EX_TOOLWINDOW;
	}

	if (desc.alwaysOnTop) dwExStyle |= WS_EX_TOPMOST;

	WNDCLASSEX tmpWc;
	bool registered = (0 != ::GetClassInfoEx(hInstance, clsName, &tmpWc));
	if (!registered) {
		if (!::RegisterClassEx(&wc)) {
			throw Error_Undefined();
		}
	}

	auto rect = desc.rect;
	if (desc.centerToScreen) {
		auto screenSize = Vec2f(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
								static_cast<float>(GetSystemMetrics(SM_CYSCREEN)));
		rect.pos = (screenSize - rect.size) / 2.0f;
	}

	_hwnd = ::CreateWindowExW(dwExStyle, clsName, clsName, dwStyle,
								static_cast<int>(desc.rect.x),
								static_cast<int>(desc.rect.y),
								static_cast<int>(desc.rect.w),
								static_cast<int>(desc.rect.h),
								nullptr, nullptr, hInstance, this);
	if (!_hwnd) {
		throw Error_Undefined();
	}

	setWorldRect(NativeUI_Win32::s_getWorldRect(_hwnd));

	setVisible(desc.visible);
}

void NativeUIWindow_Win32::onNativeSetWorldPos(const Vec2f& v) {
	if (!_hwnd) { AX_ASSERT(false); return; }

	POINT pt {static_cast<LONG>(v.x), static_cast<LONG>(v.y)};

	if (auto parentHwnd = ::GetParent(_hwnd)) {
		// get pos relative to parent
		::MapWindowPoints(nullptr, parentHwnd, &pt, 1);
	}

	RECT rc;
	::GetWindowRect(_hwnd, &rc);
	auto w = rc.right  - rc.left;
	auto h = rc.bottom - rc.top;

	::MoveWindow(_hwnd, pt.x, pt.y, w, h, false);
}

void NativeUIWindow_Win32::onNativeSetSize(const Vec2f& v) {
	if (!_hwnd) { AX_ASSERT(false); return; }

	RECT rc_{0,0,0,0};
	RECT clientRc_{0,0,0,0};
	::GetWindowRect(_hwnd, &rc_);
	::MoveWindow(_hwnd, rc_.left, rc_.top, static_cast<int>(v.x), static_cast<int>(v.y), true);

	// get again after MoveWindow
	::GetWindowRect(_hwnd, &rc_);
	::GetClientRect(_hwnd, &clientRc_);
	
	auto rc = Rect2f::s_from(rc_);
	auto clientRc = Rect2f::s_from(clientRc_);

	POINT pt_{0,0};
	::ClientToScreen(_hwnd, &pt_);
	auto pt = Vec2f::s_from(pt_);

	clientRc.pos += pt;

	Base::setPadding(rc - clientRc);
}

void NativeUIWindow_Win32::onNativeSetWindowTitle(StrView title) {
	TempStringW t;
	t.setUtf(title);

	if (_hwnd) {
		SetWindowText(_hwnd, t.c_str());
	}
}

void NativeUIWindow_Win32::doCloseWindow() {
	::SendMessage(_hwnd, WM_CLOSE, 0, 0);
}

void NativeUIWindow_Win32::doDestroyWindow() {
	if (_hwnd) {
		::DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
}

void NativeUIWindow_Win32::onNativeSetVisible(bool b) {
	::ShowWindow(_hwnd, b ? SW_SHOWNOACTIVATE : SW_HIDE);
}

void NativeUIWindow_Win32::onNativeSetActive(bool b) {
	if (b) {
		::SetActiveWindow(_hwnd);
	} else {
		if (_hwnd == ::GetActiveWindow()) {
			::SetActiveWindow(nullptr);
		}
	}
}

/*
Vec2f NativeUIWindow_toMonitorSpace(HWND hwnd, const Vec2f v) {
	auto m = ::MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
	if (!m) return v;
	MONITORINFOEX p;
	info.cbSize = sizeof(info);
	::GetMonitorInfo(m, &info);
	auto rc = info.rcWork;
}
*/

//Rect2f	NativeUIWindow::windowRect		() const { RECT rc; ::GetWindowRect(_hwnd, &rc); return Rect2f(rc); }
//Rect2f	NativeUIWindow::contentRect		() const { RECT rc; ::GetClientRect(_hwnd, &rc); return Rect2f(rc); }
//void		NativeUIWindow::setWindowRect	(const Rect2f& rect) { ::MoveWindow(_hwnd, (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h, true); }


}

#endif