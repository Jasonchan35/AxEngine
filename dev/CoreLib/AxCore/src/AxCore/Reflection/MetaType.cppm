module;
#include "AxCore-pch.h"

export module AxCore.MetaType;

export import AxCore.BasicType;
export import AxCore.Tuple;
export import AxCore.Formatter;

export namespace ax {

struct MetaTypeBase : public NonCopyable {
	MetaTypeBase() = delete;

	static StrView s_name() { return StrView(); }
};

template<class T, class BASE, StrView (*NAME_FUNC)()>
struct MetaType_ {
	static StrView s_name() { return NAME_FUNC(); }	
};

struct MetaFieldBase : public NonCopyable {
	MetaFieldBase() = delete;
};

template<class OBJ, auto OBJ::*PTR, StrView (*NAME_FUNC)()>
struct MetaField_ : public MetaFieldBase {
	static StrView s_name() { return NAME_FUNC(); }
	static Int s_offset() { return offsetof(OBJ, PTR); }
};

template<class T>
struct MetaTypeOf_ { using MetaType = T::MetaType; };

template<class T> using MetaTypeOf = typename MetaTypeOf_<T>::MetaType;

struct NoBaseClass {
};


} // namespace
