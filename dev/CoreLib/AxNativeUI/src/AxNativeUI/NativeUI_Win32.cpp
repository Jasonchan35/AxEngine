module;

#if AX_NATIVE_UI_WIN32

module AxNativeUI.Backend_Win32;

namespace ax {


NativeUIKeyCode NativeUI_Win32::s_convertKey(WPARAM vkey) {
	using KeyCode = NativeUIKeyCode;
//	AX_LOG("vkey = {:X}", vkey);
	
	using IntType = EnumFn_<NativeUIKeyCode>::IntType;

	if (vkey >= '0' && vkey <= '9')
		return NativeUIKeyCode::Digi0 + static_cast<IntType>(vkey - '0');

	if (vkey >= 'A' && vkey <= 'Z')
		return NativeUIKeyCode::A + static_cast<IntType>(vkey - 'A');

	if (vkey >= VK_NUMPAD0 && vkey <= VK_NUMPAD9)
		return NativeUIKeyCode::NumPad0 + static_cast<IntType>(vkey - VK_NUMPAD0);

	if (vkey >= VK_F1 && vkey <= VK_F24)
		return NativeUIKeyCode::F1 + static_cast<IntType>(vkey - VK_F1);

	switch (vkey) {
		case VK_BACK:				return KeyCode::Backspace;
		case VK_TAB:				return KeyCode::Tab;
		case VK_CLEAR:				return KeyCode::Clear;

		case VK_RETURN:				return KeyCode::Enter;

		case VK_SHIFT:				return KeyCode::Shift;
		case VK_CONTROL:			return KeyCode::Ctrl;
		case VK_MENU:				return KeyCode::Alt;
		case VK_PAUSE:				return KeyCode::Pause;
		case VK_CAPITAL:			return KeyCode::CapsLock;

		case VK_KANA:				return KeyCode::IME_Kana;
//		case VK_HANGUL:				return KeyCode::IME_Hangul;
		case VK_JUNJA:				return KeyCode::IME_Junja;
		case VK_FINAL:				return KeyCode::IME_Final;
//		case VK_HANJA:				return KeyCode::IME_Hanja;
		case VK_KANJI:				return KeyCode::IME_Kanji;

		case VK_ESCAPE:				return KeyCode::Escape;

		case VK_CONVERT:			return KeyCode::IME_Convert;
		case VK_NONCONVERT:			return KeyCode::IME_NonConvert;
		case VK_ACCEPT:				return KeyCode::IME_Accept;
		case VK_MODECHANGE:			return KeyCode::IME_ModeChange;

		case VK_SPACE:				return KeyCode::Space;
		case VK_PRIOR:				return KeyCode::PageUp;
		case VK_NEXT:				return KeyCode::PageDown;
		case VK_END:				return KeyCode::End;
		case VK_HOME:				return KeyCode::Home;
		case VK_LEFT:				return KeyCode::LeftArrow;
		case VK_UP:					return KeyCode::UpArrow;
		case VK_RIGHT:				return KeyCode::RightArrow;
		case VK_DOWN:				return KeyCode::DownArrow;
		case VK_SELECT:				return KeyCode::Select;
		case VK_PRINT:				return KeyCode::Print;
		case VK_EXECUTE:			return KeyCode::Execute;
		case VK_SNAPSHOT:			return KeyCode::PrintScreen;
		case VK_INSERT:				return KeyCode::Insert;
		case VK_DELETE:				return KeyCode::Delete;
		case VK_HELP:				return KeyCode::Help;
		// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
		// 0x40 : unassigned
		// VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
		case VK_LWIN:				return KeyCode::LWin;
		case VK_RWIN:				return KeyCode::RWin;
		case VK_APPS:				return KeyCode::Apps;
		// 0x5E : reserved
		case VK_SLEEP:				return KeyCode::Sleep;

		case VK_MULTIPLY:			return KeyCode::NumPadMul;
		case VK_ADD:				return KeyCode::NumPadAdd;
		case VK_SEPARATOR:			return KeyCode::Separator;
		case VK_SUBTRACT:			return KeyCode::NumPadSub;
		case VK_DECIMAL:			return KeyCode::NumPadDecimal;
		case VK_DIVIDE:				return KeyCode::NumPadDiv;
		// VK_F1 - VK_F24 (0x70 - 0x87)
		// 0x88 - 0x8F : unassigned
		case VK_NUMLOCK:			return KeyCode::NumLock;
		case VK_SCROLL:				return KeyCode::ScrollLock;
		// NEC PC-9800 kbd definitions
		case VK_OEM_NEC_EQUAL:		return KeyCode::NumPadEqual;
		// Fujitsu/OASYS kbd definitions
		//#define VK_OEM_FJ_JISHO   0x92   // 'Dictionary' key
		//#define VK_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
		//#define VK_OEM_FJ_TOUROKU 0x94   // 'Register word' key
		//#define VK_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
		//#define VK_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key
		// 0x97 - 0x9F : unassigned
		/*
		 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
		 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
		 * No other API or message will distinguish left and right keys in this way.
		 */
		case VK_LSHIFT:				return KeyCode::LShift;
		case VK_RSHIFT:				return KeyCode::RShift;
		case VK_LCONTROL:			return KeyCode::LCtrl;
		case VK_RCONTROL:			return KeyCode::RCtrl;
		case VK_LMENU:				return KeyCode::LAlt;
		case VK_RMENU:				return KeyCode::RAlt;
		// 
	#if(_WIN32_WINNT >= 0x0500)
		case VK_BROWSER_BACK:		return KeyCode::BrowserBack;
		case VK_BROWSER_FORWARD:	return KeyCode::BrowserForward;
		case VK_BROWSER_REFRESH:	return KeyCode::BrowserRefresh;
		case VK_BROWSER_STOP:		return KeyCode::BrowserStop;
		case VK_BROWSER_SEARCH:		return KeyCode::BrowserBack;
		case VK_BROWSER_FAVORITES:	return KeyCode::BrowserFavorites;
		case VK_BROWSER_HOME:		return KeyCode::BrowserHome;
		//
		case VK_VOLUME_MUTE:		return KeyCode::VolumeMute;
		case VK_VOLUME_DOWN:		return KeyCode::VolumeDown;
		case VK_VOLUME_UP:			return KeyCode::VolumeUp;
		case VK_MEDIA_NEXT_TRACK:	return KeyCode::MediaNextTrack;
		case VK_MEDIA_PREV_TRACK:	return KeyCode::MediaPrevTrack;
		case VK_MEDIA_STOP:			return KeyCode::MediaStop;
		case VK_MEDIA_PLAY_PAUSE:	return KeyCode::MediaPlayPause;
		//
		case VK_LAUNCH_MAIL:		return KeyCode::LaunchMail;
		case VK_LAUNCH_MEDIA_SELECT:return KeyCode::LaunchMediaSelect;
		case VK_LAUNCH_APP1:		return KeyCode::LaunchMediaApp1;
		case VK_LAUNCH_APP2:		return KeyCode::LaunchMediaApp2;
	#endif /* _WIN32_WINNT >= 0x0500 */
		/*
		 * 0xB8 - 0xB9 : reserved
		 */
		case VK_OEM_1:				return KeyCode::SemiColon;	// ';:' key
		case VK_OEM_PLUS:			return KeyCode::Equals;		// '=+' key
		case VK_OEM_COMMA:			return KeyCode::Comma;		// ',<' key
		case VK_OEM_MINUS:			return KeyCode::Minus;		// '-_' key
		case VK_OEM_PERIOD:			return KeyCode::Period;		// '.>' key
		case VK_OEM_2:				return KeyCode::Slash;		// '/?' key
		case VK_OEM_3:				return KeyCode::BackQuote;	// '`~' key
		/*
		 * 0xC1 - 0xD7 : reserved
		 */
		/*
		 * 0xD8 - 0xDA : unassigned
		 */
		case VK_OEM_4:				return KeyCode::LBracket;	//  '[{' for US
		case VK_OEM_5:				return KeyCode::BackSlash;	//  '\|' for US
		case VK_OEM_6:				return KeyCode::RBracket;	//  ']}' for US
		case VK_OEM_7:				return KeyCode::Quotes;		//  ''"' for US
		//case VK_OEM_8:			return KeyCode::;
		/*
		 * 0xE0 : reserved
		 */
		/*
		 * Various extended or enhanced keyboards
		 */
		//case VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
		//case VK_OEM_102		 0xE2  //  "<>" or "\|" on RT 102-key kbd.
		//case VK_ICO_HELP       0xE3  //  Help key on ICO
		//case VK_ICO_00         0xE4  //  00 key on ICO

	#if(WINVER >= 0x0400)
		case VK_PROCESSKEY:			return NativeUIKeyCode::ProcessKey;
	#endif /* WINVER >= 0x0400 */
	// #define VK_ICO_CLEAR      0xE6
	#if(_WIN32_WINNT >= 0x0500)
	// #define VK_PACKET         0xE7
	#endif /* _WIN32_WINNT >= 0x0500 */
		/*
		 * 0xE8 : unassigned
		 */
		/*
		 * Nokia/Ericsson definitions
		 */
		//#define VK_OEM_RESET      0xE9
		//#define VK_OEM_JUMP       0xEA
		//#define VK_OEM_PA1        0xEB
		//#define VK_OEM_PA2        0xEC
		//#define VK_OEM_PA3        0xED
		//#define VK_OEM_WSCTRL     0xEE
		//#define VK_OEM_CUSEL      0xEF
		//#define VK_OEM_ATTN       0xF0
		//#define VK_OEM_FINISH     0xF1
		//#define VK_OEM_COPY       0xF2
		//#define VK_OEM_AUTO       0xF3
		//#define VK_OEM_ENLW       0xF4
		//#define VK_OEM_BACKTAB    0xF5
		//
		//#define VK_ATTN           0xF6
		//#define VK_CRSEL          0xF7
		//#define VK_EXSEL          0xF8
		//#define VK_EREOF          0xF9
		//#define VK_PLAY           0xFA
		//#define VK_ZOOM           0xFB
		//#define VK_NONAME         0xFC
		//#define VK_PA1            0xFD
		//#define VK_OEM_CLEAR      0xFE
		/*
		 * 0xFF : reserved
		 */
		default:		return NativeUIKeyCode::None;
	}
}

NativeUIEventModifier NativeUI_Win32::s_eventModifier() {
	auto o = static_cast<NativeUIEventModifier>(0);
	if (::GetAsyncKeyState(VK_CONTROL)) o |= NativeUIEventModifier::Ctrl;
	if (::GetAsyncKeyState(VK_SHIFT  )) o |= NativeUIEventModifier::Shift;
	if (::GetAsyncKeyState(VK_MENU   )) o |= NativeUIEventModifier::Atl;
	if (::GetAsyncKeyState(VK_LWIN) || ::GetAsyncKeyState(VK_RWIN)) {
		o |= NativeUIEventModifier::Cmd;
	}
	return o;
}

Vec2f NativeUI_Win32::s_localToWorldPos(HWND hwnd, const Vec2f& pt) {
	HWND desktop = nullptr;
	if (auto parentHwnd = ::GetParent(hwnd)) {
		POINT tmp = pt.to_POINT();
		MapWindowPoints(hwnd, desktop, &tmp, 1);
		return Vec2f::s_from(tmp);
	}

	RECT rc;
	::GetWindowRect(hwnd, &rc);
	auto o = Rect2f::s_from(rc);
	return pt + o.pos;
}

Vec2f NativeUI_Win32::s_worldToLocalPos(HWND hwnd, const Vec2f& pt) {
	HWND desktop = nullptr;
	if (auto parentHwnd = ::GetParent(hwnd)) {
		POINT tmp = pt.to_POINT();
		MapWindowPoints(desktop, hwnd, &tmp, 1);
		return Vec2f::s_from(tmp);
	}

	RECT rc;
	::GetWindowRect(hwnd, &rc);
	auto o = Rect2f::s_from(rc);
	return pt - o.pos;
}


Rect2f NativeUI_Win32::s_getWorldRect(HWND hwnd) {
	RECT rc;
	::GetWindowRect(hwnd, &rc);
	auto o = Rect2f::s_from(rc);

	o.pos = s_localToWorldPos(hwnd, o.pos);
	return o;
}

bool NativeUIEventHandler_Win32::handleEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (_handleKeyEvent  (hwnd, msg, wParam, lParam)) return true;
	if (_handleMouseEvent(hwnd, msg, wParam, lParam)) return true;
	return false;
}

