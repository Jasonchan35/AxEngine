module;

export module AxNativeUI:NativeUIScreen;
export import :NativeUITimer;

export namespace ax {

class NativeUIScreen { // copyable
public:

#if AX_NATIVE_UI_WIN32
	using Handle = HMONITOR;
	const Handle kInvalid = nullptr;

#elif AX_NATIVE_UI_MACOSX
	using Handle = NSScreen*;
	const Handle kInvalid = nil;
	
#elif AX_NATIVE_UI_X11
	using Handle = AX_X11_Screen*;
	const Handle kInvalid = nullptr;

#else
	using Handle = Int;
	const Handle kInvalid = 0;

#endif

	NativeUIScreen() = default;
	NativeUIScreen(Handle h) : _h(h) {}

	Handle	handle() { return _h; }

	Rect2f	worldRect();

private:
	Handle	_h = kInvalid;
};


#if AX_NATIVE_UI_WIN32

Rect2f	NativeUIScreen::worldRect() {
	MONITORINFO info = {};
	info.cbSize = sizeof(info);
	if (_h) {
		if (GetMonitorInfoA(_h, &info)) {
			return Rect2f::s_from(info.rcWork);
		}
	}
	return Rect2f(0,0,0,0);
}

#elif AX_NATIVE_UI_MACOSX

Rect2f	NativeUIScreen::worldRect() {
	if (!_h) return Rect2f(0,0,0,0);
	auto rc = Rect2f([_h frame]);
	return rc;
}

#elif AX_NATIVE_UI_X11

Rect2f NativeUIScreen::worldRect() {
	if (!_h) return Rect2f(0,0,0,0);
	return Rect2f(0, 0, _h->width, _h->height);
}

#else
Rect2f NativeUIScreen::worldRect() { return Rect2f::s_zero(); }

#endif

} // namespace