module;
export module AxUI:UIView;
export import :PCH;

export namespace ax::AxUI {

class UIView : public RttiObject {
	AX_TYPE_INFO(UIView, RttiObject)
public:
	UIView() : _worldPosDirty(true) {}

	void setSize(const Vec2f& s);
	void setWorldPos(const Vec2f& pos);
	void setWorldRect(const Rect2f& rc) { setWorldPos(rc.pos); setSize(rc.size); }

	const	Rect2f&	rect() const { return _rect; }
	const	Vec2f&	worldPos() const;
			Rect2f	worldRect() const { return Rect2f(worldPos(), _rect.size); }

	void addChild(UIView* p);

protected:
	virtual void onSetWorldPos(const Vec2f& pos);
	virtual void onSetSize(const Vec2f& s);

	virtual void onSizeChanged(const Vec2f& s);
	virtual void onWorldPosChanged(const Vec2f& s);

private:
	void _computeWorldPos();
	void _setWorldPosDirty();

	Array<SPtr<UIView>, 4>	_children;

	UIView*	_parent = nullptr;
	Vec2f	_worldPos = AX_ZERO;
	Rect2f	_rect = AX_ZERO;

	bool	_worldPosDirty : 1;
};


} // namespace