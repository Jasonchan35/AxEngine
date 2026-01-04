module;

export module AxNativeUI:NativeUIWindow_Base;
export import :Common;
export import :NativeUIScreen;

export namespace ax {

struct NativeUIWindow_CreateDesc {
	NativeUIWindow_CreateDesc()
		: rect(100, 100, 320, 200)
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
	void _onNativeUIMouseEvent(NativeUIMouseEvent& ev);
	void _onNativeUIKeyEvent  (NativeUIKeyEvent&   ev);
	
	virtual	void onUIMouseEvent(UIMouseEvent& ev) {}
	virtual	void onUIKeyEvent  (UIKeyEvent&   ev) {}
	
	virtual	void onRender() {}
	
private:
	Rect2f		_worldRect = TagZero;
	Margin2f	_padding = TagZero;
	bool		_active : 1;
};

inline
NativeUIWindow_Base::NativeUIWindow_Base(CreateDesc& desc) {
	_active = false;
}

inline
void NativeUIWindow_Base::setActive(bool b) {
	onNativeSetActive(b);
}

inline
void NativeUIWindow_Base::setVisible(bool b) {
	onNativeSetVisible(b);
}

inline
void NativeUIWindow_Base::setWorldPos(const Vec2f& pos) {
	if (_worldRect.pos == pos) return;
	onNativeSetWorldPos(pos);
}

inline
void NativeUIWindow_Base::setSize(const Vec2f& size) {
	if (_worldRect.size == size) return;
	onNativeSetSize(size);
}

inline
void NativeUIWindow_Base::setWindowTitle(StrView title) {
	onNativeSetWindowTitle(title);
}

inline
void NativeUIWindow_Base::_onNativeUIMouseEvent(NativeUIMouseEvent& ev) {
	auto* mgr = UIEventManager::s_instance();
	UIMouseEvent outEvent;
	mgr->_translateNativeUIMouseEvent(outEvent, ev);
	onUIMouseEvent(outEvent);
}

inline
void NativeUIWindow_Base::_onNativeUIKeyEvent(NativeUIKeyEvent& ev) {
	auto* mgr = UIEventManager::s_instance();
	UIKeyEvent outEvent;
	mgr->_translateNativeUIKeyEvent(outEvent, ev);
	onUIKeyEvent(outEvent);
}

} // namespace