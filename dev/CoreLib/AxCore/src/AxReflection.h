#pragma once

#include "AxBase.h"

import AxCore.Reflection;

#define AX_TYPE_INFO_WITH_NAME(T, BASE, NAME) \
private: \
	using This = T; \
	using Base = BASE; \
public: \
	using TypeInfo = ax_typeof<T>; \
	static StrView _InitTypeInfoName() { \
		static_assert(std::is_same_v<BASE, NoBaseClass> || std::is_base_of_v<BASE, T>); \
		return NAME; \
	} \
	struct InitTypeInfo : public ::ax::Reflection::InitTypeInfo_<This, Base, _InitTypeInfoName> \
//---------

// #define AX_TYPE_INFO(T, BASE)	AX_TYPE_INFO_WITH_NAME(AX_WRAP(T), BASE, #T)
#define AX_TYPE_INFO(T, BASE) struct InitTypeInfo 


//------ FIELD --------
#define AX_FIELD_INFO_WITH_NAME(V, NAME) struct V
// #define AX_FIELD_INFO_WITH_NAME(V, NAME) \
// 	static StrView _FieldInfoName_##V() { return NAME; } \
// 	struct V : public ::ax::Reflection::FieldInfo_<This, decltype(This::V), &This::V, _FieldInfoName_##V> \
//----

// #define AX_FIELD_INFO(V)			AX_FIELD_INFO_WITH_NAME(V, ::ax::Reflection::_makeFieldName(#V))
#define AX_FIELD_INFO(V) struct V
