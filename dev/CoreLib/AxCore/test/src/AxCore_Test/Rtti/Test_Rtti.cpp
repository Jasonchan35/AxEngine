#if 1
#include "AxUnitTest.h"
//----- Reflection

#define AX_TYPE_INFO(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
private: \
//------

#define AX_RTTI_CLASS(T, BASE) \
	AX_TYPE_INFO(T, BASE) \
public: \
	static  Rtti* s_rtti ()				{ return rttiOf<T>(); } \
	virtual Rtti* rtti() const override	{ return rttiOf<T>(); } \
private: \
//-----------

#define AX_META_TYPE(T, BASE) MetaType_<T, BASE, []()->StrView{ return #T; } >
#define AX_META_FIELD(V) MetaField_<MetaThis, &MetaThis::V, []()->StrView{ return #V; } >

import AxCore.MetaType;
import AxCore.Rtti;

namespace ax {

class Test_Rtti : public UnitTestClass {
public:

	template<class T>
	class Foo {
		AX_TYPE_INFO(Foo, NoBaseClass)
	public:
		struct MetaType;
		int x, y, z;
	};

	template<class T>
	class Bar : public Foo<T> {
		AX_TYPE_INFO(Bar, Foo<T>)
	public:
		struct MetaType;
		int bar;
	}; 

	class MyObject : public RttiObject {
		AX_RTTI_CLASS(MyObject, RttiObject)
	};

	void test_case1() {
		using TestFoo = Foo<void>;
		using TestBar = Bar<void>;
		
		using TI = MetaTypeOf_< TestBar >;
		// AX_TEST_EQ(TI::s_name(),     "Foo");
		// AX_TEST_EQ(TI::x::s_name(),  "x");
		//		AX_TEST_EQ(TI::x::s_offset(), 0);

		Rtti* ti = rttiOf< TestBar >();
		AX_UNUSED(ti);

		AX_LOG("name = {}", ti->name);
	}
};

// struct Test_Rtti::MyObject::MetaType : public NoBaseClass::MetaType {};

template<class T>
struct Test_Rtti::Foo<T>::MetaType : public NoBaseClass::MetaType {
	using MetaThis = Foo<T>;
	static NameId s_name() { return AX_NAME("Foo"); }

	struct x : public AX_META_FIELD(x) {};
	struct y : public AX_META_FIELD(y) {};

	using OwnFields = Tuple<x, y>; 
};

template<class T>
struct Test_Rtti::Bar<T>::MetaType : public Foo<T>::MetaType {
	using MetaThis = Bar<T>;
//	static NameId s_name() { return NameId("Bar"); }
};

} // namespace

void Test_Rtti() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Rtti::test_case1)
}
#else

void Test_Rtti() {}

#endif