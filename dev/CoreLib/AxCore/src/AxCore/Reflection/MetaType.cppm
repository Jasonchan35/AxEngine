module;
#include "AxCore-pch.h"
#include "MetaType_MACRO.h"

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

struct IMetaTypeInit : public NonCopyable {
	IMetaTypeInit() = delete;
	static NameId s_name() { return NameId(); }
	using OwnFields = Tuple<>;
	using OwnAttrs  = Tuple<>;
};

struct NoBaseClass {
	NoBaseClass() = delete;
};

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
struct MetaTypeInit_Handler_ {
	using MetaTypeInit = typename T::MetaTypeInit;
};

template<>
struct MetaTypeInit_Handler_<NoBaseClass> {
	using MetaTypeInit = IMetaTypeInit;
};

template<class T> struct FinalMetaTypeOf_Handler_;

template<>
struct FinalMetaTypeOf_Handler_<NoBaseClass> {
	struct FinalMetaType : IMetaTypeInit {
		using AllFields = Tuple<>;
		using AllAttrs  = Tuple<>;
	};
};

template<class T> struct MetaTypeInit_Helper_;

template<class T>
struct FinalMetaTypeOf_Handler_ {
	using ObjThis                    = T;
	using ObjBase                    = BaseClassOf<T>;
	using Base_Handler               = FinalMetaTypeOf_Handler_<ObjBase>;
	using Base_MetaTypeInit          = typename MetaTypeInit_Handler_<ObjBase>::MetaTypeInit;
	using Base_FinalMetaType         = typename Base_Handler::FinalMetaType;
	using Base_AllFields             = typename Base_FinalMetaType::AllFields;
	using Base_AllAttrs              = typename Base_FinalMetaType::AllAttrs;
	using Potential_MetaTypeInit     = typename MetaTypeInit_Handler_<ObjThis>::MetaTypeInit;
	static constexpr bool hasOwnInit = !std::is_same_v<Potential_MetaTypeInit, Base_MetaTypeInit>;

	struct Empty_MetaTypeInit : public MetaTypeInit_Helper_<ObjBase> {};

	using MetaTypeInit = std::conditional_t<hasOwnInit, Potential_MetaTypeInit, Empty_MetaTypeInit>;
	using OwnFields    = typename MetaTypeInit::OwnFields;
	using OwnAttrs     = typename MetaTypeInit::OwnAttrs;
	
	struct FinalMetaType : public MetaTypeInit {
		//---- Combind all ---
		using AllFields = typename Base_AllFields::template JoinType<OwnFields>;
		using AllAttrs  = typename Base_AllAttrs ::template JoinType<OwnAttrs >;
	};
};

//template<class T> using MetaTypeInit_Of_	= typename MetaTypeInit_Handler_<T>::MetaTypeInit;
template<class T> using MetaTypeOf			= FinalMetaTypeOf_Handler_<T>::FinalMetaType;
// template<class T> using MetaType_OfBase_	= FinalMetaTypeOf_Handler_< BaseClassOf<T> >;

template<class T>
struct MetaTypeInit_Helper_ : public MetaTypeOf< BaseClassOf<T> > {
	using ObjThis = T;
	using ObjBase = typename T::_TYPE_INFO_Base;
	static NameId s_name() { static auto s = NameId::s_make(ax_metatype_get_class_name<T>()); return s; }

	using OwnFields = Tuple<>;
	using OwnAttrs  = Tuple<>;
};

struct MetaAttr {
	static StrView	name() { return "MetaAttr"; }
};

struct MetaFieldBase : public NonCopyable {
	MetaFieldBase() = delete;
	using OwnFields = Tuple<>;
};

template<class OBJ, class FIELD, FIELD OBJ::*PTR, NameId (*NAME_FUNC)() >
struct MetaFieldInit_Helper_ : public MetaFieldBase {
	static NameId s_name() { static auto s(NAME_FUNC()); return s; }
	static Int s_offset() { return offsetof(OBJ, PTR); }

	using FieldType = FIELD;
};

template<class T, NameId (*NAME_FUNC)()>
struct MetaTypeInit_Simple_ : public IMetaTypeInit {
	using This = T;
	using Base = NoBaseClass;

	static NameId s_name() { static auto s = NAME_FUNC(); return s; }
};

AX_META_TYPE_INIT_SIMPLE(i8)
AX_META_TYPE_INIT_SIMPLE(i16)
AX_META_TYPE_INIT_SIMPLE(i32)
AX_META_TYPE_INIT_SIMPLE(i64)
AX_META_TYPE_INIT_SIMPLE(u8)
AX_META_TYPE_INIT_SIMPLE(u16)
AX_META_TYPE_INIT_SIMPLE(u32)
AX_META_TYPE_INIT_SIMPLE(u64)
AX_META_TYPE_INIT_SIMPLE(f32)
AX_META_TYPE_INIT_SIMPLE(f64)

} // namespace
