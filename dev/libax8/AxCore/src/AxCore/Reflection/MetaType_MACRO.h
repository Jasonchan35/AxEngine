#pragma once

#define AX_DOWNCAST_GET_INSTANCE() \
	AX_INLINE static This* s_instance() { return static_cast<This*>(Base::s_instance()); }
//---------

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
	static NameId _make_field_name_##V() { return NameId::s_make(#V); } \
	struct V :	public MetaFieldInit_Helper_<_TYPE_INFO_This \
											, decltype(_TYPE_INFO_This::V) \
											, &_TYPE_INFO_This::V \
											, _make_field_name_##V > \
//------

#define AX_META_TYPE_INIT_SIMPLE(T) \
		template<> struct MetaTypeInit_Handler_<T> { \
			static NameId _type_name() { return NameId::s_make(#T); } \
			using MetaTypeInit = MetaTypeInit_Simple_<T, _type_name>; \
		}; \
//------
