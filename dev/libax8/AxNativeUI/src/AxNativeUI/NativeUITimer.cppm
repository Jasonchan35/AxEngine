module;

export module AxNativeUI:NativeUITimer;
export import :NativeUIEvent;

export namespace ax {

//! Timer in UI Main Loop
class NativeUITimer : public NonCopyable {
public:
	using Time = NativeUIEventTime;
	using Delegate = Delegate_<void ()>;

	~NativeUITimer() { stop(); }

	void start(const Delegate& dg, Int intervalMillisecond, bool repeat);
	void stop();

	void _invoke() { _delegate.invoke(); }

private:
	void _start();
	void _stop();


#if AX_OS_WINDOWS
	static void CALLBACK s_onTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) noexcept;
	UINT_PTR _p = 0;
	
#elif AX_OS_MACOSX || AX_OS_IOS
	axNSObj<AxNativeUITimer_Obj> _p;

#elif AX_NATIVE_UI_X11
	friend class NativeUIApp_X11;
protected:
	void _updateNextFireTime();
	Time	_nextFireTime;
	bool	_addedToUIApp = false;

private:
#endif
	Delegate	_delegate;
	Int			_intervalMillisecond = 0;
	bool		_repeats;
};

} // namespace


namespace ax {

void NativeUITimer::start(const Delegate& dg, Int intervalMillisecond, bool repeats) {
	stop();

	_delegate = dg;
	_intervalMillisecond = intervalMillisecond;
	_repeats = repeats;
	_start();
}

void NativeUITimer::stop() {
	_stop();
}

} // namespace ax

#if AX_OS_WINDOWS

namespace ax {

class NativeUITimerManager : public NonCopyable {
	using This = NativeUITimerManager;
public:
	static This* s_instance() {
		static GlobalSingleton<This> s;
		return s.ptr();
	}

	Dict<UINT_PTR, NativeUITimer*>	_timerDict;
};

void NativeUITimer::_start() {
	UINT t = SafeCast(_intervalMillisecond);
	_p = ::SetTimer(nullptr, 0, t, s_onTimer);
	NativeUITimerManager::s_instance()->_timerDict.add(_p, this);
}

void NativeUITimer::_stop() {
	if (_p) {
		NativeUITimerManager::s_instance()->_timerDict.erase(_p);
		::KillTimer(nullptr, _p);
		_p = 0;
	}
}

void CALLBACK NativeUITimer::s_onTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) noexcept {
	if (!idEvent) {
		AX_ASSERT(false);
		return;
	}

	auto** ppTimer = NativeUITimerManager::s_instance()->_timerDict.find(idEvent);
	if (!ppTimer) {
		AX_ASSERT(false);
		return;
	}
	
	auto& timer = **ppTimer;

	if (!timer._repeats) {
		timer._stop();
	}
	timer._invoke();
}

} // namespace ax

#elif AX_OS_MACOSX || AX_OS_IOS

@interface AxNativeUITimer_Obj : NSObject {
	NativeUITimer* _owner;
	NSTimer* _timer;
}
@end

@implementation AxNativeUITimer_Obj
- (void)onTimer:(NSTimer*)timer {
	_owner->_invoke();
}

- (void)stop {
	if (_timer) {
		[_timer invalidate];
		_timer = nil;
	}
}

- (instancetype)init:(NativeUITimer*)owner interval:(NSTimeInterval)interval repeats:(bool)repeats {
	self = [super init];
	_owner = owner;
	_timer = [NSTimer scheduledTimerWithTimeInterval:interval target:self selector:@selector(onTimer:) userInfo:nil repeats:repeats];
	return self;
}
@end

namespace ax {

void NativeUITimer::_start() {
	auto t = _intervalMillisecond / 1000.0;
	_p.ref([[NativeUITimer_Obj alloc] init:this interval:t repeats:_repeats]);
}

void NativeUITimer::_stop() {
	if (_p) {
		[_p stop];
		_p.unref();
	}
}

} // namespace ax

#elif AX_NATIVE_UI_X11

namespace ax {

void NativeUITimer::_updateNextFireTime() {
	_nextFireTime.setToNow();
	_nextFireTime += Time::s_milliseconds(_intervalMillisecond);
}

void NativeUITimer::_start() {
	AX_ASSERT(!_addedToUIApp);

	_updateNextFireTime();	
	auto* app = NativeUIApp_X11::s_instance();
	_addedToUIApp = true;
	app->_timers.append(this);
}

void NativeUITimer::_stop() {
	if (_addedToUIApp) {
		auto* app = NativeUIApp_X11::s_instance();
		auto ret = app->_timers.findAndRemove(this);
		if (ret < 0) AX_ASSERT(false);
		_addedToUIApp = false;
	}
}

} // namespace ax

#elif AX_OS_ANDROID

namespace ax {

void NativeUITimer::_start() {
	AX_NOT_IMPLEMENTED();
}

void NativeUITimer::_stop() {
	AX_NOT_IMPLEMENTED();
}

} // namespace ax

#endif

