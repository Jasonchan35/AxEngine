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

#define AX_META_TYPE(T)  public MetaType_Make_<T, ([]()->StrView{ return #T; }) >
#define AX_META_FIELD(V) public MetaField_Make_<_TYPE_INFO_This, decltype(_TYPE_INFO_This::V), &_TYPE_INFO_This::V, ([]()->StrView{ return #V; }) > 
//------

// #define AX_META_TYPE(...)  public MetaType_Make_< __VA_ARGS__, ([]()->StrView{ return #__VA_ARGS__; }) >
// #define AX_META_FIELD(...) \
// 	public MetaField_Make_< \
// 		_TYPE_INFO_This, \
// 		decltype(_TYPE_INFO_This::__VA_ARGS__), \
// 		&_TYPE_INFO_This::__VA_ARGS__, \
// 		([]()->StrView{ return #__VA_ARGS__; }) \
// 	> \
// //------

#define AX_SIMPLE_META_TYPE(T) template<> struct MetaTypeOf_Handler_<T> { using MetaType = MetaType_Simple_<T, ([]()->StrView{ return #T; }) >; };
//------