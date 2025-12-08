module;

export module AxNativeUI:NativeUIApp_Base;
export import :NativeUIWindow_Base;

export namespace ax {

#define AX_NativeUIApi_ENUM_LIST(E) \
	E(Unknown,) \
	E(Win32,) \
	E(WinRT,) \
	E(X11,) \
	E(MacOSX_AppKit,) \
	E(iOS_UIKit,) \
//-----
AX_ENUM_CLASS(AX_NativeUIApi_ENUM_LIST, NativeUIApi, u8);

struct NativeUIApp_CreateDesc {
	bool peekMessage = false;
};

class NativeUIApp_Base : public AppBase {
	AX_RTTI_INFO(NativeUIApp_Base, AppBase)
public:
	using CreateDesc = NativeUIApp_CreateDesc;
	using Time = NativeUIEventTime;

	NativeUIApp_Base(const CreateDesc& desc);
	virtual ~NativeUIApp_Base() override;

	virtual void	quit		(int returnCode) = 0;

	bool setCursor(NativeUICursorType type) { return onSetNativeCursor(type); }
	bool setCursorPos(const Vec2f& worldPos) { return onSetNativeCursorPos(worldPos); }

	void requestCustomAppEvent();
	void handleCustomAppEvent(bool force);

	void updateLayout() { onUpdateLayout(); }

protected:
	virtual bool onRequestNativeCustomAppEvent() { return false; }
	virtual void onHandleCustomAppEvent() {}
	virtual void onUpdateLayout() {}
	virtual bool onSetNativeCursor(NativeUICursorType type) { return false; }
	virtual bool onSetNativeCursorPos(const Vec2f& worldPos) { return false; }
	virtual void onPeekMessage() {}

	CreateDesc _desc;

	bool _pendingCustomAppEvent : 1;
	int	_returnCode = 0;
};

inline
NativeUIApp_Base::NativeUIApp_Base(const CreateDesc& desc)
	: Base()
	, _desc(desc)
	, _pendingCustomAppEvent(false)
{
}

inline
NativeUIApp_Base::~NativeUIApp_Base() {
 
}

inline
void NativeUIApp_Base::requestCustomAppEvent() {
	if (_pendingCustomAppEvent) return;
	if (!onRequestNativeCustomAppEvent()) return;
	_pendingCustomAppEvent = true;
}

inline
void NativeUIApp_Base::handleCustomAppEvent(bool force) {
	if (_pendingCustomAppEvent || force) {
		_pendingCustomAppEvent = false;
		onHandleCustomAppEvent();
	}
}

} // namespace