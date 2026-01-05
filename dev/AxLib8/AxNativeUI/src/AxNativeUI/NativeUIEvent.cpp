module AxNativeUI;
import :NativeUIEvent;

namespace ax {

UIEventManager* UIEventManager::s_instance() {
	static GlobalSingleton<UIEventManager> s;
	return s.ptr();
}

void UIEventManager::_translateNativeUIMouseEvent(UIMouseEvent& outEvent, const NativeUIMouseEvent& inEvent) {
	NativeUIMouseEvent& copyEvent = outEvent;
	copyEvent = inEvent;
	
	if (_firstMouseEvent) {
		_firstMouseEvent = false;
		_lastMouseWorldPos = outEvent.worldPos;
	}

	outEvent.deltaPos  = copyEvent.worldPos - _lastMouseWorldPos;
	_lastMouseWorldPos = copyEvent.worldPos;
	
	switch (inEvent.type) {
		case UIMouseEventType::Down: {
			_lastPressedButton = ax_bit_set(_lastPressedButton, inEvent.button);
		} break;
		case UIMouseEventType::Up: {
			_lastPressedButton = ax_bit_unset(_lastPressedButton, inEvent.button);
		} break;
	}
	
	outEvent.pressedButton = _lastPressedButton;
}

void UIEventManager::_translateNativeUIKeyEvent(UIKeyEvent& outEvent, const NativeUIKeyEvent& inEvent) {
	NativeUIKeyEvent& copyEvent = outEvent;
	copyEvent = inEvent;

}

} // namespace