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
consteval
StrViewA ax_metatype_get_class_name() {
	auto* sig = AX_FUNC_SIG;
	auto src = StrView_c_str(sig);

#if AX_COMPILER_VC	
	auto t = StrViewA("ax_metatype_get_class_name");
	if (auto pos = src.find(t)) {
		return src.sliceFrom(pos.value());
	} else {
//		AX_ASSERT(false)
		return src;
	}
#else
	return src;
#endif
}

struct MetaTypeBase : public NonCopyable {
	MetaTypeBase() = delete;
	static NameId s_name() { return NameId(); }
	using OwnFields = Tuple<>;
};

struct NoBaseClass {
	using MetaType = MetaTypeBase;
};

template<class T>
struct MetaTypeOf_Handler_ {
	using MetaType = typename T::MetaType;
};

template<class T> using MetaTypeOf_ = typename MetaTypeOf_Handler_<T>::MetaType;

template<class T, StrView (*NAME_FUNC)() >
struct MetaType_Make_ : public MetaTypeOf_<typename T::_TYPE_INFO_Base> {
	using T_Base = typename T::_TYPE_INFO_Base;
	using Base_MetaType = MetaTypeOf_<T_Base>;
	
//	static NameId s_name() { return AX_NAMEID(NAME_FUNC()); }
	static NameId s_name() {
		return AX_NAMEID(ax_metatype_get_class_name<T>());
	}

	~MetaType_Make_() {
		static_assert(Type_IsBaseOf<MetaTypeBase, Base_MetaType>);
	}
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
struct MetaType_Simple_ : public MetaTypeBase {
	static NameId s_name() { return AX_NAMEID(NAME_FUNC()); }
};

AX_SIMPLE_META_TYPE(i8)
AX_SIMPLE_META_TYPE(i16)
AX_SIMPLE_META_TYPE(i32)
AX_SIMPLE_META_TYPE(i64)
AX_SIMPLE_META_TYPE(u8)
AX_SIMPLE_META_TYPE(u16)
AX_SIMPLE_META_TYPE(u32)
AX_SIMPLE_META_TYPE(u64)
AX_SIMPLE_META_TYPE(f32)
AX_SIMPLE_META_TYPE(f64)

} // namespace
