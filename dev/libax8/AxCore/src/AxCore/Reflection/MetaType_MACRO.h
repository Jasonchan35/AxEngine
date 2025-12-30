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
	struct _INIT_##V : public MetaFieldBase { \
		using FieldType = decltype(_TYPE_INFO_This::V); \
		static NameId s_name() { return NameId::s_make(#V); } \
		static Int s_offset() { return MemUtil::memberOffset(&_TYPE_INFO_This::V); } \
	}; \
	struct V :	public _INIT_##V
//------

#define AX_META_TYPE_INIT_SIMPLE(T) \
	template<> struct MetaTypeInit_Handler_<T> { \
		struct MetaTypeInit : public IMetaTypeInit { \
			using This = T; \
			using Base = NoBaseClass; \
			static NameId s_name() { static auto s = NameId::s_make(#T); return s; } \
		}; \
	}; \
//------
