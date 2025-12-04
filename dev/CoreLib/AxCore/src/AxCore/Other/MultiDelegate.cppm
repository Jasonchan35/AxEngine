module;
#include "AxCore-pch.h"
export module AxCore.MultiDelegate;
export import AxCore.Delegate;
import AxCore.LinkedList;

export namespace ax {

template<bool ENTER_ONCE, class FUNC_SIG>
class EventBase;

template<bool ENTER_ONCE, class RETURN_TYPE, class... ARGS>
class EventBase<ENTER_ONCE, RETURN_TYPE (ARGS...)> : public NonCopyable {
public:
	static constexpr bool kEnterOnce = ENTER_ONCE;

	using ReturnType	= RETURN_TYPE;	

						using StaticFunc	= ReturnType (*)(ARGS...);
	template<class OBJ>	using ObjFunc		= ReturnType (*)(OBJ* obj, ARGS...);
	template<class OBJ>	using MemFunc		= ReturnType (OBJ::*)(ARGS...);

						void bindStatic(                StaticFunc func)	{ _bindStatic(func); }

	template<class OBJ>	void bindWPtr(SPtr<OBJ>&  obj, ObjFunc<OBJ> func)	{ _bindWPtr(obj, func); }
	template<class OBJ>	void bindWPtr(SPtr<OBJ>&  obj, MemFunc<OBJ> func)	{ _bindWPtr(obj, func); }

	template<class OBJ>	void bindUnowned(OBJ* obj, ObjFunc<OBJ> func)	{ _bindUnowned(obj, func); }
	template<class OBJ>	void bindUnowned(OBJ* obj, MemFunc<OBJ> func)	{ _bindUnowned(obj, func); }

						template<class LAMBDA>
						void bindLambda(LAMBDA func)	{ _bindLambda(func); }

	//-------------------
						void unbindAll()									{ _unbindAll(); }
						void unbindStatic(StaticFunc func)					{ _unbindStatic(func); }

	template<class OBJ>	void unbindWPtr(SPtr<OBJ>& obj, MemFunc<OBJ> func)	{ _unbind(obj.ptr(), func); }
	template<class OBJ>	void unbindWPtr(SPtr<OBJ>& obj, ObjFunc<OBJ> func)	{ _unbind(obj.ptr(), func); }

	template<class OBJ>	void unbindUnowned(OBJ*    obj, MemFunc<OBJ> func)	{ _unbind(obj, func); }
	template<class OBJ>	void unbindUnowned(OBJ*    obj, ObjFunc<OBJ> func)	{ _unbind(obj, func); }

	template<class OBJ> void unbindByObject(     OBJ*  obj)							{ _unbindByObject(obj); }

	AX_INLINE void invoke(ARGS... args)			{ _invoke(AX_FORWARD(args)...); }
	AX_INLINE bool valid() const				{ return _nodes.size() != 0; }
	AX_INLINE explicit operator bool () const	{ return valid(); }

private:
	using Delegate = Delegate_<void (ARGS...)>;

	struct Node : public LinkedListNode<Node> {
		Delegate dg;
	};

	void _unbindAll() {
		_nodes.clear();
	}

	void _bindStatic(StaticFunc func)	{ _nodes.append(UPtr_new<Node>(AX_ALLOC_REQ))->dg.bindStatic(func); }

	template<class LAMBDA>
	void _bindLambda(LAMBDA func)		{ _nodes.append(UPtr_new<Node>(AX_ALLOC_REQ))->dg.bindLambda(func); }

	template<class OBJ, class FUNC>
	void _bindWPtr(SPtr<OBJ>& obj, FUNC func) { if (!obj) return; _nodes.append(UPtr_new<Node>(AX_ALLOC_REQ))->dg.bindWPtr(obj, func); }

	template<class OBJ, class FUNC>
	void _bindUnowned(OBJ* obj, FUNC func) { if (!obj) return; _nodes.append(UPtr_new<Node>(AX_ALLOC_REQ))->dg.bindUnowned(obj, func); }

#if AX_HAS_AWX_WIDGETS
	template<class OBJ, class FUNC>
	void _bindWxWeak(OBJ* obj, FUNC func) { if (!obj) return; _nodes.append(UPtr_new<Node>(AX_ALLOC_REQ))->dg.bindWxWeak(obj, func); }	
#endif

	void _unbindStatic(StaticFunc func)	{ _nodes.removeIf([&](auto& p) { return p->dg.hasBinding(func); }); }

	template<class OBJ, class FUNC>
	void _unbind(OBJ* obj, FUNC func)	{ _nodes.removeIf([&](auto& p) { return p->dg.hasBinding(obj, func); }); }

	template<class OBJ>
	void _unbindByObject(OBJ* obj)		{ _nodes.removeIf([&](auto& p) { return p->dg.hasObject(obj); }); }

	void _invoke(ARGS... args) {
		if constexpr (!kEnterOnce) {
			_internalInvoke(AX_FORWARD(args)...);
		} else if (auto re = _enterOnce.enter()) {
			_internalInvoke(AX_FORWARD(args)...);
		}
	}

	void _internalInvoke(ARGS... args) {	
		auto* p = _nodes.head();
		while (p) {
			auto* next = p->listNode_next;
			bool valid = true;
			p->dg._invokeValid(valid, AX_FORWARD(args)...);
			if (!valid) {
				_nodes.remove(p);
			}
			p = next;
		}
	}

	LinkedList<Node> _nodes;
	ScopeEnterOnce _enterOnce;
};

template<class FUNC_SIG> using Event			= EventBase<true,  FUNC_SIG>;
template<class FUNC_SIG> using ReEntrantEvent	= EventBase<false, FUNC_SIG>;

} // namespace 