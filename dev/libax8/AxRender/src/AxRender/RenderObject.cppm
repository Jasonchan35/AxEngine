module;
export module AxRender:RenderObject;
export import :Common;

export namespace ax {

class RenderObject : public RttiObject {
	AX_RTTI_INFO(RenderObject, RttiObject)
public:

#if AX_RENDER_DEBUG_NAME
	void setDebugName(InNameId name) { _name = name; onSetDebugName(_name); }
#endif

	NameId	name() const { return _name; }

protected:
	NameId	_name;
#if AX_RENDER_DEBUG_NAME
	virtual void onSetDebugName(NameId name) {}
#endif
};

} // namespace