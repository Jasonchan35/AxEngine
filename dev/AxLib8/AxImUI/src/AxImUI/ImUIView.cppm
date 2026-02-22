module;
export module AxImUI:ImUIView;
export import :Common;

export namespace ax {

class ImUIView : public RttiObject {
	AX_RTTI_INFO(ImUIView, RttiObject)
public:
	ImUIView() : _worldPosDirty(true) {}

	void setSize(const Vec2f& s);
	void setWorldPos(const Vec2f& pos);
	void setWorldRect(const Rect2f& rc) { setWorldPos(rc.pos); setSize(rc.size); }

	const	Rect2f&	rect() const { return _rect; }
	const	Vec2f&	worldPos() const;
			Rect2f	worldRect() const { return Rect2f(worldPos(), _rect.size); }

	void addChild(ImUIView* p);

protected:
	virtual void onSetWorldPos(const Vec2f& pos);
	virtual void onSetSize(const Vec2f& s);

	virtual void onSizeChanged(const Vec2f& s);
	virtual void onWorldPosChanged(const Vec2f& s);

private:
	void _computeWorldPos();
	void _setWorldPosDirty();

	Array<SPtr<ImUIView>, 4> _children;

	ImUIView* _parent   = nullptr;
	Vec2f     _worldPos = TagZero;
	Rect2f    _rect     = TagZero;
	bool      _worldPosDirty : 1;
};

struct ImUI_InspectorRequest : public NonCopyable {
	RenderRequest* renderRequest = nullptr;
};


} // namespace