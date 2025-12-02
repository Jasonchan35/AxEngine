module;
#include "AxCore-pch.h"

export module AxCore.Rtti;
export import AxCore.WPtr;
export import AxCore.MetaType;

export namespace ax {

struct Rtti : public NonCopyable {
	StrView name() const { return _name; }
protected:
	StrView _name;
};

template<class T>
struct Rtti_ : public Rtti {
	using InfoBase = Rtti;
	using MetaType = MetaTypeOf<T>;
	
	Rtti_() {
		InfoBase::_name = MetaType::s_name();
	}
};

template<class T> struct Rtti_Handler_ {
	static Rtti* s_rtti() {
		static Rtti_<T> s;
		return &s;
	}
};

template<class T> Rtti* rttiOf() { return Rtti_Handler_<T>::s_rtti(); }

class RttiObject : public WPtrReferenable {
	AX_TYPE_INFO(RttiObject, NoBaseClass)
public:
	struct MetaType : public MetaTypeBase {};
	
	static  Rtti* s_rtti ()			{ return rttiOf<This>(); }
	virtual Rtti* getRtti() const 	{ return rttiOf<This>(); }
};


} // namespace

