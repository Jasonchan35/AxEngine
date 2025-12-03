module;
#include "AxCore-pch.h"

export module AxCore.MetaType;

export import AxCore.BasicType;
export import AxCore.Tuple;
export import AxCore.Formatter;
export import AxCore.NameId;

export namespace ax {

struct NoBaseClass {
	struct MetaType {
		static NameId s_name() { return NameId(); }
		using OwnFields = Tuple<>;
	};
};

struct MetaTypeBase : public NonCopyable {
	MetaTypeBase() = delete;

	static NameId s_name() { return NameId(); }

	using OwnFields = Tuple<>;
};

template<class T, StrView (*NAME_FUNC)()>
struct MetaType_ {
	using _MetaThis = T;
	static NameId s_name() { return NameId(NAME_FUNC()); }
	using OwnFields = Tuple<>;
};

struct MetaFieldBase : public NonCopyable {
	MetaFieldBase() = delete;
};

template<class OBJ, auto OBJ::*PTR, StrView (*NAME_FUNC)()>
struct MetaField_ : public MetaFieldBase {
	static NameId s_name() { return AX_NAME(NAME_FUNC()); }
	static Int s_offset() { return offsetof(OBJ, PTR); }
};

template<class T>
struct MetaTypeOf_Handler_ {
	using MetaType = T::MetaType;
};

template<class T> using MetaTypeOf_ = typename MetaTypeOf_Handler_<T>::MetaType;

} // namespace
