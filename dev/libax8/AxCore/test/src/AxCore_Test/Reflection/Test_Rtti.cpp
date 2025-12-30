
#include "AxCore/Reflection/MetaType_MACRO.h"
#include "AxCore/Reflection/Rtti_MACRO.h"

import AxCore_Test.PCH;

import AxCore.MetaType;
import AxCore.Rtti;

namespace ax {

class Test_Rtti : public UnitTestClass {
public:
	template<class T>
	class Foo {
	public:
		int x, y;
		AX_META_TYPE(Foo, NoBaseClass) {
			AX_META_FIELD(x) {};
			AX_META_FIELD(y) {};
			using OwnFields = Tuple<x,y>;
		};
	};

	template<class T, Int N>
	class Bar : public Foo<T> {
		AX_META_TYPE_EX(Bar, Foo<T>)
	public:
		int bar;
	}; 

	template<class T, f32 N>
	class Bar2_NoInitMetaType : public Foo<T> {
		AX_META_TYPE(Bar2_NoInitMetaType, Foo<T>) {};
	public:
		int bar2;
	}; 
	
	class MyObject : public RttiObject {
		AX_RTTI_INFO(MyObject, RttiObject)
		
	};
	
	void test_case1();
};

//template<class T, Int N>
//struct Test_Rtti::Bar<T, N>::MetaTypeInit : AX_META_TYPE_INIT(AX_WRAP(Bar<T,N>)) {

template<class T, Int N>
struct Test_Rtti::Bar<T, N>::MetaTypeInit : public MetaTypeInit_Helper_<_TYPE_INFO_This> {
//	static NameId s_name() { return NameId("Bar"); }
	AX_META_FIELD(bar) {};
	using OwnFields = Tuple<bar>;
};

void Test_Rtti::test_case1() {
	{
		Rtti* ti = rttiOf< Foo<void> >();
//		ti->debugDump();
		AX_TEST_EQ(ti->allFields.size(), 2);
		AX_TEST_EQ(ti->allFields[0]->name, AX_NAMEID("x"));
		AX_TEST_EQ(ti->allFields[1]->name, AX_NAMEID("y"));
	}
		
	{
		Rtti* ti = rttiOf< Bar<StrView, 99> >();
		ti->debugDump();
		AX_TEST_EQ(ti->allFields.size(), 3);
		AX_TEST_EQ(ti->allFields[0]->name, AX_NAMEID("x"));
		AX_TEST_EQ(ti->allFields[1]->name, AX_NAMEID("y"));
		AX_TEST_EQ(ti->allFields[2]->name, AX_NAMEID("bar"));
		// own fields
		AX_TEST_EQ(ti->ownFields[0]->name, AX_NAMEID("bar"));
	}

	{
		Rtti* ti = rttiOf< Bar2_NoInitMetaType<float, 1.1f> >();
//		ti->debugDump();
		AX_TEST_EQ(ti->allFields.size(), 2);
		AX_TEST_EQ(ti->allFields[0]->name, AX_NAMEID("x"));
		AX_TEST_EQ(ti->allFields[1]->name, AX_NAMEID("y"));
	}
	
	{
		Rtti* ti = rttiOf<MyObject>();
		AX_TEST_EQ(ti->name, AX_NAMEID("class ax::Test_Rtti::MyObject"));
	}
	
}

} // namespace

void Test_Rtti() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Rtti::test_case1)
}
