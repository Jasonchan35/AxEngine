module;
module AxUI;
import :UIView;

namespace ax::AxUI {

void UIView::setSize(const Vec2f& s)				{ onSetSize(s); }
void UIView::onSetSize(const Vec2f& s)				{ onSizeChanged(s); }
void UIView::onSizeChanged(const Vec2f& s)			{ _rect.size = s; }

void UIView::setWorldPos(const Vec2f& pos)			{ onSetWorldPos(pos); }
void UIView::onSetWorldPos(const Vec2f& pos)		{ onWorldPosChanged(pos); }
void UIView::onWorldPosChanged(const Vec2f& pos)	{
	auto parentPos = _parent ? _parent->worldPos() : Vec2f(0,0);
	_rect.pos = pos - parentPos;
	_setWorldPosDirty();
}

const Vec2f& UIView::worldPos() const {
	if (_worldPosDirty) {
		ax_const_cast(this)->_computeWorldPos();
	}
	return _worldPos;
}

void UIView::_computeWorldPos() {
	_worldPosDirty = false;
	_worldPos = _parent ? _parent->worldPos() : Vec2f(0,0);
	_worldPos += _rect.pos;
}

void UIView::addChild(UIView* p) {
	if (!p) return;
	if (p->_parent) {
		p->_parent->_children.eraseAllIfEquals(p);
	}

	p->_parent = this;
	_children.append(p);
}

void UIView::_setWorldPosDirty() {
	if (_worldPosDirty) return;
	_worldPosDirty = true;
	for (auto& c : _children) {
		if (c) c->_setWorldPosDirty();
	}
}

} // namespace