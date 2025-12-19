module;
export module AxRender:RenderObject;
export import :Common;

export namespace ax {

class RenderObject : public RttiObject {
	AX_RTTI_INFO(RenderObject, RttiObject)
public:

#if AX_RENDER_DEBUG_NAME
	void setDebugName(const StrView& name) { _name = name; onSetDebugName(_name); }
#endif

	StrView	name() const { return _name; }

protected:
	String	_name;
#if AX_RENDER_DEBUG_NAME
	virtual void onSetDebugName(const String& name) {}
#endif
};

} // namespace