module;
#include "AxCore-pch.h"
#include "AxRtti.h"

export module AxCore.Rtti;
export import AxCore.NameId;
export import AxCore.WPtr;
export import AxCore.MetaType;

export namespace ax {

struct Rtti;

struct RttiField : public NonCopyable {
	NameId	name;
	Rtti*	fieldType = nullptr;
};


struct Rtti : public NonCopyable {
	NameId name;
	Array<RttiField> fields;
};

template<class T> struct Rtti_;

template<class T> struct Rtti_Handler_ {
	static Rtti* s_rtti() {
		static Rtti_<T> s;
		return &s;
	}
};

template<class T> Rtti* rttiOf() { return Rtti_Handler_<T>::s_rtti(); }


template<class T>
struct Rtti_ : public Rtti {
	using Rtti_Base = Rtti;
	using T_MetaType = MetaTypeOf_<T>;
	
	struct OwnField_Handler {
		template<Int Index, class Field>
		static void onEach(Rtti_* This) {
			auto& f = This->fields.emplaceBack();
			f.name = Field::s_name();

			using FieldType = typename Field::FieldType;
			f.fieldType = rttiOf<FieldType>();
		}
	};
	
	Rtti_() {
		static_assert(Type_IsBaseOf<MetaTypeBase, T_MetaType>, "MetaType must based on MetaTypeBase");
		
		Rtti_Base::name = T_MetaType::s_name();
		using OwnFields = typename T_MetaType::OwnFields;
		fields.reserve(OwnFields::Size);
		OwnFields::template ForEachType<OwnField_Handler>(this);
	}

};


class RttiObject : public WPtrReferenable {
	using This = RttiObject;
public:
	struct MetaType : public MetaTypeBase {};
	
	static  Rtti* s_rtti ()		{ return rttiOf<This>(); }
	virtual Rtti* rtti() const 	{ return rttiOf<This>(); }
};


} // namespace

