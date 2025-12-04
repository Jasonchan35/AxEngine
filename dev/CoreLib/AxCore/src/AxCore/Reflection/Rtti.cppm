module;
#include "AxCore-pch.h"
#include "AxRtti.h"

export module AxCore.Rtti;
export import AxCore.NameId;
export import AxCore.WPtr;
export import AxCore.MetaType;

export namespace ax {

struct Rtti;

struct MutRttiField : public NonCopyable {
	NameId	name;
	Rtti*	fieldType = nullptr;
	Rtti*	fieldOwner = nullptr;
};

using RttiField = const MutRttiField;


struct Rtti : public NonCopyable {
	Rtti* baseRtti = nullptr;
	NameId name;
	Array<RttiField*> ownFields;
	Array<RttiField*> allFields;

	void DebugDump();
};

template<class T> struct Rtti_;

template<class T> struct Rtti_Handler_ {
	static Rtti* s_rtti() {
		static Rtti_<T> s;
		return &s;
	}
};

template<class T> Rtti* rttiOf() { return Rtti_Handler_<T>::s_rtti(); }

template<>
struct Rtti_<NoBaseClass> {

}; 

template<class T>
struct Rtti_ : public Rtti {
	Rtti_() { _ctor(); }
private:
	using ObjThis      = T;
	using ObjBase      = BaseClassOf<T>;

	using MetaType     = MetaType_Of_<T>;
	using BaseMetaType = MetaType_Of_<ObjBase>;

	static constexpr bool noBase = std::is_same_v<ObjBase, NoBaseClass>; 
	
	struct OwnField_Handler {
		template<Int Index, class Field>
		static void onEach(Rtti_* rtti) {
			static MutRttiField field;
			field.name = Field::s_name();
			using FieldType = typename Field::FieldType;
			field.fieldOwner = rtti;
			field.fieldType = rttiOf<FieldType>();
			rtti->ownFields.emplaceBack(&field);
		}
	};

	void _ctor() {
//		static_assert(Type_IsBaseOf<IMetaType, MetaType>, "MetaType must based on IMetaType");

		if constexpr (noBase) {
			baseRtti = nullptr;
		} else {
			baseRtti = rttiOf<ObjBase>();
		}
		
		this->name = MetaType::s_name();
		using OwnFields = typename MetaType::OwnFields;
		
		this->ownFields.reserve(OwnFields::Size);
		OwnFields::template ForEachType<OwnField_Handler>(this);
		
		//---- All Fields -----
		if (baseRtti) {
			this->allFields.reserve(baseRtti->allFields.size() + OwnFields::Size);
			this->allFields = baseRtti->allFields;
		}
		for (auto* field : this->ownFields) {
			this->allFields.emplaceBack(field);
		}
	}

};


class RttiObject : public WPtrReferenable {
	AX_TYPE_INFO(RttiObject, NoBaseClass)
public:
	virtual ~RttiObject() = default;
	
	using _TYPE_INFO_Base = NoBaseClass;
	struct MetaTypeInit : AX_META_TYPE_INIT(This) {};
	
	static  Rtti* s_rtti ()		{ return rttiOf<This>(); }
	virtual Rtti* rtti() const 	{ return rttiOf<This>(); }
};


} // namespace

