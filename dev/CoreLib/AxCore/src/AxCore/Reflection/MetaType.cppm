module;
#include "AxCore-pch.h"
#include "AxMetaType.h"

export module AxCore.MetaType;

export import AxCore.BasicType;
export import AxCore.Tuple;
export import AxCore.Formatter;
export import AxCore.NameId;

export namespace ax {

template<class T> inline
#if AX_COMPILER_VC
	consteval
#else
	constexpr
#endif
StrViewA ax_metatype_get_class_name() {
	auto* sig = AX_FUNC_SIG;
	auto src = StrView_c_str(sig);

#if AX_COMPILER_VC	
	auto pair = src.split("ax_metatype_get_class_name<");
	pair = pair.second.splitBack(">(void)");
	return pair.first;
#else
	auto pair = src.split("ax_metatype_get_class_name() [T = ");
	pair = pair.second.splitBack("]");
	return pair.first;
#endif
}

struct IOwnMetaType : public NonCopyable {
	IOwnMetaType() = delete;
	static NameId s_name() { return NameId(); }
	using OwnFields = Tuple<>;
};

struct NoBaseClass {};

template<class T>
struct BaseClassOf_Handler_ {
	using Type = typename T::_TYPE_INFO_Base;
};

template<class T> requires Type_IsFundamental<T>
struct BaseClassOf_Handler_<T> {
	using Type = NoBaseClass;
};

template<class T> using BaseClassOf = typename BaseClassOf_Handler_<T>::Type;

template<class T>
struct OwnMetaTypeOf_Handler_ {
	using OwnMetaType = typename T::OwnMetaType;
};

template<>
struct OwnMetaTypeOf_Handler_<NoBaseClass> {
	using OwnMetaType = IOwnMetaType;
};

template<class T> struct FinalMetaTypeOf_Handler_;

template<>
struct FinalMetaTypeOf_Handler_<NoBaseClass> {
	struct FinalMetaType : IOwnMetaType {
		using AllFields = Tuple<>;
	};
};

template<class T> struct OwnMetaType_Make_;

template<class T>
struct FinalMetaTypeOf_Handler_ {
	using ObjThis                        = T;
	using ObjBase                        = BaseClassOf<T>;
	using Base_Handler                   = typename FinalMetaTypeOf_Handler_<ObjBase>;
	using Base_OwnMetaType               = typename OwnMetaTypeOf_Handler_<ObjBase>::OwnMetaType;
	using Base_FinalMetaType             = typename Base_Handler::FinalMetaType;
	using Base_AllFields                 = typename Base_FinalMetaType::AllFields;

	using Potential_OwnMetaType          = typename OwnMetaTypeOf_Handler_<ObjThis>::OwnMetaType;
	static constexpr bool hasOwnMetaType = !std::is_same_v<Potential_OwnMetaType, Base_OwnMetaType>;

	struct EmptyOwnMetaType : public OwnMetaType_Make_<ObjBase> {};
	
	using OwnMetaType		 = std::conditional_t<hasOwnMetaType, Potential_OwnMetaType, EmptyOwnMetaType>;
	using OwnFields          = typename OwnMetaType::OwnFields;
	
	struct FinalMetaType : public OwnMetaType {
		using AllFields = typename Base_AllFields::template JoinType<OwnFields>;
	};
};

template<class T> using OwnMetaTypeOf_ = typename OwnMetaTypeOf_Handler_<T>::OwnMetaType;
template<class T> using MetaTypeOf_		= FinalMetaTypeOf_Handler_<T>::FinalMetaType;
template<class T> using MetaTypeOfBase_ = FinalMetaTypeOf_Handler_< BaseClassOf<T> >;

template<class T>
struct OwnMetaType_Make_ : public MetaTypeOfBase_<T> {
	using ObjThis = T;
	using ObjBase = typename T::_TYPE_INFO_Base;
	static NameId s_name() { return AX_NAMEID(ax_metatype_get_class_name<T>()); }

	using OwnFields = Tuple<>;
}; 

struct MetaFieldBase : public NonCopyable {
	MetaFieldBase() = delete;
	using OwnFields = Tuple<>;
};

template<class OBJ, class FIELD, FIELD OBJ::*PTR, StrView (*NAME_FUNC)()>
struct MetaField_Make_ : public MetaFieldBase {
	static NameId s_name() { return AX_NAMEID(NAME_FUNC()); }
	static Int s_offset() { return offsetof(OBJ, PTR); }

	using FieldType = FIELD;
};

template<class T, StrView (*NAME_FUNC)()>
struct OwnMetaType_Simple_ : public IOwnMetaType {
	using This = T;
	using Base = NoBaseClass;

	static NameId s_name() { return AX_NAMEID(NAME_FUNC()); }
};

AX_SIMPLE_OWN_META_TYPE(i8)
AX_SIMPLE_OWN_META_TYPE(i16)
AX_SIMPLE_OWN_META_TYPE(i32)
AX_SIMPLE_OWN_META_TYPE(i64)
AX_SIMPLE_OWN_META_TYPE(u8)
AX_SIMPLE_OWN_META_TYPE(u16)
AX_SIMPLE_OWN_META_TYPE(u32)
AX_SIMPLE_OWN_META_TYPE(u64)
AX_SIMPLE_OWN_META_TYPE(f32)
AX_SIMPLE_OWN_META_TYPE(f64)

} // namespace