bool NativeUIEventHandler_Win32::_handleMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto* app = NativeUIApp_Win32::s_instance();
	if (!app) return false;

	using Type = NativeUIMouseEvent::Type;
	using Button = NativeUIMouseEvent::Button;
	using Modifier = NativeUIMouseEvent::Modifier;

	NativeUIMouseEvent ev;
	ev.time = Milliseconds(app->_win32_msg.time);
	ev.modifier = NativeUI_Win32::s_eventModifier();

#if 0
	// !! From Windows Document:
	// Important  Do not use the LOWORD or HIWORD macros to extract the x- and y- coordinates of the cursor position 
	// because these macros return incorrect results on systems with multiple monitors. 
	// Systems with multiple monitors can have negative x- and y- coordinates, and LOWORD and HIWORD treat the coordinates as unsigned quantities.

	//ev.worldPos = axWin32_LPARAM_to_Vec2f(lParam) + posRelativeToWindow;
#else
	POINT curPos;
	::GetCursorPos(&curPos);

	auto pos = Vec2f::s_from(curPos);

	ev.worldPos = pos;
#endif

	auto xbutton = Button::None;
	uint16_t wParamHi = HIWORD(wParam);

	switch(wParamHi) {
		case XBUTTON1: xbutton = Button::Button4; break;
		case XBUTTON2: xbutton = Button::Button5; break;
	}

	switch (msg) {
		//case WM_LBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = Button::Left;   } break;
		//case WM_MBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = Button::Middle; } break;
		//case WM_RBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = Button::Right;  } break;
		//case WM_XBUTTONDBLCLK:	{ ev.type = Type::DoubleClick;	ev.button = xbutton;		} break;

		case WM_LBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = Button::Left;   } break;
		case WM_MBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = Button::Middle; } break;
		case WM_RBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = Button::Right;  } break;
		case WM_XBUTTONDOWN:	{ ev.type = Type::Down;		ev.button = xbutton;		} break;

		case WM_LBUTTONUP:		{ ev.type = Type::Up;		ev.button = Button::Left;   } break;
		case WM_MBUTTONUP:		{ ev.type = Type::Up;		ev.button = Button::Middle; } break;
		case WM_RBUTTONUP:		{ ev.type = Type::Up;		ev.button = Button::Right;  } break;
		case WM_XBUTTONUP:		{ ev.type = Type::Up;		ev.button = xbutton;		} break;

		case WM_MOUSEMOVE:		{ ev.type = Type::Move;		ev.button = Button::None;   } break;

		#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
			case WM_MOUSEWHEEL:		{ ev.type = Type::Wheel;	ev.wheelDelta.set(0,GET_WHEEL_DELTA_WPARAM(wParam)); } break;
		#endif

		#if (_WIN32_WINNT >= 0x0600)
			case WM_MOUSEHWHEEL:	{ ev.type = Type::Wheel;	ev.wheelDelta.set(GET_WHEEL_DELTA_WPARAM(wParam),0); } break;
		#endif

		default:
			return false;
	}	

	auto pressedFn = EnumFn_(_mousePressedButtons);
	pressedFn = AX_ZERO;

	if (wParam & MK_LBUTTON)  pressedFn.setFlags(Button::Left);
	if (wParam & MK_MBUTTON)  pressedFn.setFlags(Button::Middle);
	if (wParam & MK_RBUTTON)  pressedFn.setFlags(Button::Right);
	if (wParam & MK_XBUTTON1) pressedFn.setFlags(Button::Button4);
	if (wParam & MK_XBUTTON2) pressedFn.setFlags(Button::Button5);

