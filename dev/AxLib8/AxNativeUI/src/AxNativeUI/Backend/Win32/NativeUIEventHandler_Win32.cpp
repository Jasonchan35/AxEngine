module;

#if AX_NATIVE_UI_WIN32

module AxNativeUI;
import :NativeUIEventHandler_Win32;
import :NativeUI_Win32;

namespace ax {

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

	_mousePressedButtons = Button::None;
	if (wParam & MK_LBUTTON)  _mousePressedButtons |= Button::Left;
	if (wParam & MK_MBUTTON)  _mousePressedButtons |= Button::Middle;
	if (wParam & MK_RBUTTON)  _mousePressedButtons |= Button::Right;
	if (wParam & MK_XBUTTON1) _mousePressedButtons |= Button::Button4;
	if (wParam & MK_XBUTTON2) _mousePressedButtons |= Button::Button5;

//	ev.pressedButtons = _mousePressedButtons;

	if (ev.type == Type::Down) {
		if (_mousePressedButtons == Button::None) {
			::SetCapture(hwnd);
		}

	}else if (ev.type == Type::Up) {
		if (_mousePressedButtons == Button::None) {
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
	ev.key		= NativeUI_Win32::s_toNativeKey(int(wParam));

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

}

#endif