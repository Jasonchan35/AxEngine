module;

export module AxNativeUI.Backend_Base;
export import AxNativeUI.PCH;
export import AxNativeUI.Screen;

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

class NativeUIApp_CreateDesc {
public:
	bool peekMessage = false;
};

struct NativeUIWindow_CreateDesc {
	NativeUIWindow_CreateDesc()
		: rect(100, 100, 8, 8)
		, titleBar(true)
		, isMainWindow(false)
		, visible(true)
		, resizable(true)
		, closeButton(true)
		, minButton(true)
		, maxButton(true)
		, centerToScreen(true)
		, alwaysOnTop(false)
	{}

	enum class Type {
		Unknown,
		NormalWindow,
		ToolWindow,
		PopupWindow,
	};

	Rect2f		rect = Rect2f::s_zero();
	Type		type = Type::NormalWindow;
	bool		titleBar		: 1;
	bool		isMainWindow	: 1;
	bool		visible			: 1;
	bool		resizable		: 1;
	bool		closeButton		: 1;
	bool		minButton		: 1;
	bool		maxButton		: 1;
	bool		centerToScreen	: 1;
	bool		alwaysOnTop		: 1;
};

class NativeUIWindow_Base : public RttiObject {
	AX_RTTI_INFO(NativeUIWindow_Base, RttiObject)
public:
	using CreateDesc = NativeUIWindow_CreateDesc;

	NativeUIWindow_Base(CreateDesc& desc);

	virtual void onCloseButton	() = 0;

			void setActive			(bool b);
	virtual void onNativeSetActive	(bool b) = 0;
	virtual void onActiveChanged	(bool b) { _active = b; }

			void setVisible			(bool b);
	virtual void onNativeSetVisible	(bool b) = 0;

			void setWorldPos		(const Vec2f& pos);
	virtual void onNativeSetWorldPos(const Vec2f& pos) = 0;
	virtual void onWorldPosChanged	(const Vec2f& pos)	{ _worldRect.pos  = pos; }

	const Rect2f&	worldRect		() const		{ return _worldRect; }
	const Vec2f&	worldPos		() const		{ return _worldRect.pos; }
	const Margin2f&	padding			() const		{ return _padding; }
		  Rect2f	contentWorldRect() const		{ return _worldRect - _padding; }
		  Vec2f		contentWorldPos	() const		{ return _worldRect.pos + _padding.topLeft(); }

			void setSize		(const Vec2f& size);
	virtual void onNativeSetSize(const Vec2f& size) = 0;
	virtual void onSizeChanged	(const Vec2f& size)		{ _worldRect.size = size; }

			void setWorldRect	(const Rect2f& rect)	{ setWorldPos(rect.pos); setSize(rect.size); }
			void setPadding		(const Margin2f& m)		{ _padding = m; }

			//! same as click close button by user
			void closeWindow	() { doCloseWindow(); }
	virtual void doCloseWindow	() = 0;
	
			void destroyWindow	() { doDestroyWindow(); }
	virtual void doDestroyWindow() = 0;

			void setWindowTitle			(StrView title);
	virtual void onNativeSetWindowTitle	(StrView title) = 0;

protected:
	virtual	void onNativeUIMouseEvent(NativeUIMouseEvent& ev) {}
	virtual	void onNativeUIKeyEvent(NativeUIKeyEvent& ev) {}
	virtual	void onRender() {}
	
private:
	Rect2f		_worldRect = AX_ZERO;
	Margin2f	_padding = AX_ZERO;
	bool		_active : 1;
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

} // namespace ax