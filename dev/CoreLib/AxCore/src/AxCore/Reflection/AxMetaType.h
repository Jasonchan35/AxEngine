#pragma once

#define AX_TYPE_INFO(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
public: \
	using _TYPE_INFO_This = T; \
	using _TYPE_INFO_Base = BASE; \
private: \
//------

#define AX_META_TYPE()		public MetaTypeInit_Helper_<_TYPE_INFO_This>

#define AX_META_FIELD(V) \
	static NameId _FieldName_##V() { static NameId s(#V); return s; } \
	struct V :	public MetaFieldInit_Helper_<_TYPE_INFO_This \
											, decltype(_TYPE_INFO_This::V) \
											, &_TYPE_INFO_This::V \
											, _FieldName_##V > \
//------

#define AX_META_TYPE_INIT_SIMPLE(T) \
		template<> struct MetaTypeInit_Handler_<T> { \
			static NameId _type_name() { static NameId s(#T); return s; } \
			using MetaTypeInit = MetaTypeInit_Simple_<T, _type_name>; \
		}; \
//------
