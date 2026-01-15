module;
module AxImUI;
import :ImUIView;

namespace ax {

void ImUIView::setSize(const Vec2f& s)				{ onSetSize(s); }
void ImUIView::onSetSize(const Vec2f& s)			{ onSizeChanged(s); }
void ImUIView::onSizeChanged(const Vec2f& s)		{ _rect.size = s; }

void ImUIView::setWorldPos(const Vec2f& pos)		{ onSetWorldPos(pos); }
void ImUIView::onSetWorldPos(const Vec2f& pos)		{ onWorldPosChanged(pos); }
void ImUIView::onWorldPosChanged(const Vec2f& pos)	{
	auto parentPos = _parent ? _parent->worldPos() : Vec2f(0,0);
	_rect.pos = pos - parentPos;
	_setWorldPosDirty();
}

const Vec2f& ImUIView::worldPos() const {
	if (_worldPosDirty) {
		ax_const_cast(this)->_computeWorldPos();
	}
	return _worldPos;
}

void ImUIView::_computeWorldPos() {
	_worldPosDirty = false;
	_worldPos = _parent ? _parent->worldPos() : Vec2f(0,0);
	_worldPos += _rect.pos;
}

void ImUIView::addChild(ImUIView* p) {
	if (!p) return;
	if (p->_parent) {
		p->_parent->_children.eraseAllIfEquals(p);
	}

	p->_parent = this;
	_children.append(p);
}

void ImUIView::_setWorldPosDirty() {
	if (_worldPosDirty) return;
	_worldPosDirty = true;
	for (auto& c : _children) {
		if (c) c->_setWorldPosDirty();
	}
}

} // namespace