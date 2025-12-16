module;

#if AX_NATIVE_UI_WIN32

module AxNativeUI;
import :NativeUI_Win32;

namespace ax {

struct NativeUI_Win32_KeyMap {
	using This = NativeUI_Win32_KeyMap;
	
	static NativeUI_Win32_KeyMap*	s_instance() {
		static GlobalSingleton<This> s;
		return s.ptr();
	}

	Dict<int, NativeUIKeyCode>	_win32ToNative;
	Dict<NativeUIKeyCode, int>	_nativeToWin32;

	NativeUI_Win32_KeyMap() {
		using KeyCode = NativeUIKeyCode;
		for (u8 i = 0; i < 26; ++i) { addEntry(KeyCode::A       + i, 'A'        + i); }
		for (u8 i = 0; i < 10; ++i) { addEntry(KeyCode::Digi0   + i, '0'        + i); }
		for (u8 i = 0; i < 10; ++i) { addEntry(KeyCode::NumPad0 + i, VK_NUMPAD0 + i); }
		for (u8 i = 0; i < 24; ++i) { addEntry(KeyCode::F1      + i, VK_F1      + i); }

		addEntry(KeyCode::Backspace                 , VK_BACK                 );
		addEntry(KeyCode::Tab                       , VK_TAB                  );
		addEntry(KeyCode::Clear                     , VK_CLEAR                );
		addEntry(KeyCode::Enter                     , VK_RETURN               );
		addEntry(KeyCode::Shift                     , VK_SHIFT                );
		addEntry(KeyCode::Ctrl                      , VK_CONTROL              );
		addEntry(KeyCode::Alt                       , VK_MENU                 );
		addEntry(KeyCode::Pause                     , VK_PAUSE                );
		addEntry(KeyCode::CapsLock                  , VK_CAPITAL              );
		addEntry(KeyCode::IME_Kana                  , VK_KANA                 );
		addEntry(KeyCode::IME_Hangul                , VK_HANGUL               );
		addEntry(KeyCode::IME_Junja                 , VK_JUNJA                );
		addEntry(KeyCode::IME_Final                 , VK_FINAL                );
		addEntry(KeyCode::IME_Hanja                 , VK_HANJA                );
		addEntry(KeyCode::IME_Kanji                 , VK_KANJI                );
		addEntry(KeyCode::Escape                    , VK_ESCAPE               );
		addEntry(KeyCode::IME_Convert               , VK_CONVERT              );
		addEntry(KeyCode::IME_NonConvert            , VK_NONCONVERT           );
		addEntry(KeyCode::IME_Accept                , VK_ACCEPT               );
		addEntry(KeyCode::IME_ModeChange            , VK_MODECHANGE           );
		addEntry(KeyCode::Space                     , VK_SPACE                );
		addEntry(KeyCode::PageUp                    , VK_PRIOR                );
		addEntry(KeyCode::PageDown                  , VK_NEXT                 );
		addEntry(KeyCode::End                       , VK_END                  );
		addEntry(KeyCode::Home                      , VK_HOME                 );
		addEntry(KeyCode::LeftArrow                 , VK_LEFT                 );
		addEntry(KeyCode::UpArrow                   , VK_UP                   );
		addEntry(KeyCode::RightArrow                , VK_RIGHT                );
		addEntry(KeyCode::DownArrow                 , VK_DOWN                 );
		addEntry(KeyCode::Select                    , VK_SELECT               );
		addEntry(KeyCode::Print                     , VK_PRINT                );
		addEntry(KeyCode::Execute                   , VK_EXECUTE              );
		addEntry(KeyCode::PrintScreen               , VK_SNAPSHOT             );
		addEntry(KeyCode::Insert                    , VK_INSERT               );
		addEntry(KeyCode::Delete                    , VK_DELETE               );
		addEntry(KeyCode::Help                      , VK_HELP                 );
		addEntry(KeyCode::LWin                      , VK_LWIN                 );
		addEntry(KeyCode::RWin                      , VK_RWIN                 );
		addEntry(KeyCode::Apps                      , VK_APPS                 );
		addEntry(KeyCode::Sleep                     , VK_SLEEP                );
		addEntry(KeyCode::NumPadMul                 , VK_MULTIPLY             );
		addEntry(KeyCode::NumPadAdd                 , VK_ADD                  );
		addEntry(KeyCode::Separator                 , VK_SEPARATOR            );
		addEntry(KeyCode::NumPadSub                 , VK_SUBTRACT             );
		addEntry(KeyCode::NumPadDecimal             , VK_DECIMAL              );
		addEntry(KeyCode::NumPadDiv                 , VK_DIVIDE               );
		addEntry(KeyCode::NumLock                   , VK_NUMLOCK              );
		addEntry(KeyCode::ScrollLock                , VK_SCROLL               );
		addEntry(KeyCode::NumPadEqual               , VK_OEM_NEC_EQUAL        );
		addEntry(KeyCode::LShift                    , VK_LSHIFT               );
		addEntry(KeyCode::RShift                    , VK_RSHIFT               );
		addEntry(KeyCode::LCtrl                     , VK_LCONTROL             );
		addEntry(KeyCode::RCtrl                     , VK_RCONTROL             );
		addEntry(KeyCode::LAlt                      , VK_LMENU                );
		addEntry(KeyCode::RAlt                      , VK_RMENU                );
		addEntry(KeyCode::BrowserBack               , VK_BROWSER_BACK         );
		addEntry(KeyCode::BrowserForward            , VK_BROWSER_FORWARD      );
		addEntry(KeyCode::BrowserRefresh            , VK_BROWSER_REFRESH      );
		addEntry(KeyCode::BrowserStop               , VK_BROWSER_STOP         );
		addEntry(KeyCode::BrowserBack               , VK_BROWSER_SEARCH       );
		addEntry(KeyCode::BrowserFavorites          , VK_BROWSER_FAVORITES    );
		addEntry(KeyCode::BrowserHome               , VK_BROWSER_HOME         );
		addEntry(KeyCode::VolumeMute                , VK_VOLUME_MUTE          );
		addEntry(KeyCode::VolumeDown                , VK_VOLUME_DOWN          );
		addEntry(KeyCode::VolumeUp                  , VK_VOLUME_UP            );
		addEntry(KeyCode::MediaNextTrack            , VK_MEDIA_NEXT_TRACK     );
		addEntry(KeyCode::MediaPrevTrack            , VK_MEDIA_PREV_TRACK     );
		addEntry(KeyCode::MediaStop                 , VK_MEDIA_STOP           );
		addEntry(KeyCode::MediaPlayPause            , VK_MEDIA_PLAY_PAUSE     );
		addEntry(KeyCode::LaunchMail                , VK_LAUNCH_MAIL          );
		addEntry(KeyCode::LaunchMediaSelect         , VK_LAUNCH_MEDIA_SELECT  );
		addEntry(KeyCode::LaunchMediaApp1           , VK_LAUNCH_APP1          );
		addEntry(KeyCode::LaunchMediaApp2           , VK_LAUNCH_APP2          );
		addEntry(KeyCode::SemiColon                  , VK_OEM_1               ); // [ ;: ] key
		addEntry(KeyCode::Equals                     , VK_OEM_PLUS            ); // [ =+ ] key
		addEntry(KeyCode::Comma                      , VK_OEM_COMMA           ); // [ ,< ] key
		addEntry(KeyCode::Minus                      , VK_OEM_MINUS           ); // [ -_ ] key
		addEntry(KeyCode::Period                     , VK_OEM_PERIOD          ); // [ .> ] key
		addEntry(KeyCode::Slash                      , VK_OEM_2               ); // [ /? ] key
		addEntry(KeyCode::BackQuote                  , VK_OEM_3               ); // [ `~ ] key
		addEntry(KeyCode::LBracket                   , VK_OEM_4               ); // [ [{ ] for US
		addEntry(KeyCode::BackSlash                  , VK_OEM_5               ); // [ \| ] for US
		addEntry(KeyCode::RBracket                   , VK_OEM_6               ); // [ ]} ] for US
		addEntry(KeyCode::Quotes                     , VK_OEM_7               ); // [ '" ] for US
		addEntry(KeyCode::ProcessKey                 , VK_PROCESSKEY          );
	}

	void addEntry(NativeUIKeyCode code, int w) {
		_win32ToNative.add(w, code);
		_nativeToWin32.add(code, w);
	}
};

NativeUIKeyCode NativeUI_Win32::s_toNativeKey(int key) {
	auto& map = NativeUI_Win32_KeyMap::s_instance()->_win32ToNative;
	return ax_ptr_value_or(map.find(key), NativeUIKeyCode::None);
}

int NativeUI_Win32::s_toWin32Key(NativeUIKeyCode key) {
	auto& map = NativeUI_Win32_KeyMap::s_instance()->_nativeToWin32;
	return ax_ptr_value_or(map.find(key), 0);
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
	if (::GetParent(hwnd)) {
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
	if (::GetParent(hwnd)) {
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
	auto rc = s_getLocalRect(hwnd);
	rc.pos = s_localToWorldPos(hwnd, rc.pos);
	return rc;
}

Rect2f NativeUI_Win32::s_getLocalRect(HWND hwnd) {
	RECT rc;
	::GetWindowRect(hwnd, &rc);
	return Rect2f::s_from(rc);
}


} // namespace

#endif // #if AX_NATIVE_UI_WIN32
