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

#define AX_META_TYPE_INIT(T)  public MetaTypeInit_Helper_<T>
#define AX_META_FIELD_INIT(V) public InitMetaField_Helper_<_TYPE_INFO_This, decltype(_TYPE_INFO_This::V), &_TYPE_INFO_This::V, ([]()->StrView{ return #V; }) > 
//------

// #define AX_META_TYPE_INIT(...)  public MetaTypeInit_Helper_< __VA_ARGS__, ([]()->StrView{ return #__VA_ARGS__; }) >
// #define AX_META_FIELD_INIT(...) \
// 	public InitMetaField_Helper_< \
// 		_TYPE_INFO_This, \
// 		decltype(_TYPE_INFO_This::__VA_ARGS__), \
// 		&_TYPE_INFO_This::__VA_ARGS__, \
// 		([]()->StrView{ return #__VA_ARGS__; }) \
// 	> \
// //------

#define AX_INIT_META_TYPE_SIMPLE(T) template<> struct MetaTypeInit_Handler_<T> { using MetaTypeInit = MetaTypeInit_Simple_<T, ([]()->StrView{ return #T; }) >; };
//------