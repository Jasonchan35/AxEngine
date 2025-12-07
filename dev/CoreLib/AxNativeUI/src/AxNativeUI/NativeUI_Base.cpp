module;

module AxNativeUI.Backend_Base;

namespace ax {

NativeUIWindow_Base::NativeUIWindow_Base(CreateDesc& desc) {
	_active = false;
}

void NativeUIWindow_Base::setActive(bool b) {
	onNativeSetActive(b);
}

void NativeUIWindow_Base::setVisible(bool b) {
	onNativeSetVisible(b);
}

void NativeUIWindow_Base::setWorldPos(const Vec2f& pos) {
	if (_worldRect.pos == pos) return;
	onNativeSetWorldPos(pos);
}

void NativeUIWindow_Base::setSize(const Vec2f& size) {
	if (_worldRect.size == size) return;
	onNativeSetSize(size);
}

void NativeUIWindow_Base::setWindowTitle(StrView title) {
	onNativeSetWindowTitle(title);
}

NativeUIApp_Base::NativeUIApp_Base(const CreateDesc& desc)
	: Base()
	, _desc(desc)
	, _pendingCustomAppEvent(false)
{
}

NativeUIApp_Base::~NativeUIApp_Base() {
 
}

void NativeUIApp_Base::requestCustomAppEvent() {
	if (_pendingCustomAppEvent) return;
	if (!onRequestNativeCustomAppEvent()) return;
	_pendingCustomAppEvent = true;
}

void NativeUIApp_Base::handleCustomAppEvent(bool force) {
	if (_pendingCustomAppEvent || force) {
		_pendingCustomAppEvent = false;
		onHandleCustomAppEvent();
	}
}


} // namespace 