//	ev.pressedButtons = _mousePressedButtons;

	if (ev.type == Type::Down) {
		if (pressedFn.isZero()) {
			::SetCapture(hwnd);
		}

	}else if (ev.type == Type::Up) {
		if (pressedFn.isZero()) {
			::ReleaseCapture();
		}
	}

//	AX_DUMP_VAR(ev);
	onUIMouseEvent(ev);
	_lastMousePressedButtons = _mousePressedButtons;
	return true;
}

bool NativeUIEventHandler_Win32::_handleKeyEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto* app = NativeUIApp_Win32::s_instance();
	if (!app) return false;

	NativeUIKeyEvent ev;
	using Type = NativeUIKeyEventType;

	ev.time		= Milliseconds(app->_win32_msg.time);
	ev.modifier = NativeUI_Win32::s_eventModifier();
	ev.key		= NativeUI_Win32::s_convertKey(wParam);

	if (msg == WM_CHAR) {
		ev.type = Type::Char;
		Char32 ch = static_cast<Char32>(wParam);
		ev.text.setUtf(ch);

	} else {
		switch (msg) {
			case WM_KEYUP:		ev.type = Type::Up;	break;
			case WM_KEYDOWN:	ev.type = Type::Down; break;
			case WM_SYSKEYUP:	ev.type = Type::Up;	break;
			case WM_SYSKEYDOWN: ev.type = Type::Down; break;
			default: return false;
		}

		if (ev.type == NativeUIKeyEventType::Down) {
			ev.isRepeat = (lParam & (1<<30)) != 0;
			// if (ev.isRepeat) return; // don't send repeat down
		}
	}

//	AX_DUMP_VAR(ev);
	onUIKeyEvent(ev);
	return true;
}



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


int NativeUIApp_Win32::onRun() {
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

	if (!::PostMessage(_hiddenWindow, NativeUIEventHandler_Win32::AX_WM_USER_HasCustomAppEvent, 0, 0))
		return false;
	return true;
}

LRESULT WINAPI NativeUIApp_Win32::s_hiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case NativeUIEventHandler_Win32::AX_WM_USER_HasCustomAppEvent: {
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

} // namespace

#endif // #if AX_NATIVE_UI_WIN32
