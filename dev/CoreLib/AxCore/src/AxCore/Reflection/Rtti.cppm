module;
#include "AxCore-pch.h"

export module AxCore.Rtti;
export import AxCore.NameId;
export import AxCore.WPtr;
export import AxCore.MetaType;

export namespace ax {

struct Rtti : public NonCopyable {
	NameId name;
};

struct RttiField : public NonCopyable {
	NameId name;
};

template<class T>
struct Rtti_ : public Rtti {
	using InfoBase = Rtti;
	using MetaType = MetaTypeOf_<T>;

	Array<RttiField> fields;

	struct OwnField_Handler {
		template<Int Index, class Field>
		static void onEach(Rtti_* This) {
			auto& f = This->fields.emplaceBack();
			f.name = Field::s_name();
		}
	};
	
	Rtti_() {
		InfoBase::name = MetaType::s_name();
		using OwnFields = typename MetaType::OwnFields;
		fields.reserve(OwnFields::Size);
		OwnFields::template ForEachType<OwnField_Handler>(this);
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
	using This = RttiObject;
public:
	struct MetaType : public MetaTypeBase {};
	
	static  Rtti* s_rtti ()		{ return rttiOf<This>(); }
	virtual Rtti* rtti() const 	{ return rttiOf<This>(); }
};


} // namespace

